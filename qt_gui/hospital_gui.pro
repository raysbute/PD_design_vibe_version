QT += core gui widgets
TARGET = hospital_gui
TEMPLATE = app
CONFIG += c++17

# Workaround for Qt 6.x + g++ 8.1.0 noexcept incompatibility
# Undefine __cpp_noexcept_function_type so Qt falls back to _NON_NOEXCEPT macros
QMAKE_CXXFLAGS += -U__cpp_noexcept_function_type
QMAKE_CXXFLAGS_RELEASE += -U__cpp_noexcept_function_type

SOURCES += ../global.cpp ../list_ops.cpp ../utils.cpp ../init_data.cpp ../persistence.cpp ../batch_input.cpp
SOURCES += main_window.cpp admin_panels.cpp doctor_panels.cpp patient_panels.cpp qt_main.cpp
HEADERS += ../global.h ../entities.h ../list_ops.h ../utils.h ../init_data.h ../persistence.h ../batch_input.h
HEADERS += main_window.h admin_panels.h doctor_panels.h patient_panels.h
