/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QGroupBox *connectionGroup;
    QGridLayout *connectionLayout;
    QLabel *portLabel;
    QComboBox *portCombo;
    QPushButton *refreshButton;
    QLabel *baudLabel;
    QComboBox *baudCombo;
    QLabel *addressLabel;
    QSpinBox *addressSpin;
    QLabel *dataBitsLabel;
    QComboBox *dataBitsCombo;
    QLabel *parityLabel;
    QComboBox *parityCombo;
    QLabel *stopBitsLabel;
    QComboBox *stopBitsCombo;
    QGroupBox *firmwareGroup;
    QHBoxLayout *firmwareLayout;
    QLineEdit *firmwareEdit;
    QPushButton *browseButton;
    QProgressBar *progressBar;
    QGroupBox *logGroup;
    QVBoxLayout *logLayout;
    QPlainTextEdit *logText;
    QHBoxLayout *buttonsLayout;
    QSpacerItem *buttonSpacer;
    QPushButton *startButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setMinimumSize(QSize(700, 540));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        connectionGroup = new QGroupBox(centralWidget);
        connectionGroup->setObjectName(QStringLiteral("connectionGroup"));
        connectionLayout = new QGridLayout(connectionGroup);
        connectionLayout->setObjectName(QStringLiteral("connectionLayout"));
        portLabel = new QLabel(connectionGroup);
        portLabel->setObjectName(QStringLiteral("portLabel"));

        connectionLayout->addWidget(portLabel, 0, 0, 1, 1);

        portCombo = new QComboBox(connectionGroup);
        portCombo->setObjectName(QStringLiteral("portCombo"));

        connectionLayout->addWidget(portCombo, 0, 1, 1, 1);

        refreshButton = new QPushButton(connectionGroup);
        refreshButton->setObjectName(QStringLiteral("refreshButton"));

        connectionLayout->addWidget(refreshButton, 0, 2, 1, 1);

        baudLabel = new QLabel(connectionGroup);
        baudLabel->setObjectName(QStringLiteral("baudLabel"));

        connectionLayout->addWidget(baudLabel, 1, 0, 1, 1);

        baudCombo = new QComboBox(connectionGroup);
        baudCombo->setObjectName(QStringLiteral("baudCombo"));

        connectionLayout->addWidget(baudCombo, 1, 1, 1, 1);

        addressLabel = new QLabel(connectionGroup);
        addressLabel->setObjectName(QStringLiteral("addressLabel"));

        connectionLayout->addWidget(addressLabel, 1, 2, 1, 1);

        addressSpin = new QSpinBox(connectionGroup);
        addressSpin->setObjectName(QStringLiteral("addressSpin"));

        connectionLayout->addWidget(addressSpin, 1, 3, 1, 1);

        dataBitsLabel = new QLabel(connectionGroup);
        dataBitsLabel->setObjectName(QStringLiteral("dataBitsLabel"));

        connectionLayout->addWidget(dataBitsLabel, 2, 0, 1, 1);

        dataBitsCombo = new QComboBox(connectionGroup);
        dataBitsCombo->setObjectName(QStringLiteral("dataBitsCombo"));

        connectionLayout->addWidget(dataBitsCombo, 2, 1, 1, 1);

        parityLabel = new QLabel(connectionGroup);
        parityLabel->setObjectName(QStringLiteral("parityLabel"));

        connectionLayout->addWidget(parityLabel, 2, 2, 1, 1);

        parityCombo = new QComboBox(connectionGroup);
        parityCombo->setObjectName(QStringLiteral("parityCombo"));

        connectionLayout->addWidget(parityCombo, 2, 3, 1, 1);

        stopBitsLabel = new QLabel(connectionGroup);
        stopBitsLabel->setObjectName(QStringLiteral("stopBitsLabel"));

        connectionLayout->addWidget(stopBitsLabel, 3, 0, 1, 1);

        stopBitsCombo = new QComboBox(connectionGroup);
        stopBitsCombo->setObjectName(QStringLiteral("stopBitsCombo"));

        connectionLayout->addWidget(stopBitsCombo, 3, 1, 1, 1);


        mainLayout->addWidget(connectionGroup);

        firmwareGroup = new QGroupBox(centralWidget);
        firmwareGroup->setObjectName(QStringLiteral("firmwareGroup"));
        firmwareLayout = new QHBoxLayout(firmwareGroup);
        firmwareLayout->setObjectName(QStringLiteral("firmwareLayout"));
        firmwareEdit = new QLineEdit(firmwareGroup);
        firmwareEdit->setObjectName(QStringLiteral("firmwareEdit"));

        firmwareLayout->addWidget(firmwareEdit);

        browseButton = new QPushButton(firmwareGroup);
        browseButton->setObjectName(QStringLiteral("browseButton"));

        firmwareLayout->addWidget(browseButton);


        mainLayout->addWidget(firmwareGroup);

        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        mainLayout->addWidget(progressBar);

        logGroup = new QGroupBox(centralWidget);
        logGroup->setObjectName(QStringLiteral("logGroup"));
        logLayout = new QVBoxLayout(logGroup);
        logLayout->setObjectName(QStringLiteral("logLayout"));
        logText = new QPlainTextEdit(logGroup);
        logText->setObjectName(QStringLiteral("logText"));
        logText->setReadOnly(true);
        logText->setMaximumBlockCount(2000);

        logLayout->addWidget(logText);


        mainLayout->addWidget(logGroup);

        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setObjectName(QStringLiteral("buttonsLayout"));
        buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonsLayout->addItem(buttonSpacer);

        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setMinimumSize(QSize(170, 36));

        buttonsLayout->addWidget(startButton);


        mainLayout->addLayout(buttonsLayout);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MBLoader - Firmware Programmer", Q_NULLPTR));
        connectionGroup->setTitle(QApplication::translate("MainWindow", "Modbus RTU connection", Q_NULLPTR));
        portLabel->setText(QApplication::translate("MainWindow", "Serial port", Q_NULLPTR));
        refreshButton->setText(QApplication::translate("MainWindow", "Refresh", Q_NULLPTR));
        baudLabel->setText(QApplication::translate("MainWindow", "Baud rate", Q_NULLPTR));
        addressLabel->setText(QApplication::translate("MainWindow", "Slave address", Q_NULLPTR));
        dataBitsLabel->setText(QApplication::translate("MainWindow", "Data bits", Q_NULLPTR));
        parityLabel->setText(QApplication::translate("MainWindow", "Parity", Q_NULLPTR));
        stopBitsLabel->setText(QApplication::translate("MainWindow", "Stop bits", Q_NULLPTR));
        firmwareGroup->setTitle(QApplication::translate("MainWindow", "Firmware file", Q_NULLPTR));
        firmwareEdit->setPlaceholderText(QApplication::translate("MainWindow", "Select a .bin file...", Q_NULLPTR));
        browseButton->setText(QApplication::translate("MainWindow", "Browse...", Q_NULLPTR));
        progressBar->setFormat(QApplication::translate("MainWindow", "Progress %p%", Q_NULLPTR));
        logGroup->setTitle(QApplication::translate("MainWindow", "Log", Q_NULLPTR));
        startButton->setText(QApplication::translate("MainWindow", "Start update", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
