#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QDateTime>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), loader(new ModbusLoader(this)) {
    ui->setupUi(this);
    ui->baudCombo->addItem("9600", QSerialPort::Baud9600);
    ui->baudCombo->addItem("19200", QSerialPort::Baud19200);
    ui->baudCombo->addItem("38400", QSerialPort::Baud38400);
    ui->baudCombo->addItem("57600", QSerialPort::Baud57600);
    ui->baudCombo->addItem("115200", QSerialPort::Baud115200);
    ui->baudCombo->setCurrentIndex(4);
    ui->dataBitsCombo->addItem("5", QSerialPort::Data5);
    ui->dataBitsCombo->addItem("6", QSerialPort::Data6);
    ui->dataBitsCombo->addItem("7", QSerialPort::Data7);
    ui->dataBitsCombo->addItem("8", QSerialPort::Data8);
    ui->dataBitsCombo->setCurrentIndex(3);
    ui->parityCombo->addItem("None", QSerialPort::NoParity);
    ui->parityCombo->addItem("Even", QSerialPort::EvenParity);
    ui->parityCombo->addItem("Odd", QSerialPort::OddParity);
    ui->stopBitsCombo->addItem("1", QSerialPort::OneStop);
    ui->stopBitsCombo->addItem("2", QSerialPort::TwoStop);
    ui->addressSpin->setRange(1, 247);
    ui->addressSpin->setValue(1);
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshPorts()));
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseFirmware()));
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startProgramming()));
    connect(loader, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
    connect(loader, SIGNAL(progressChanged(int)), this, SLOT(setProgress(int)));
    connect(loader, SIGNAL(finished(bool,QString)), this, SLOT(finished(bool,QString)));
    refreshPorts();
    log("Ready. Choose a BIN file and serial port.");
}
MainWindow::~MainWindow() { delete ui; }
void MainWindow::refreshPorts() {
    const QString oldPort = ui->portCombo->currentData().toString();
    ui->portCombo->clear();
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (int i = 0; i < ports.size(); ++i) {
        QString label = ports.at(i).portName();
        if (!ports.at(i).description().isEmpty()) label += " - " + ports.at(i).description();
        ui->portCombo->addItem(label, ports.at(i).portName());
    }
    const int index = ui->portCombo->findData(oldPort);
    if (index >= 0) ui->portCombo->setCurrentIndex(index);
    if (ui->portCombo->count() == 0) log("No serial ports found.");
}
void MainWindow::browseFirmware() {
    const QString name = QFileDialog::getOpenFileName(this, tr("Select firmware"), QString(), tr("Firmware (*.bin);;All files (*)"));
    if (!name.isEmpty()) ui->firmwareEdit->setText(name);
}
void MainWindow::startProgramming() {
    if (ui->portCombo->currentData().toString().isEmpty()) { QMessageBox::warning(this, tr("Port"), tr("Select a serial port.")); return; }
    if (ui->firmwareEdit->text().trimmed().isEmpty()) { QMessageBox::warning(this, tr("Firmware"), tr("Select a BIN firmware file.")); return; }
    ModbusLoader::Settings s;
    s.portName = ui->portCombo->currentData().toString();
    s.baudRate = static_cast<QSerialPort::BaudRate>(ui->baudCombo->currentData().toInt());
    s.dataBits = static_cast<QSerialPort::DataBits>(ui->dataBitsCombo->currentData().toInt());
    s.parity = static_cast<QSerialPort::Parity>(ui->parityCombo->currentData().toInt());
    s.stopBits = static_cast<QSerialPort::StopBits>(ui->stopBitsCombo->currentData().toInt());
    s.address = ui->addressSpin->value();
    ui->progressBar->setValue(0);
    setBusy(true);
    loader->start(ui->firmwareEdit->text().trimmed(), s);
}
void MainWindow::showMessage(const QString &message) { log(message); }
void MainWindow::setProgress(int value) { ui->progressBar->setValue(value); }
void MainWindow::finished(bool success, const QString &message) {
    setBusy(false);
    log(message);
    if (success) QMessageBox::information(this, tr("MBLoader"), message);
    else QMessageBox::critical(this, tr("MBLoader error"), message);
}
void MainWindow::setBusy(bool busy) {
    ui->startButton->setEnabled(!busy); ui->browseButton->setEnabled(!busy); ui->refreshButton->setEnabled(!busy);
    ui->portCombo->setEnabled(!busy); ui->baudCombo->setEnabled(!busy); ui->dataBitsCombo->setEnabled(!busy);
    ui->parityCombo->setEnabled(!busy); ui->stopBitsCombo->setEnabled(!busy); ui->addressSpin->setEnabled(!busy);
}
void MainWindow::log(const QString &message) { ui->logText->appendPlainText(QDateTime::currentDateTime().toString("HH:mm:ss") + "  " + message); }
