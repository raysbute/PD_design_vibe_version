CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = hospital.exe
OBJS = main.o global.o list_ops.o utils.o init_data.o persistence.o menus_admin.o menus_doctor.o menus_patient.o reports.o batch_input.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp global.h entities.h list_ops.h init_data.h persistence.h utils.h menus_admin.h menus_doctor.h menus_patient.h reports.h
	$(CXX) $(CXXFLAGS) -c main.cpp

global.o: global.cpp global.h
	$(CXX) $(CXXFLAGS) -c global.cpp

list_ops.o: list_ops.cpp list_ops.h entities.h global.h
	$(CXX) $(CXXFLAGS) -c list_ops.cpp

utils.o: utils.cpp utils.h entities.h list_ops.h global.h
	$(CXX) $(CXXFLAGS) -c utils.cpp

init_data.o: init_data.cpp init_data.h entities.h list_ops.h global.h
	$(CXX) $(CXXFLAGS) -c init_data.cpp

persistence.o: persistence.cpp persistence.h entities.h list_ops.h global.h
	$(CXX) $(CXXFLAGS) -c persistence.cpp

menus_admin.o: menus_admin.cpp menus_admin.h entities.h list_ops.h global.h utils.h persistence.h reports.h
	$(CXX) $(CXXFLAGS) -c menus_admin.cpp

menus_doctor.o: menus_doctor.cpp menus_doctor.h entities.h list_ops.h global.h utils.h reports.h
	$(CXX) $(CXXFLAGS) -c menus_doctor.cpp

menus_patient.o: menus_patient.cpp menus_patient.h entities.h list_ops.h global.h utils.h
	$(CXX) $(CXXFLAGS) -c menus_patient.cpp

reports.o: reports.cpp reports.h entities.h list_ops.h global.h utils.h
	$(CXX) $(CXXFLAGS) -c reports.cpp

batch_input.o: batch_input.cpp batch_input.h entities.h list_ops.h global.h
	$(CXX) $(CXXFLAGS) -c batch_input.cpp

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
