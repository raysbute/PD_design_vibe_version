#ifndef UTILS_H
#define UTILS_H

#include "entities.h"
#include "list_ops.h"

// ==================== Robust Input Functions ====================
// These functions handle all input errors gracefully, never crash

// Read an integer from stdin, with retry on error
int readInt(const char* prompt = "请输入整数: ");

// Read a double from stdin, with retry on error
double readDouble(const char* prompt = "请输入数字: ");

// Read a non-empty string, trimmed, max length enforced
std::string readString(const char* prompt = "请输入: ", int maxLen = MAX_NAME_LEN);

// Read a string that can be empty (optional field)
std::string readOptionalString(const char* prompt = "请输入: ", int maxLen = MAX_DESC_LEN);

// Read a yes/no confirmation
bool readConfirm(const char* prompt = "确认? (y/n): ");

// Read an integer in a range [min, max]
int readIntRange(const char* prompt, int minVal, int maxVal);

// Read a non-negative double
double readNonNegDouble(const char* prompt = "请输入金额: ");

// Clear input buffer
void clearInputBuffer();

// Pause and wait for enter
void pauseScreen(const char* msg = "\n按回车键继续...");

// ==================== Validation Functions ====================
bool isValidName(const std::string& name);
bool isValidContact(const std::string& contact);
bool isValidAge(int age);

// Check if a doctor works on a given day of week (1=Monday, etc.)
bool doctorWorksOnDay(DoctorNode* doctor, int dayOfWeek);

// Check if a department exists
bool deptExists(DepartmentNode* head, const std::string& deptID);

// Check if a medicine is linked to a department
bool isMedicineInDept(DeptMedicineNode* dmHead, const std::string& deptID, const std::string& medID);

// ==================== Display Helpers ====================
void printLine(int len = 60, char ch = '-');
void printTitle(const char* title, int len = 60);

// Display time and money status
void printStatus();

// Print doctor level name
const char* getLevelName(int level);

// Print ward type name
const char* getWardTypeName(int type);

// Print status strings for various entities
const char* getRegStatusStr(int status);
const char* getRecordStatusStr(int status);
const char* getHospStatusStr(int status);
const char* getPrescStatusStr(int status);

#endif
