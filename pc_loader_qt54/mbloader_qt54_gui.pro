QT += core gui widgets serialport
TEMPLATE = app
TARGET = mbloader_qt54_gui
CONFIG += c++11
CONFIG -= app_bundle
SOURCES += src/main.cpp src/mainwindow.cpp src/modbusloader.cpp
HEADERS += src/mainwindow.h src/modbusloader.h
FORMS += src/mainwindow.ui
