#include "batch_input.h"
#include <cstdio>
#include <cstdlib>
#include <vector>

// Split a string by delimiter
static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        // Trim item
        size_t start = 0;
        while (start < item.length() && (item[start] == ' ' || item[start] == '\t')) start++;
        size_t end = item.length();
        while (end > start && (item[end-1] == ' ' || item[end-1] == '\t')) end--;
        result.push_back(item.substr(start, end - start));
    }
    return result;
}

// Parse workdays from comma-separated string
static int parseWorkDays(const std::string& str, int* days) {
    if (str.empty()) return 0;
    std::vector<std::string> parts = split(str, ',');
    int count = 0;
    for (size_t i = 0; i < parts.size() && count < MAX_WORKDAYS; i++) {
        int d = atoi(parts[i].c_str());
        if (d >= 1 && d <= 7) {
            // Check for duplicates
            bool dup = false;
            for (int j = 0; j < count; j++) {
                if (days[j] == d) { dup = true; break; }
            }
            if (!dup) days[count++] = d;
        }
    }
    return count;
}

// Parse a single line of batch input
static int parseLine(const std::string& line,
    DepartmentNode*& deptHead,
    DoctorNode*& doctorHead,
    PatientNode*& patientHead,
    RegistrationNode*& regHead,
    MedicineNode*& medHead,
    DeptMedicineNode*& deptMedHead,
    WardNode*& wardHead,
    AdminNode*& adminHead) {

    std::vector<std::string> parts = split(line, '|');
    if (parts.empty()) return 0;

    const std::string& type = parts[0];

    if (type == "DEPT") {
        if (parts.size() < 3) { std::cerr << "[错误] DEPT格式: name|description" << std::endl; return 0; }
        DepartmentNode* node = new DepartmentNode();
        node->deptID = generateDeptID();
        node->name = parts[1];
        node->description = parts[2];
        insertDepartmentTail(deptHead, node);
        return 1;
    }

    if (type == "DOCTOR") {
        if (parts.size() < 5) { std::cerr << "[错误] DOCTOR格式: name|level|deptID|workDays" << std::endl; return 0; }
        DoctorNode* node = new DoctorNode();
        node->doctorID = generateDoctorID();
        node->name = parts[1];
        node->level = atoi(parts[2].c_str());
        if (node->level < 0 || node->level >= MAX_DOCTOR_LEVELS) node->level = 3;
        node->departmentID = parts[3];
        node->workDayCount = parseWorkDays(parts[4], node->workDays);
        insertDoctorTail(doctorHead, node);
        return 1;
    }

    if (type == "PATIENT") {
        if (parts.size() < 5) { std::cerr << "[错误] PATIENT格式: name|age|contact|type(0/1)" << std::endl; return 0; }
        PatientNode* node = new PatientNode();
        node->patientID = generatePatientID();
        node->name = parts[1];
        node->age = atoi(parts[2].c_str());
        node->contact = parts[3];
        node->type = atoi(parts[4].c_str());
        insertPatientTail(patientHead, node);
        return 1;
    }

    if (type == "MEDICINE") {
        if (parts.size() < 6) { std::cerr << "[错误] MEDICINE格式: tradeName|genericName|alias|spec|stock" << std::endl; return 0; }
        MedicineNode* node = new MedicineNode();
        node->medID = generateMedID();
        node->tradeName = parts[1];
        node->genericName = parts[2];
        node->alias = parts[3];
        node->spec = parts[4];
        node->stock = atoi(parts[5].c_str());
        if (node->stock < 0) node->stock = 0;
        node->consumed = 0;
        insertMedicineTail(medHead, node);
        return 1;
    }

    if (type == "WARD") {
        if (parts.size() < 4) { std::cerr << "[错误] WARD格式: type(0/1)|deptID|bedCount" << std::endl; return 0; }
        WardNode* node = new WardNode();
        node->wardID = generateWardID();
        node->type = atoi(parts[1].c_str());
        node->departmentID = parts[2];
        node->bedCount = atoi(parts[3].c_str());
        if (node->bedCount > MAX_BEDS) node->bedCount = MAX_BEDS;
        if (node->bedCount < 1) node->bedCount = 1;
        insertWardTail(wardHead, node);
        return 1;
    }

    if (type == "ADMIN") {
        if (parts.size() < 3) { std::cerr << "[错误] ADMIN格式: adminID|password" << std::endl; return 0; }
        // Check for duplicate admin ID
        if (findAdminByID(adminHead, parts[1])) {
            std::cerr << "[警告] 管理员ID " << parts[1] << " 已存在，跳过。" << std::endl;
            return 0;
        }
        AdminNode* node = new AdminNode();
        node->adminID = parts[1];
        node->password = parts[2];
        insertAdminTail(adminHead, node);
        return 1;
    }

    if (type == "REG") {
        if (parts.size() < 4) { std::cerr << "[错误] REG格式: patientID|deptID|doctorID" << std::endl; return 0; }
        RegistrationNode* node = new RegistrationNode();
        node->regID = generateRegID(weekday);
        node->patientID = parts[1];
        node->departmentID = parts[2];
        node->doctorID = parts[3];
        node->regDay = weekday;
        node->status = STATUS_PENDING;
        insertRegistrationTail(regHead, node);
        return 1;
    }

    if (type == "DEPTMED") {
        if (parts.size() < 3) { std::cerr << "[错误] DEPTMED格式: deptID|medID" << std::endl; return 0; }
        if (findDeptMedicine(deptMedHead, parts[1], parts[2])) {
            std::cerr << "[警告] 科室药品关联已存在，跳过。" << std::endl;
            return 0;
        }
        DeptMedicineNode* node = new DeptMedicineNode();
        node->deptID = parts[1];
        node->medID = parts[2];
        insertDeptMedicineTail(deptMedHead, node);
        return 1;
    }

    std::cerr << "[警告] 未知实体类型: " << type << "，跳过。" << std::endl;
    return 0;
}

// Parse batch input from a text file
int batchImportFromFile(const char* filename,
    DepartmentNode*& deptHead,
    DoctorNode*& doctorHead,
    PatientNode*& patientHead,
    RegistrationNode*& regHead,
    MedicineNode*& medHead,
    DeptMedicineNode*& deptMedHead,
    WardNode*& wardHead,
    AdminNode*& adminHead) {

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "[错误] 无法打开文件: " << filename << std::endl;
        return -1;
    }

    int count = 0;
    int lineNo = 0;
    std::string line;
    while (std::getline(file, line)) {
        lineNo++;
        // Trim trailing \r (Windows line endings)
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // Skip empty lines and comments
        if (line.empty()) continue;
        if (line[0] == '#' || line[0] == ';') continue;

        count += parseLine(line, deptHead, doctorHead, patientHead, regHead,
                          medHead, deptMedHead, wardHead, adminHead);
    }
    file.close();

    std::cout << "[提示] 批量导入完成: 共处理 " << lineNo << " 行，成功导入 " << count << " 条记录。" << std::endl;
    return count;
}

// Batch input from standard input
int batchImportFromStdin(
    DepartmentNode*& deptHead,
    DoctorNode*& doctorHead,
    PatientNode*& patientHead,
    RegistrationNode*& regHead,
    MedicineNode*& medHead,
    DeptMedicineNode*& deptMedHead,
    WardNode*& wardHead,
    AdminNode*& adminHead) {

    std::cout << "=== 批量输入模式 ===" << std::endl;
    std::cout << "格式: ENTITY|field1|field2|..." << std::endl;
    std::cout << "支持类型: DEPT, DOCTOR, PATIENT, MEDICINE, WARD, ADMIN, REG, DEPTMED" << std::endl;
    std::cout << "输入空行结束输入。" << std::endl;
    std::cout << "格式示例:" << std::endl;
    std::cout << "  DEPT|内科学|诊治内科疾病" << std::endl;
    std::cout << "  DOCTOR|张医生|0|DEPT001|1,3,5" << std::endl;
    std::cout << "  PATIENT|王某某|30|1380000|0" << std::endl;
    std::cout << "  MEDICINE|阿莫西林|阿莫西林||0.25g*24|500" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    int count = 0;
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break;
        if (line[0] == '#' || line[0] == ';') continue;

        count += parseLine(line, deptHead, doctorHead, patientHead, regHead,
                          medHead, deptMedHead, wardHead, adminHead);
    }

    std::cout << "[提示] 批量输入完成: 成功导入 " << count << " 条记录。" << std::endl;
    return count;
}
