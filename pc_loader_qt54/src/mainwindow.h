#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "modbusloader.h"
namespace Ui { class MainWindow; }
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void refreshPorts();
    void browseFirmware();
    void startProgramming();
    void showMessage(const QString &message);
    void setProgress(int value);
    void finished(bool success, const QString &message);
private:
    Ui::MainWindow *ui;
    ModbusLoader *loader;
    void setBusy(bool busy);
    void log(const QString &message);
};
#endif
