#ifndef MODBUSLOADER_H
#define MODBUSLOADER_H
#include <QObject>
#include <QFile>
#include <QSerialPort>
#include <QTimer>
class ModbusLoader : public QObject {
    Q_OBJECT
public:
    struct Settings { QString portName; QSerialPort::BaudRate baudRate; QSerialPort::DataBits dataBits; QSerialPort::Parity parity; QSerialPort::StopBits stopBits; int address; };
    explicit ModbusLoader(QObject *parent = 0);
    void start(const QString &fileName, const Settings &settings);
signals:
    void message(const QString &message);
    void progressChanged(int value);
    void finished(bool success, const QString &message);
private slots:
    void onReadyRead();
    void onTimeout();
private:
    enum State { Idle, Erasing, Programming, Checking, Resetting };
    QSerialPort *serial;
    QTimer *timer;
    QFile firmware;
    Settings settings;
    State state;
    QByteArray rx;
    quint8 checksum;
    qint64 fileSize;
    qint64 sentBytes;
    bool active;
    bool openPort();
    void sendCommand(quint8 subfunction, const QByteArray &payload = QByteArray());
    void processResponse(const QByteArray &frame);
    void sendNextBlock();
    void fail(const QString &reason);
    void succeed();
    QByteArray frame(quint8 subfunction, const QByteArray &payload) const;
    static quint16 crc16(const QByteArray &data);
};
#endif
