QT += core gui widgets charts
TARGET = hospital_gui
TEMPLATE = app
CONFIG += c++11
SOURCES += ../global.cpp ../list_ops.cpp ../utils.cpp ../init_data.cpp ../persistence.cpp ../batch_input.cpp
SOURCES += main_window.cpp admin_panels.cpp doctor_panels.cpp patient_panels.cpp qt_main.cpp
HEADERS += ../global.h ../entities.h ../list_ops.h ../utils.h ../init_data.h ../persistence.h ../batch_input.h
HEADERS += main_window.h admin_panels.h doctor_panels.h patient_panels.h
