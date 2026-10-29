#include "modbusloader.h"
#include <QFileInfo>
static const quint8 MBLOADER_FUNCTION = 0x6C;
static const int DATA_BLOCK_SIZE = 246;
static const int RESPONSE_TIMEOUT_MS = 5000;
ModbusLoader::ModbusLoader(QObject *parent) : QObject(parent), serial(new QSerialPort(this)), timer(new QTimer(this)), state(Idle), checksum(0), fileSize(0), sentBytes(0), active(false) {
    timer->setSingleShot(true);
    connect(serial, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}
void ModbusLoader::start(const QString &fileName, const Settings &s) {
    if (active) { emit finished(false, "An update is already running."); return; }
    firmware.setFileName(fileName);
    if (QFileInfo(fileName).suffix().compare("bin", Qt::CaseInsensitive) != 0 || !firmware.open(QIODevice::ReadOnly)) {
        emit finished(false, "Cannot open firmware BIN file: " + firmware.errorString()); return;
    }
    if (firmware.size() <= 0) { firmware.close(); emit finished(false, "Firmware file is empty."); return; }
    settings = s; fileSize = firmware.size(); sentBytes = 0; checksum = 0; rx.clear(); state = Erasing; active = true;
    if (!openPort()) { const QString error = serial->errorString(); active = false; firmware.close(); emit finished(false, "Cannot open serial port: " + error); return; }
    emit message(QString("Port %1 opened at %2 baud; slave address %3.").arg(s.portName).arg(static_cast<int>(s.baudRate)).arg(s.address));
    emit message(QString("Firmware size: %1 bytes.").arg(fileSize));
    emit message("Erasing flash..."); sendCommand(0x00);
}
bool ModbusLoader::openPort() {
    if (serial->isOpen()) serial->close();
    serial->setPortName(settings.portName); serial->setBaudRate(settings.baudRate); serial->setDataBits(settings.dataBits);
    serial->setParity(settings.parity); serial->setStopBits(settings.stopBits); serial->setFlowControl(QSerialPort::NoFlowControl);
    if (!serial->open(QIODevice::ReadWrite)) return false;
    serial->clear(QSerialPort::AllDirections); return true;
}
QByteArray ModbusLoader::frame(quint8 subfunction, const QByteArray &payload) const {
    QByteArray out; out.append(static_cast<char>(settings.address)); out.append(static_cast<char>(MBLOADER_FUNCTION)); out.append(static_cast<char>(subfunction)); out.append(payload);
    const quint16 crc = crc16(out); out.append(static_cast<char>(crc & 0xFF)); out.append(static_cast<char>((crc >> 8) & 0xFF)); return out;
}
void ModbusLoader::sendCommand(quint8 subfunction, const QByteArray &payload) {
    if (!active) return;
    rx.clear(); serial->clear(QSerialPort::Input);
    const QByteArray out = frame(subfunction, payload);
    if (serial->write(out) != out.size()) { fail("Serial write failed: " + serial->errorString()); return; }
    timer->start(RESPONSE_TIMEOUT_MS);
}
void ModbusLoader::onReadyRead() {
    if (!active) return;
    rx.append(serial->readAll());
    // Response RTU frame: address + function(0x6C) + subfunction + 1-byte result + CRC16 = 6 bytes.
    if (rx.size() < 6) return;
    const QByteArray response = rx.left(6);
    const quint16 got = static_cast<quint8>(response.at(4)) | (static_cast<quint16>(static_cast<quint8>(response.at(5))) << 8);
    if (crc16(response.left(4)) != got) { fail("Invalid CRC in response: " + QString(response.toHex())); return; }
    if (static_cast<quint8>(response.at(0)) != settings.address || static_cast<quint8>(response.at(1)) != MBLOADER_FUNCTION) { fail("Unexpected response: " + QString(response.toHex())); return; }
    rx.remove(0, 6); timer->stop(); processResponse(response);
}
void ModbusLoader::processResponse(const QByteArray &response) {
    const quint8 sub = static_cast<quint8>(response.at(2));
    const quint8 result = static_cast<quint8>(response.at(3));
    if (state == Erasing) {
        if (sub != 0x00 || result != 0) { fail(QString("Flash erase failed; status 0x%1").arg(result, 2, 16, QLatin1Char('0'))); return; }
        emit message("Flash erased."); state = Programming; sendNextBlock(); return;
    }
    if (state == Programming) {
        if (sub != 0x01 || result != 0) { fail(QString("Flash write failed; status 0x%1").arg(result, 2, 16, QLatin1Char('0'))); return; }
        sendNextBlock(); return;
    }
    if (state == Checking) {
        if (sub != 0x02) { fail("Unexpected checksum response."); return; }
        if (result != checksum) { fail(QString("Checksum mismatch: host 0x%1, device 0x%2").arg(checksum, 2, 16, QLatin1Char('0')).arg(result, 2, 16, QLatin1Char('0'))); return; }
        emit message(QString("Checksum OK: 0x%1").arg(checksum, 2, 16, QLatin1Char('0'))); state = Resetting; emit message("Resetting device..."); sendCommand(0x03); return;
    }
    if (state == Resetting) {
        if (sub != 0x03 || result != 0) { fail(QString("Reset failed; status 0x%1").arg(result, 2, 16, QLatin1Char('0'))); return; }
        succeed(); return;
    }
    fail("Response received in an invalid state.");
}
void ModbusLoader::sendNextBlock() {
    if (!active) return;
    if (firmware.atEnd()) { emit progressChanged(100); emit message("Firmware written. Checking checksum..."); state = Checking; sendCommand(0x02); return; }
    const quint32 offset = static_cast<quint32>(firmware.pos());
    const QByteArray data = firmware.read(DATA_BLOCK_SIZE);
    if (data.isEmpty()) { fail("Failed to read firmware data: " + firmware.errorString()); return; }
    for (int i = 0; i < data.size(); ++i) checksum = static_cast<quint8>(checksum + static_cast<quint8>(data.at(i)));
    QByteArray payload; payload.append(static_cast<char>(offset & 0xFF)); payload.append(static_cast<char>((offset >> 8) & 0xFF)); payload.append(static_cast<char>((offset >> 16) & 0xFF)); payload.append(static_cast<char>((offset >> 24) & 0xFF)); payload.append(data);
    sentBytes += data.size(); emit progressChanged(static_cast<int>((sentBytes * 100) / fileSize));
    emit message(QString("Writing firmware: %1/%2 bytes").arg(sentBytes).arg(fileSize)); sendCommand(0x01, payload);
}
void ModbusLoader::onTimeout() { if (active) fail("Timed out waiting for bootloader response."); }
void ModbusLoader::fail(const QString &reason) {
    timer->stop(); active = false; state = Idle; if (firmware.isOpen()) firmware.close(); if (serial->isOpen()) serial->close();
    emit message("ERROR: " + reason); emit finished(false, reason);
}
void ModbusLoader::succeed() {
    timer->stop(); active = false; state = Idle; if (firmware.isOpen()) firmware.close(); if (serial->isOpen()) serial->close();
    emit progressChanged(100); emit message("Firmware update completed."); emit finished(true, "Firmware update completed successfully.");
}
quint16 ModbusLoader::crc16(const QByteArray &data) {
    quint16 crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) { crc ^= static_cast<quint8>(data.at(i)); for (int bit = 0; bit < 8; ++bit) crc = (crc & 1) ? static_cast<quint16>((crc >> 1) ^ 0xA001) : static_cast<quint16>(crc >> 1); }
    return crc;
}
