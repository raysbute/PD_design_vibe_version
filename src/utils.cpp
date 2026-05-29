#include "utils.h"
#include <cstdio>
#include <cctype>
#include <limits>

// ==================== Robust Input Functions ====================

void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const char* prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            clearInputBuffer();
            return val;
        }
        // Input error - clear and retry
        std::cin.clear();
        clearInputBuffer();
        std::cout << "[错误] 输入不是有效整数，请重新输入。" << std::endl;
    }
}

double readDouble(const char* prompt) {
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            clearInputBuffer();
            return val;
        }
        std::cin.clear();
        clearInputBuffer();
        std::cout << "[错误] 输入不是有效数字，请重新输入。" << std::endl;
    }
}

std::string readString(const char* prompt, int maxLen) {
    std::string input;
    while (true) {
        std::cout << prompt;
        if (std::getline(std::cin, input)) {
            // Trim leading/trailing whitespace
            size_t start = 0;
            while (start < input.length() && (input[start] == ' ' || input[start] == '\t' || input[start] == '\r'))
                start++;
            size_t end = input.length();
            while (end > start && (input[end-1] == ' ' || input[end-1] == '\t' || input[end-1] == '\r'))
                end--;
            input = input.substr(start, end - start);

            if (input.empty()) {
                std::cout << "[错误] 输入不能为空，请重新输入。" << std::endl;
                continue;
            }
            if ((int)input.length() > maxLen) {
                std::cout << "[错误] 输入过长(最大" << maxLen << "个字符)，请重新输入。" << std::endl;
                continue;
            }
            return input;
        }
    }
}

std::string readOptionalString(const char* prompt, int maxLen) {
    std::string input;
    while (true) {
        std::cout << prompt;
        if (std::getline(std::cin, input)) {
            size_t start = 0;
            while (start < input.length() && (input[start] == ' ' || input[start] == '\t' || input[start] == '\r'))
                start++;
            size_t end = input.length();
            while (end > start && (input[end-1] == ' ' || input[end-1] == '\t' || input[end-1] == '\r'))
                end--;
            input = input.substr(start, end - start);

            if ((int)input.length() > maxLen) {
                std::cout << "[错误] 输入过长(最大" << maxLen << "个字符)，请重新输入。" << std::endl;
                continue;
            }
            return input;  // Can return empty string
        }
    }
}

bool readConfirm(const char* prompt) {
    while (true) {
        std::string input = readString(prompt, 5);
        if (input == "y" || input == "Y" || input == "yes" || input == "YES" || input == "是") return true;
        if (input == "n" || input == "N" || input == "no" || input == "NO" || input == "否") return false;
        std::cout << "[错误] 请输入 y 或 n。" << std::endl;
    }
}

int readIntRange(const char* prompt, int minVal, int maxVal) {
    while (true) {
        int val = readInt(prompt);
        if (val >= minVal && val <= maxVal) return val;
        std::cout << "[错误] 输入超出范围[" << minVal << ", " << maxVal << "]，请重新输入。" << std::endl;
    }
}

double readNonNegDouble(const char* prompt) {
    while (true) {
        double val = readDouble(prompt);
        if (val >= 0.0) return round2(val);
        std::cout << "[错误] 金额不能为负数，请重新输入。" << std::endl;
    }
}

void pauseScreen(const char* msg) {
    std::cout << msg;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// ==================== Validation ====================
bool isValidName(const std::string& name) {
    if (name.empty()) return false;
    if ((int)name.length() > MAX_NAME_LEN) return false;
    return true;
}

bool isValidContact(const std::string& contact) {
    if (contact.empty()) return false;
    if ((int)contact.length() > MAX_CONTACT_LEN) return false;
    return true;
}

bool isValidAge(int age) {
    return age >= 0 && age <= 150;
}

bool doctorWorksOnDay(DoctorNode* doctor, int dayOfWeek) {
    if (!doctor) return false;
    for (int i = 0; i < doctor->workDayCount; i++) {
        if (doctor->workDays[i] == dayOfWeek) return true;
    }
    return false;
}

bool deptExists(DepartmentNode* head, const std::string& deptID) {
    return findDepartmentByID(head, deptID) != nullptr;
}

bool isMedicineInDept(DeptMedicineNode* dmHead, const std::string& deptID, const std::string& medID) {
    return findDeptMedicine(dmHead, deptID, medID);
}

// ==================== Display Helpers ====================
void printLine(int len, char ch) {
    for (int i = 0; i < len; i++) std::cout << ch;
    std::cout << std::endl;
}

void printTitle(const char* title, int len) {
    printLine(len, '=');
    int pad = (len - (int)strlen(title)) / 2;
    if (pad < 0) pad = 0;
    for (int i = 0; i < pad; i++) std::cout << ' ';
    std::cout << title << std::endl;
    printLine(len, '=');
}

void printStatus() {
    std::cout << "[" << formatTime() << "] ";
    std::cout << "医院资金: " << std::fixed << std::setprecision(2) << money << " 元" << std::endl;
}

const char* getLevelName(int level) {
    if (level >= 0 && level < MAX_DOCTOR_LEVELS) return LEVEL_NAMES[level];
    return "未知";
}

const char* getWardTypeName(int type) {
    return (type == WARD_SPECIAL) ? "特殊病房" : "普通病房";
}

const char* getRegStatusStr(int status) {
    switch (status) {
        case 0: return "待就诊";
        case 1: return "已就诊";
        case 2: return "已过期";
        case 3: return "已撤销";
        default: return "未知";
    }
}

const char* getRecordStatusStr(int status) {
    switch (status) {
        case 0: return "有效";
        case 1: return "已撤销";
        default: return "未知";
    }
}

const char* getHospStatusStr(int status) {
    switch (status) {
        case 0: return "在院";
        case 1: return "已出院";
        case 2: return "已撤销";
        default: return "未知";
    }
}

const char* getPrescStatusStr(int status) {
    switch (status) {
        case 0: return "待缴费";
        case 1: return "已撤销";
        case 2: return "已缴费";
        default: return "未知";
    }
}
