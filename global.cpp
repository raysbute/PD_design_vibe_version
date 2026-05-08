#include "global.h"

// Global variables
unsigned long long globalTime = 0;
unsigned int weekday = 1;  // = globalTime/86400 + 1
double money = 10000.00;

// Set the global time and automatically sync weekday
void setTime(unsigned long long newTime) {
    globalTime = newTime;
    weekday = (unsigned int)(newTime / 86400) + 1;
}

// ID prefix definitions
const char* PREFIX_DEPT = "DEPT";
const char* PREFIX_DOCTOR = "DOC";
const char* PREFIX_PATIENT = "PAT";
const char* PREFIX_REG = "R";
const char* PREFIX_CONSULT = "CS";
const char* PREFIX_EXAM = "EX";
const char* PREFIX_PRESC = "PR";
const char* PREFIX_MED = "MED";
const char* PREFIX_WARD = "WRD";
const char* PREFIX_HOSP = "HSP";
const char* PREFIX_ADMIN = "ADM";

// Doctor level names
const char* LEVEL_NAMES[] = {"主任医师", "副主任医师", "主治医师", "住院医师"};

// Weekday names
const char* WEEKDAY_NAMES[] = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};
