#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

// Global time and money
extern unsigned long long globalTime;  // seconds (unix-style, 0 = Day1 00:00:00)
extern unsigned int weekday;           // day count, derived from globalTime: weekday = globalTime/86400 + 1
extern double money;                   // hospital funds, precision to 2 decimal places

// Set the global time and automatically sync weekday from it
void setTime(unsigned long long newTime);

// Constants
const int MAX_NAME_LEN = 20;
const int MAX_CONTACT_LEN = 20;
const int MAX_DESC_LEN = 100;
const int MAX_ITEM_LEN = 50;
const int MAX_BEDS = 30;
const int MAX_WORKDAYS = 7;
const int MAX_DOCTOR_LEVELS = 4;
const double REGISTRATION_FEE = 20.00;
const double HOSPITAL_DAILY_FEE = 200.00;
const unsigned long long SECONDS_PER_DAY = 86400;

// ID prefixes
extern const char* PREFIX_DEPT;
extern const char* PREFIX_DOCTOR;
extern const char* PREFIX_PATIENT;
extern const char* PREFIX_REG;
extern const char* PREFIX_CONSULT;
extern const char* PREFIX_EXAM;
extern const char* PREFIX_PRESC;
extern const char* PREFIX_MED;
extern const char* PREFIX_WARD;
extern const char* PREFIX_HOSP;
extern const char* PREFIX_ADMIN;

// Status codes (context-dependent; each entity uses its own subset)
// Registration: uses PENDING(0) / SEEN(1) / EXPIRED(2) / cancelled(3 literal)
// Consultation/Examination: uses VALID(0) / CANCELLED(1)
// Prescription: uses 0(unpaid) / 1(cancelled literal) / 2(paid literal)
// Hospitalization: uses 0(in-hospital) / DISCHARGED(2) / CANCELLED(1)
const int STATUS_VALID = 0;
const int STATUS_CANCELLED = 1;
const int STATUS_PENDING = 0;
const int STATUS_SEEN = 1;
const int STATUS_EXPIRED = 2;
const int STATUS_DISCHARGED = 2;

// Patient types
const int PATIENT_OUTPATIENT = 0;
const int PATIENT_INPATIENT = 1;

// Ward types
const int WARD_REGULAR = 0;
const int WARD_SPECIAL = 1;

// Doctor levels
extern const char* LEVEL_NAMES[];

// Weekday names
extern const char* WEEKDAY_NAMES[];

// Format time as "DayX HH:MM:SS"
inline std::string formatTime() {
    unsigned int seconds = globalTime % 60;
    unsigned int minutes = (globalTime / 60) % 60;
    unsigned int hours = (globalTime / 3600) % 24;
    char buf[32];
    snprintf(buf, sizeof(buf), "Day%u %02u:%02u:%02u", weekday, hours, minutes, seconds);
    return std::string(buf);
}

// Round double to 2 decimal places
inline double round2(double val) {
    return std::round(val * 100.0) / 100.0;
}

#endif
