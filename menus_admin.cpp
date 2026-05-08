#include "menus_admin.h"
#include "utils.h"
#include "persistence.h"
#include "reports.h"

extern AdminNode* adminHead;

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

// ==================== adminLogin ====================
bool adminLogin(AdminNode* adminHead) {
    int attempts = 0;
    while (attempts < 3) {
        printTitle("管理员登录");
        string adminID = readString("请输入管理员ID (0=取消): ", 10);
        if (adminID == "0") return false;

        string password;
        cout << "请输入密码: ";
        getline(cin, password);

        AdminNode* admin = findAdminByID(adminHead, adminID);
        if (admin != nullptr && admin->password == password) {
            cout << "\n登录成功! 欢迎, " << adminID << "!" << endl;
            pauseScreen();
            return true;
        } else {
            attempts++;
            int remaining = 3 - attempts;
            cout << "ID或密码错误!" << endl;
            if (remaining > 0) {
                cout << "还有 " << remaining << " 次尝试机会。" << endl;
                pauseScreen();
            }
        }
    }
    cout << "\n尝试次数过多，登录失败。" << endl;
    pauseScreen();
    return false;
}

// ==================== Department Management ====================
static void adminDeptAdd(DepartmentNode*& deptHead) {
    printTitle("添加科室");

    string name = readString("科室名称 (≤20字符, 0=取消): ", MAX_NAME_LEN);
    if (name == "0") return;

    string desc = readOptionalString("科室描述 (≤100字符, 可留空): ", MAX_DESC_LEN);

    string deptID = generateDeptID();
    cout << "科室ID: " << deptID << endl;

    DepartmentNode* newNode = new DepartmentNode();
    newNode->deptID = deptID;
    newNode->name = name;
    newNode->description = desc;
    insertDepartmentHead(deptHead, newNode);
    cout << "科室 " << name << " 添加成功!" << endl;
    pauseScreen();
}

static void adminDeptModify(DepartmentNode* deptHead) {
    printTitle("修改科室");
    string id = readString("请输入科室ID (0=取消): ", 10);
    if (id == "0") return;

    DepartmentNode* dept = findDepartmentByID(deptHead, id);
    if (dept == nullptr) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "\n当前信息:" << endl;
    cout << "  名称: " << dept->name << endl;
    cout << "  描述: " << dept->description << endl;
    cout << "\n输入新信息 (直接回车保留原值):" << endl;

    string name = readOptionalString("科室名称 (≤20字符): ", MAX_NAME_LEN);
    if (!name.empty()) dept->name = name;

    string desc = readOptionalString("科室描述 (≤100字符): ", MAX_DESC_LEN);
    if (!desc.empty()) dept->description = desc;

    cout << "科室信息已更新!" << endl;
    pauseScreen();
}

static void adminDeptDelete(DepartmentNode*& deptHead,
                            DoctorNode* doctorHead,
                            WardNode* wardHead) {
    printTitle("删除科室");
    string id = readString("请输入科室ID (0=取消): ", 10);
    if (id == "0") return;

    DepartmentNode* dept = findDepartmentByID(deptHead, id);
    if (dept == nullptr) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    // Check if any doctors belong to this department
    int docCount = 0;
    for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
        if (d->departmentID == id) docCount++;
    }
    if (docCount > 0) {
        cout << "科室 " << dept->name << " 下还有 " << docCount << " 名医生，无法删除!" << endl;
        pauseScreen();
        return;
    }

    // Check if any wards belong to this department
    int wrdCount = 0;
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        if (w->departmentID == id) wrdCount++;
    }
    if (wrdCount > 0) {
        cout << "科室 " << dept->name << " 下还有 " << wrdCount << " 个病房，无法删除!" << endl;
        pauseScreen();
        return;
    }

    if (!readConfirm(("确认删除科室 " + dept->name + "? (y/n): ").c_str())) {
        cout << "已取消删除。" << endl;
        pauseScreen();
        return;
    }

    if (deleteDepartmentByID(deptHead, id)) {
        cout << "科室已删除!" << endl;
    } else {
        cout << "删除失败!" << endl;
    }
    pauseScreen();
}

static void adminDeptList(DepartmentNode* deptHead) {
    printTitle("科室列表");
    if (deptHead == nullptr) {
        cout << "暂无科室记录。" << endl;
        pauseScreen();
        return;
    }
    printf("%-8s %-22s %-30s\n", "ID", "名称", "描述");
    printLine(65);
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        printf("%-8s %-22s %-30s\n", d->deptID.c_str(), d->name.c_str(), d->description.c_str());
    }
    printLine(65);
    cout << "共 " << countDepartments(deptHead) << " 个科室。" << endl;
    pauseScreen();
}

static void adminDeptAssignWard(DepartmentNode* deptHead, WardNode*& wardHead) {
    printTitle("分配病房到科室");
    if (wardHead == nullptr) {
        cout << "暂无病房记录。" << endl;
        pauseScreen();
        return;
    }

    // List all wards
    cout << "现有病房:" << endl;
    printf("%-8s %-8s %-10s %s\n", "ID", "类型", "床位数", "所属科室");
    printLine(50);
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        DepartmentNode* d = findDepartmentByID(deptHead, w->departmentID);
        printf("%-8s %-8s %-10d %s\n", w->wardID.c_str(), getWardTypeName(w->type),
               w->bedCount, d ? d->name.c_str() : "无");
    }
    printLine(50);

    string wardID = readString("\n请输入病房ID (0=取消): ", 10);
    if (wardID == "0") return;

    WardNode* ward = findWardByID(wardHead, wardID);
    if (ward == nullptr) {
        cout << "病房不存在!" << endl;
        pauseScreen();
        return;
    }

    // List departments
    cout << "\n现有科室:" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string deptID = readString("\n请输入目标科室ID (0=取消): ", 10);
    if (deptID == "0") return;

    if (!deptExists(deptHead, deptID)) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    ward->departmentID = deptID;
    cout << "病房 " << wardID << " 已分配到科室 " << deptID << "!" << endl;
    pauseScreen();
}

static void adminDeptAssignMedicine(DepartmentNode* deptHead,
                                    MedicineNode* medHead,
                                    DeptMedicineNode*& deptMedHead) {
    printTitle("分配药品到科室");
    if (medHead == nullptr) {
        cout << "暂无药品记录。" << endl;
        pauseScreen();
        return;
    }

    // List medicines
    cout << "现有药品:" << endl;
    printf("%-8s %-20s\n", "ID", "商品名");
    printLine(35);
    for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
        printf("%-8s %-20s\n", m->medID.c_str(), m->tradeName.c_str());
    }
    printLine(35);

    string medID = readString("\n请输入药品ID (0=取消): ", 10);
    if (medID == "0") return;

    if (findMedicineByID(medHead, medID) == nullptr) {
        cout << "药品不存在!" << endl;
        pauseScreen();
        return;
    }

    // List departments
    cout << "\n现有科室:" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string deptID = readString("请输入科室ID (0=取消): ", 10);
    if (deptID == "0") return;

    if (!deptExists(deptHead, deptID)) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    if (isMedicineInDept(deptMedHead, deptID, medID)) {
        cout << "该药品已分配到该科室，不可重复分配!" << endl;
        pauseScreen();
        return;
    }

    DeptMedicineNode* dm = new DeptMedicineNode();
    dm->deptID = deptID;
    dm->medID = medID;
    insertDeptMedicineHead(deptMedHead, dm);
    cout << "药品 " << medID << " 已分配到科室 " << deptID << "!" << endl;
    pauseScreen();
}

static void adminDeptMenu(DepartmentNode*& deptHead,
                          DoctorNode* doctorHead,
                          WardNode*& wardHead,
                          MedicineNode* medHead,
                          DeptMedicineNode*& deptMedHead) {
    int choice;
    do {
        printTitle("科室管理");
        cout << "  1. 添加科室" << endl;
        cout << "  2. 修改科室" << endl;
        cout << "  3. 删除科室" << endl;
        cout << "  4. 列出所有科室" << endl;
        cout << "  5. 分配病房到科室" << endl;
        cout << "  6. 分配药品到科室" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 6);

        switch (choice) {
            case 1: adminDeptAdd(deptHead); break;
            case 2: adminDeptModify(deptHead); break;
            case 3: adminDeptDelete(deptHead, doctorHead, wardHead); break;
            case 4: adminDeptList(deptHead); break;
            case 5: adminDeptAssignWard(deptHead, wardHead); break;
            case 6: adminDeptAssignMedicine(deptHead, medHead, deptMedHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Ward Management ====================
static void adminWardAdd(DepartmentNode* deptHead, WardNode*& wardHead) {
    printTitle("添加病房");
    string wardID = generateWardID();
    cout << "病房ID: " << wardID << endl;

    cout << "病房类型: 0-普通病房  1-特需病房" << endl;
    int type = readIntRange("请选择病房类型 (0=普通/1=特需, -1=取消): ", -1, 1);
    if (type < 0) return;

    // List departments for selection
    if (deptHead == nullptr) {
        cout << "暂无科室，请先添加科室!" << endl;
        pauseScreen();
        return;
    }
    cout << "\n现有科室:" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string deptID = readString("请输入所属科室ID (0=取消): ", 10);
    if (deptID == "0") return;
    if (!deptExists(deptHead, deptID)) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    int bedCount = readIntRange("床位数 (1-30, 0=取消): ", 0, MAX_BEDS);
    if (bedCount == 0) return;

    WardNode* newNode = new WardNode();
    newNode->wardID = wardID;
    newNode->type = type;
    newNode->departmentID = deptID;
    newNode->bedCount = bedCount;
    insertWardHead(wardHead, newNode);
    cout << "病房 " << wardID << " 添加成功!" << endl;
    pauseScreen();
}

static void adminWardModify(DepartmentNode* deptHead, WardNode* wardHead) {
    printTitle("修改病房");
    string id = readString("请输入病房ID (0=取消): ", 10);
    if (id == "0") return;

    WardNode* ward = findWardByID(wardHead, id);
    if (ward == nullptr) {
        cout << "病房不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "\n当前信息:" << endl;
    cout << "  类型: " << getWardTypeName(ward->type) << endl;
    DepartmentNode* curDept = findDepartmentByID(deptHead, ward->departmentID);
    cout << "  所属科室: " << (curDept ? curDept->name : "未知") << " (" << ward->departmentID << ")" << endl;
    cout << "  床位数: " << ward->bedCount << endl;

    cout << "\n输入新信息 (输入-1保留原值):" << endl;

    int type = readIntRange("病房类型 (0=普通/1=特需, -1=不修改): ", -1, 1);
    if (type >= 0) ward->type = type;

    string deptID = readOptionalString("所属科室ID (直接回车不修改): ", 10);
    if (!deptID.empty()) {
        if (!deptExists(deptHead, deptID)) {
            cout << "科室不存在，放弃修改科室归属!" << endl;
        } else {
            ward->departmentID = deptID;
        }
    }

    int bedCount = readIntRange("床位数 (1-30, -1=不修改, 0=取消): ", -1, MAX_BEDS);
    if (bedCount == 0) return;
    if (bedCount > 0) ward->bedCount = bedCount;

    cout << "病房信息已更新!" << endl;
    pauseScreen();
}

static void adminWardDelete(WardNode*& wardHead) {
    printTitle("删除病房");
    string id = readString("请输入病房ID (0=取消): ", 10);
    if (id == "0") return;

    WardNode* ward = findWardByID(wardHead, id);
    if (ward == nullptr) {
        cout << "病房不存在!" << endl;
        pauseScreen();
        return;
    }

    // Check if any beds are occupied
    int occupied = 0;
    for (int i = 0; i < ward->bedCount; i++) {
        if (!ward->bedStatus[i].empty()) occupied++;
    }
    if (occupied > 0) {
        cout << "病房 " << ward->wardID << " 中还有 " << occupied << " 张床位被占用，无法删除!" << endl;
        pauseScreen();
        return;
    }

    if (!readConfirm(("确认删除病房 " + ward->wardID + "? (y/n): ").c_str())) {
        cout << "已取消删除。" << endl;
        pauseScreen();
        return;
    }

    if (deleteWardByID(wardHead, id)) {
        cout << "病房已删除!" << endl;
    } else {
        cout << "删除失败!" << endl;
    }
    pauseScreen();
}

static void adminWardList(DepartmentNode* deptHead, WardNode* wardHead) {
    printTitle("病房列表");
    if (wardHead == nullptr) {
        cout << "暂无病房记录。" << endl;
        pauseScreen();
        return;
    }

    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        DepartmentNode* d = findDepartmentByID(deptHead, w->departmentID);
        cout << "\n病房 " << w->wardID << " (" << getWardTypeName(w->type) << ")" << endl;
        cout << "  所属科室: " << (d ? d->name : "未知") << " (" << w->departmentID << ")" << endl;
        cout << "  床位数: " << w->bedCount << "  空闲: " << getFreeBedCount(w) << endl;
        // Show bed status
        cout << "  床位状态: ";
        for (int i = 0; i < w->bedCount; i++) {
            if (i > 0 && i % 10 == 0) cout << "\n           ";
            cout << (i + 1) << "号:" << (w->bedStatus[i].empty() ? "空闲" : "占用") << " ";
        }
        cout << endl;
    }
    printLine();
    pauseScreen();
}

static void adminWardChangeDept(DepartmentNode* deptHead, WardNode* wardHead) {
    printTitle("更改病房所属科室");
    string wardID = readString("请输入病房ID (0=取消): ", 10);
    if (wardID == "0") return;

    WardNode* ward = findWardByID(wardHead, wardID);
    if (ward == nullptr) {
        cout << "病房不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "当前所属科室: ";
    DepartmentNode* curDept = findDepartmentByID(deptHead, ward->departmentID);
    cout << (curDept ? curDept->name : "未知") << " (" << ward->departmentID << ")" << endl;

    cout << "\n可选科室:" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string newDeptID = readString("\n请输入新科室ID (0=取消): ", 10);
    if (newDeptID == "0") return;
    if (!deptExists(deptHead, newDeptID)) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    ward->departmentID = newDeptID;
    cout << "病房 " << wardID << " 已更改为科室 " << newDeptID << "!" << endl;
    pauseScreen();
}

static void adminWardMenu(DepartmentNode* deptHead, WardNode*& wardHead) {
    int choice;
    do {
        printTitle("病房管理");
        cout << "  1. 添加病房" << endl;
        cout << "  2. 修改病房" << endl;
        cout << "  3. 删除病房" << endl;
        cout << "  4. 列出所有病房" << endl;
        cout << "  5. 更改病房所属科室" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 5);

        switch (choice) {
            case 1: adminWardAdd(deptHead, wardHead); break;
            case 2: adminWardModify(deptHead, wardHead); break;
            case 3: adminWardDelete(wardHead); break;
            case 4: adminWardList(deptHead, wardHead); break;
            case 5: adminWardChangeDept(deptHead, wardHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Doctor Management ====================
static void adminDoctorAdd(DepartmentNode* deptHead, DoctorNode*& doctorHead) {
    printTitle("添加医生");

    string name = readString("医生姓名 (≤20字符, 0=取消): ", MAX_NAME_LEN);
    if (name == "0") return;

    cout << "\n医生级别:" << endl;
    for (int i = 0; i < MAX_DOCTOR_LEVELS; i++) {
        cout << "  " << i << " - " << LEVEL_NAMES[i] << endl;
    }
    int level = readIntRange("请选择级别 (0-3, -1=取消): ", -1, 3);
    if (level < 0) return;

    // List departments
    if (deptHead == nullptr) {
        cout << "暂无科室，请先添加科室!" << endl;
        pauseScreen();
        return;
    }
    cout << "\n现有科室:" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string deptID = readString("请输入所属科室ID (0=取消): ", 10);
    if (deptID == "0") return;
    if (!deptExists(deptHead, deptID)) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    // Input work days
    cout << "\n出诊日设置 (1=周一 ~ 7=周日, 逗号分隔, 如: 1,3,5):" << endl;
    string workDaysStr = readString("出诊日: ", 50);
    if (workDaysStr == "0") return;

    string doctorID = generateDoctorID();
    cout << "医生ID: " << doctorID << endl;

    DoctorNode* newNode = new DoctorNode();
    newNode->doctorID = doctorID;
    newNode->name = name;
    newNode->level = level;
    newNode->departmentID = deptID;

    // Parse work days
    int dayCount = 0;
    stringstream ss(workDaysStr);
    string token;
    while (getline(ss, token, ',')) {
        // Trim
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start == string::npos) continue;
        token = token.substr(start, end - start + 1);

        int day = atoi(token.c_str());
        if (day >= 1 && day <= MAX_WORKDAYS && dayCount < MAX_WORKDAYS) {
            // Check for duplicates
            bool dup = false;
            for (int i = 0; i < dayCount; i++) {
                if (newNode->workDays[i] == day) { dup = true; break; }
            }
            if (!dup) {
                newNode->workDays[dayCount++] = day;
            }
        }
    }
    newNode->workDayCount = dayCount;
    if (dayCount == 0) {
        cout << "警告: 未设置有效出诊日!" << endl;
    }

    insertDoctorHead(doctorHead, newNode);
    cout << "医生 " << name << " 添加成功! 出诊日: ";
    for (int i = 0; i < newNode->workDayCount; i++) {
        if (i > 0) cout << ", ";
        cout << WEEKDAY_NAMES[newNode->workDays[i] - 1];
    }
    cout << endl;
    pauseScreen();
}

static void adminDoctorModify(DepartmentNode* deptHead, DoctorNode* doctorHead) {
    printTitle("修改医生");
    string id = readString("请输入医生ID (0=取消): ", 10);
    if (id == "0") return;

    DoctorNode* doc = findDoctorByID(doctorHead, id);
    if (doc == nullptr) {
        cout << "医生不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "\n当前信息:" << endl;
    cout << "  姓名: " << doc->name << endl;
    cout << "  级别: " << getLevelName(doc->level) << endl;
    DepartmentNode* curDept = findDepartmentByID(deptHead, doc->departmentID);
    cout << "  科室: " << (curDept ? curDept->name : "未知") << " (" << doc->departmentID << ")" << endl;
    cout << "  出诊日: ";
    for (int i = 0; i < doc->workDayCount; i++) {
        if (i > 0) cout << ", ";
        cout << WEEKDAY_NAMES[doc->workDays[i] - 1];
    }
    cout << endl;

    cout << "\n输入新信息 (直接回车保留原值, 0=取消):" << endl;

    string name = readOptionalString("姓名 (≤20字符): ", MAX_NAME_LEN);
    if (!name.empty()) doc->name = name;

    cout << "级别 (0=主任/1=副主任/2=主治/3=住院, -1=不修改, -2=取消): ";
    int level = readIntRange("", -2, MAX_DOCTOR_LEVELS - 1);
    if (level == -2) return;
    if (level >= 0 && level < MAX_DOCTOR_LEVELS) doc->level = level;

    string deptID = readOptionalString("科室ID (直接回车不修改): ", 10);
    if (!deptID.empty()) {
        if (!deptExists(deptHead, deptID)) {
            cout << "科室不存在，放弃修改!" << endl;
        } else {
            doc->departmentID = deptID;
        }
    }

    string workDaysStr = readOptionalString("出诊日 (逗号分隔, 直接回车不修改): ", 50);
    if (!workDaysStr.empty()) {
        int dayCount = 0;
        int tempDays[MAX_WORKDAYS] = {0};
        stringstream ss(workDaysStr);
        string token;
        while (getline(ss, token, ',')) {
            size_t start = token.find_first_not_of(" \t");
            size_t end = token.find_last_not_of(" \t");
            if (start == string::npos) continue;
            token = token.substr(start, end - start + 1);
            int day = atoi(token.c_str());
            if (day >= 1 && day <= MAX_WORKDAYS && dayCount < MAX_WORKDAYS) {
                bool dup = false;
                for (int i = 0; i < dayCount; i++) {
                    if (tempDays[i] == day) { dup = true; break; }
                }
                if (!dup) tempDays[dayCount++] = day;
            }
        }
        doc->workDayCount = dayCount;
        for (int i = 0; i < dayCount; i++) doc->workDays[i] = tempDays[i];
    }

    cout << "医生信息已更新!" << endl;
    pauseScreen();
}

static void adminDoctorDelete(DoctorNode*& doctorHead, RegistrationNode* regHead) {
    printTitle("删除医生");
    string id = readString("请输入医生ID (0=取消): ", 10);
    if (id == "0") return;

    DoctorNode* doc = findDoctorByID(doctorHead, id);
    if (doc == nullptr) {
        cout << "医生不存在!" << endl;
        pauseScreen();
        return;
    }

    // Check for linked records that would become orphaned
    int pendingRegs = 0, totalRegs = 0;
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->doctorID == id) {
            totalRegs++;
            if (r->status == STATUS_PENDING) pendingRegs++;
        }
    }
    if (totalRegs > 0) {
        cout << "警告: 医生 " << doc->name << " 有 " << totalRegs << " 条关联挂号记录"
             << "(其中 " << pendingRegs << " 条待处理)!" << endl;
        cout << "删除后这些记录的医生信息将无法解析。" << endl;
    }

    if (!readConfirm(("确认删除医生 " + doc->name + "? (y/n): ").c_str())) {
        cout << "已取消删除。" << endl;
        pauseScreen();
        return;
    }

    if (deleteDoctorByID(doctorHead, id)) {
        cout << "医生已删除!" << endl;
    } else {
        cout << "删除失败!" << endl;
    }
    pauseScreen();
}

static void adminDoctorListByDept(DepartmentNode* deptHead, DoctorNode* doctorHead) {
    printTitle("按科室列出医生");

    // Show departments for filtering
    cout << "科室列表:" << endl;
    cout << "  0 - 全部" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string deptID = readString("\n请输入科室ID (0=全部): ", 10);

    cout << "\n";
    printf("%-8s %-16s %-10s %-10s %s\n", "ID", "姓名", "级别", "科室", "出诊日");
    printLine(75);

    int count = 0;
    for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
        if (deptID != "0" && d->departmentID != deptID) continue;

        DepartmentNode* dept = findDepartmentByID(deptHead, d->departmentID);
        string deptName = dept ? dept->name : "未知";

        printf("%-8s %-16s %-10s %-10s ",
               d->doctorID.c_str(), d->name.c_str(),
               getLevelName(d->level), deptName.c_str());

        for (int i = 0; i < d->workDayCount; i++) {
            if (i > 0) cout << ",";
            cout << WEEKDAY_NAMES[d->workDays[i] - 1];
        }
        cout << endl;
        count++;
    }
    printLine(75);
    cout << "共 " << count << " 名医生。" << endl;
    pauseScreen();
}

static void adminDoctorMenu(DepartmentNode* deptHead,
                            DoctorNode*& doctorHead,
                            RegistrationNode* regHead) {
    int choice;
    do {
        printTitle("医生管理");
        cout << "  1. 添加医生" << endl;
        cout << "  2. 修改医生" << endl;
        cout << "  3. 删除医生" << endl;
        cout << "  4. 按科室列出医生" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 4);

        switch (choice) {
            case 1: adminDoctorAdd(deptHead, doctorHead); break;
            case 2: adminDoctorModify(deptHead, doctorHead); break;
            case 3: adminDoctorDelete(doctorHead, regHead); break;
            case 4: adminDoctorListByDept(deptHead, doctorHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Medicine Management ====================
static void adminMedAdd(MedicineNode*& medHead) {
    printTitle("添加药品");

    string tradeName = readString("商品名 (≤20字符, 0=取消): ", MAX_NAME_LEN);
    if (tradeName == "0") return;

    string genericName = readString("通用名 (≤20字符): ", MAX_NAME_LEN);
    string alias = readOptionalString("别名 (可留空, ≤20字符): ", MAX_NAME_LEN);
    string spec = readString("规格 (≤50字符): ", MAX_ITEM_LEN);
    int stock = readIntRange("初始库存 (≥0, -1=取消): ", -1, 99999);
    if (stock < 0) return;

    string medID = generateMedID();
    cout << "药品ID: " << medID << endl;

    MedicineNode* newNode = new MedicineNode();
    newNode->medID = medID;
    newNode->tradeName = tradeName;
    newNode->genericName = genericName;
    newNode->alias = alias;
    newNode->spec = spec;
    newNode->stock = stock;
    newNode->consumed = 0;
    insertMedicineHead(medHead, newNode);
    cout << "药品 " << tradeName << " 添加成功!" << endl;
    pauseScreen();
}

static void adminMedModify(MedicineNode* medHead) {
    printTitle("修改药品");
    string id = readString("请输入药品ID (0=取消): ", 10);
    if (id == "0") return;

    MedicineNode* med = findMedicineByID(medHead, id);
    if (med == nullptr) {
        cout << "药品不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "\n当前信息:" << endl;
    cout << "  商品名: " << med->tradeName << endl;
    cout << "  通用名: " << med->genericName << endl;
    cout << "  别名: " << med->alias << endl;
    cout << "  规格: " << med->spec << endl;
    cout << "  库存: " << med->stock << endl;

    cout << "\n输入新信息 (直接回车保留原值):" << endl;

    string tradeName = readOptionalString("商品名 (≤20字符): ", MAX_NAME_LEN);
    if (!tradeName.empty()) med->tradeName = tradeName;

    string genericName = readOptionalString("通用名 (≤20字符): ", MAX_NAME_LEN);
    if (!genericName.empty()) med->genericName = genericName;

    string alias = readOptionalString("别名 (≤20字符): ", MAX_NAME_LEN);
    if (!alias.empty()) med->alias = alias;

    string spec = readOptionalString("规格 (≤50字符): ", MAX_ITEM_LEN);
    if (!spec.empty()) med->spec = spec;

    cout << "库存 (-1=不修改): ";
    int stock = readInt("");
    if (stock >= 0) med->stock = stock;

    cout << "药品信息已更新!" << endl;
    pauseScreen();
}

static void adminMedDelete(MedicineNode*& medHead) {
    printTitle("删除药品");
    string id = readString("请输入药品ID (0=取消): ", 10);
    if (id == "0") return;

    MedicineNode* med = findMedicineByID(medHead, id);
    if (med == nullptr) {
        cout << "药品不存在!" << endl;
        pauseScreen();
        return;
    }

    if (!readConfirm(("确认删除药品 " + med->tradeName + "? (y/n): ").c_str())) {
        cout << "已取消删除。" << endl;
        pauseScreen();
        return;
    }

    if (deleteMedicineByID(medHead, id)) {
        cout << "药品已删除!" << endl;
    } else {
        cout << "删除失败!" << endl;
    }
    pauseScreen();
}

static void adminMedList(MedicineNode* medHead) {
    printTitle("药品列表");
    if (medHead == nullptr) {
        cout << "暂无药品记录。" << endl;
        pauseScreen();
        return;
    }

    printf("%-8s %-20s %-20s %-10s %s\n", "ID", "商品名", "通用名", "规格", "库存");
    printLine(85);
    for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
        printf("%-8s %-20s %-20s %-10s %d\n",
               m->medID.c_str(), m->tradeName.c_str(),
               m->genericName.c_str(), m->spec.c_str(), m->stock);
    }
    printLine(85);
    cout << "共 " << countMedicines(medHead) << " 种药品。" << endl;
    pauseScreen();
}

static void adminMedMenu(MedicineNode*& medHead) {
    int choice;
    do {
        printTitle("药品管理");
        cout << "  1. 添加药品" << endl;
        cout << "  2. 修改药品" << endl;
        cout << "  3. 删除药品" << endl;
        cout << "  4. 列出所有药品" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 4);

        switch (choice) {
            case 1: adminMedAdd(medHead); break;
            case 2: adminMedModify(medHead); break;
            case 3: adminMedDelete(medHead); break;
            case 4: adminMedList(medHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Patient Management ====================
static void adminPatientAdd(PatientNode*& patientHead) {
    printTitle("添加患者");

    string name = readString("患者姓名 (≤20字符, 0=取消): ", MAX_NAME_LEN);
    if (name == "0") return;

    int age = readIntRange("年龄 (0-150, -1=取消): ", -1, 150);
    if (age < 0) return;

    string contact = readString("联系方式 (≤20字符): ", MAX_CONTACT_LEN);

    cout << "患者类型: 0-门诊患者  1-住院患者" << endl;
    int type = readIntRange("请选择 (0/1, -1=取消): ", -1, 1);
    if (type < 0) return;

    string patientID = generatePatientID();
    cout << "患者ID: " << patientID << endl;

    PatientNode* newNode = new PatientNode();
    newNode->patientID = patientID;
    newNode->name = name;
    newNode->age = age;
    newNode->contact = contact;
    newNode->type = type;
    newNode->deposit = 0.0;
    newNode->admitDays = 0;
    newNode->bedID = "";
    insertPatientHead(patientHead, newNode);
    cout << "患者 " << name << " 添加成功!" << endl;
    pauseScreen();
}

static void adminPatientModify(PatientNode* patientHead) {
    printTitle("修改患者");
    string id = readString("请输入患者ID (0=取消): ", 10);
    if (id == "0") return;

    PatientNode* p = findPatientByID(patientHead, id);
    if (p == nullptr) {
        cout << "患者不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "\n当前信息:" << endl;
    cout << "  姓名: " << p->name << endl;
    cout << "  年龄: " << p->age << endl;
    cout << "  联系方式: " << p->contact << endl;
    cout << "  类型: " << (p->type == PATIENT_OUTPATIENT ? "门诊患者" : "住院患者") << endl;
    if (p->type == PATIENT_INPATIENT) {
        cout << "  押金余额: " << p->deposit << endl;
    }

    cout << "\n输入新信息 (直接回车保留原值, 0=取消):" << endl;

    string name = readOptionalString("姓名 (≤20字符): ", MAX_NAME_LEN);
    if (!name.empty()) p->name = name;

    cout << "年龄 (0-150, -1=不修改, -2=取消): ";
    int age = readIntRange("", -2, 150);
    if (age == -2) return;
    if (age >= 0) p->age = age;

    string contact = readOptionalString("联系方式 (≤20字符): ", MAX_CONTACT_LEN);
    if (!contact.empty()) p->contact = contact;

    // For inpatients, allow modifying deposit
    if (p->type == PATIENT_INPATIENT) {
        cout << "修改押金 (-1=不修改): ";
        double deposit = readDouble("");
        if (deposit >= 0) p->deposit = round2(deposit);
    }

    cout << "患者信息已更新!" << endl;
    pauseScreen();
}

static void adminPatientDelete(PatientNode*& patientHead, HospitalizationNode* hospHead) {
    printTitle("删除患者(逻辑删除)");
    string id = readString("请输入患者ID (0=取消): ", 10);
    if (id == "0") return;

    PatientNode* p = findPatientByID(patientHead, id);
    if (p == nullptr) {
        cout << "患者不存在!" << endl;
        pauseScreen();
        return;
    }

    // Check for active hospitalization
    HospitalizationNode* activeHosp = findActiveHospitalizationByPatient(hospHead, id);
    if (activeHosp != nullptr) {
        cout << "警告: 患者 " << p->name << " 目前正在住院 (住院ID: " << activeHosp->hospID << ")，无法删除!" << endl;
        pauseScreen();
        return;
    }

    if (!readConfirm(("确认逻辑删除患者 " + p->name + "? (y/n): ").c_str())) {
        cout << "已取消。" << endl;
        pauseScreen();
        return;
    }

    // Logical delete: just remove from list (or we can keep as logical - here we remove)
    deletePatientByID(patientHead, id);
    cout << "患者已逻辑删除!" << endl;
    pauseScreen();
}

static void adminPatientList(PatientNode* patientHead) {
    printTitle("患者列表");
    if (patientHead == nullptr) {
        cout << "暂无患者记录。" << endl;
        pauseScreen();
        return;
    }

    cout << "筛选条件: 0-全部  1-门诊  2-住院" << endl;
    int filter = readIntRange("请选择 (0-2): ", 0, 2);

    cout << "\n";
    printf("%-8s %-16s %-6s %-16s %-6s %s\n", "ID", "姓名", "年龄", "联系方式", "类型", "押金");
    printLine(85);

    int count = 0;
    for (PatientNode* p = patientHead; p != nullptr; p = p->next) {
        if (filter == 1 && p->type != PATIENT_OUTPATIENT) continue;
        if (filter == 2 && p->type != PATIENT_INPATIENT) continue;

        const char* typeStr = (p->type == PATIENT_OUTPATIENT) ? "门诊" : "住院";
        printf("%-8s %-16s %-6d %-16s %-6s %8.2f\n",
               p->patientID.c_str(), p->name.c_str(), p->age,
               p->contact.c_str(), typeStr, p->deposit);
        count++;
    }
    printLine(85);
    cout << "共 " << count << " 名患者。" << endl;
    pauseScreen();
}

static void adminPatientMenu(PatientNode*& patientHead, HospitalizationNode* hospHead) {
    int choice;
    do {
        printTitle("患者管理");
        cout << "  1. 添加患者" << endl;
        cout << "  2. 修改患者" << endl;
        cout << "  3. 删除患者" << endl;
        cout << "  4. 列出患者(可筛选)" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 4);

        switch (choice) {
            case 1: adminPatientAdd(patientHead); break;
            case 2: adminPatientModify(patientHead); break;
            case 3: adminPatientDelete(patientHead, hospHead); break;
            case 4: adminPatientList(patientHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Registration Management ====================
static void adminRegCreate(DepartmentNode* deptHead,
                           DoctorNode* doctorHead,
                           PatientNode* patientHead,
                           RegistrationNode*& regHead) {
    printTitle("创建挂号");
    if (patientHead == nullptr) {
        cout << "暂无患者记录!" << endl;
        pauseScreen();
        return;
    }
    if (deptHead == nullptr) {
        cout << "暂无科室记录!" << endl;
        pauseScreen();
        return;
    }

    // Select patient
    cout << "患者列表:" << endl;
    printf("%-8s %-16s %s\n", "ID", "姓名", "类型");
    for (PatientNode* p = patientHead; p != nullptr; p = p->next) {
        printf("%-8s %-16s %s\n", p->patientID.c_str(), p->name.c_str(),
               p->type == PATIENT_OUTPATIENT ? "门诊" : "住院");
    }

    string patientID = readString("\n请输入患者ID (0=取消): ", 10);
    if (patientID == "0") return;

    PatientNode* patient = findPatientByID(patientHead, patientID);
    if (patient == nullptr) {
        cout << "患者不存在!" << endl;
        pauseScreen();
        return;
    }

    // Select department
    cout << "\n科室列表:" << endl;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        cout << "  " << d->deptID << " - " << d->name << endl;
    }

    string deptID = readString("\n请输入科室ID (0=取消): ", 10);
    if (deptID == "0") return;
    if (!deptExists(deptHead, deptID)) {
        cout << "科室不存在!" << endl;
        pauseScreen();
        return;
    }

    // Check no duplicate registration (same patient+dept+day)
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->patientID == patientID && r->departmentID == deptID
            && r->regDay == weekday && r->status == STATUS_PENDING) {
            cout << "该患者今天已在该科室挂号，不可重复挂号!" << endl;
            pauseScreen();
            return;
        }
    }

    // Select doctor, filtered by department AND working today
    int dayOfWeek = ((weekday - 1) % 7) + 1;

    cout << "\n今日(" << WEEKDAY_NAMES[dayOfWeek - 1] << ")该科室出诊医生:" << endl;
    printf("%-8s %-16s %-10s\n", "ID", "姓名", "级别");
    printLine(40);
    int docCount = 0;
    for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
        if (d->departmentID == deptID && doctorWorksOnDay(d, dayOfWeek)) {
            printf("%-8s %-16s %-10s\n", d->doctorID.c_str(),
                   d->name.c_str(), getLevelName(d->level));
            docCount++;
        }
    }

    if (docCount == 0) {
        cout << "该科室今日无出诊医生!" << endl;
        pauseScreen();
        return;
    }

    string doctorID = readString("\n请输入医生ID (0=取消): ", 10);
    if (doctorID == "0") return;

    DoctorNode* doctor = findDoctorByID(doctorHead, doctorID);
    if (doctor == nullptr) {
        cout << "医生不存在!" << endl;
        pauseScreen();
        return;
    }
    if (doctor->departmentID != deptID) {
        cout << "该医生不属于所选科室!" << endl;
        pauseScreen();
        return;
    }
    if (!doctorWorksOnDay(doctor, dayOfWeek)) {
        cout << "该医生今日不出诊!" << endl;
        pauseScreen();
        return;
    }

    // Create registration
    RegistrationNode* newNode = new RegistrationNode();
    newNode->regID = generateRegID(weekday);
    newNode->patientID = patientID;
    newNode->departmentID = deptID;
    newNode->doctorID = doctorID;
    newNode->regDay = weekday;
    newNode->status = STATUS_PENDING;
    insertRegistrationTail(regHead, newNode);

    cout << "挂号成功! 挂号ID: " << newNode->regID << endl;
    cout << "  患者: " << patient->name << endl;
    DepartmentNode* dept = findDepartmentByID(deptHead, deptID);
    cout << "  科室: " << (dept ? dept->name : deptID) << endl;
    cout << "  医生: " << doctor->name << endl;
    cout << "  日期: Day" << weekday << " (" << WEEKDAY_NAMES[dayOfWeek - 1] << ")" << endl;
    pauseScreen();
}

static void adminRegView(DepartmentNode* deptHead,
                         DoctorNode* doctorHead,
                         PatientNode* patientHead,
                         RegistrationNode* regHead) {
    printTitle("查看挂号记录");
    cout << "筛选方式:" << endl;
    cout << "  1. 按科室" << endl;
    cout << "  2. 按医生" << endl;
    cout << "  3. 按日期" << endl;
    cout << "  0. 取消" << endl;
    int filter = readIntRange("请选择: ", 0, 3);
    if (filter == 0) return;

    string filterID;
    unsigned int filterDay = 0;

    if (filter == 1) {
        cout << "\n科室列表:" << endl;
        for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
            cout << "  " << d->deptID << " - " << d->name << endl;
        }
        filterID = readString("请输入科室ID (0=取消): ", 10);
        if (filterID == "0") return;
    } else if (filter == 2) {
        cout << "\n医生列表:" << endl;
        for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
            cout << "  " << d->doctorID << " - " << d->name << endl;
        }
        filterID = readString("请输入医生ID (0=取消): ", 10);
        if (filterID == "0") return;
    } else if (filter == 3) {
        filterDay = (unsigned int)readIntRange("请输入Day编号 (0=取消): ", 0, 999999);
        if (filterDay == 0) return;
    }

    cout << "\n";
    printf("%-10s %-10s %-10s %-10s %-10s %-10s %s\n",
           "挂号ID", "患者ID", "科室ID", "医生ID", "Day", "状态", "患者姓名");
    printLine(85);

    int count = 0;
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        bool match = false;
        if (filter == 1 && r->departmentID == filterID) match = true;
        else if (filter == 2 && r->doctorID == filterID) match = true;
        else if (filter == 3 && r->regDay == filterDay) match = true;

        if (!match) continue;

        PatientNode* p = findPatientByID(patientHead, r->patientID);
        string patientName = p ? p->name : "未知";

        printf("%-10s %-10s %-10s %-10s %-10u %-10s %s\n",
               r->regID.c_str(), r->patientID.c_str(), r->departmentID.c_str(),
               r->doctorID.c_str(), r->regDay, getRegStatusStr(r->status),
               patientName.c_str());
        count++;
    }
    printLine(85);
    cout << "共 " << count << " 条记录。" << endl;
    pauseScreen();
}

static void adminRegMenu(DepartmentNode* deptHead,
                         DoctorNode* doctorHead,
                         PatientNode* patientHead,
                         RegistrationNode*& regHead) {
    int choice;
    do {
        printTitle("挂号管理");
        cout << "  1. 创建挂号" << endl;
        cout << "  2. 查看挂号记录" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 2);

        switch (choice) {
            case 1: adminRegCreate(deptHead, doctorHead, patientHead, regHead); break;
            case 2: adminRegView(deptHead, doctorHead, patientHead, regHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Hospitalization Management ====================
static void adminHospAdmit(DepartmentNode* deptHead,
                           DoctorNode* doctorHead,
                           PatientNode*& patientHead,
                           WardNode* wardHead,
                           HospitalizationNode*& hospHead,
                           RegistrationNode* regHead) {
    printTitle("患者入院");
    if (patientHead == nullptr) {
        cout << "暂无患者记录!" << endl;
        pauseScreen();
        return;
    }

    // Select outpatient patient
    cout << "门诊患者列表:" << endl;
    printf("%-8s %-16s %s\n", "ID", "姓名", "联系方式");
    printLine(40);

    int opCount = 0;
    for (PatientNode* p = patientHead; p != nullptr; p = p->next) {
        if (p->type == PATIENT_OUTPATIENT) {
            printf("%-8s %-16s %s\n", p->patientID.c_str(), p->name.c_str(), p->contact.c_str());
            opCount++;
        }
    }

    if (opCount == 0) {
        cout << "暂无门诊患者!" << endl;
        pauseScreen();
        return;
    }

    string patientID = readString("\n请输入患者ID (0=取消): ", 10);
    if (patientID == "0") return;

    PatientNode* patient = findPatientByID(patientHead, patientID);
    if (patient == nullptr) {
        cout << "患者不存在!" << endl;
        pauseScreen();
        return;
    }
    if (patient->type != PATIENT_OUTPATIENT) {
        cout << "该患者不是门诊患者，无法办理入院!" << endl;
        pauseScreen();
        return;
    }

    // Find the department from the patient's registration
    string deptID = "";
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->patientID == patientID && r->status == STATUS_PENDING) {
            deptID = r->departmentID;
            break;
        }
    }
    // If no pending reg, check any reg
    if (deptID.empty()) {
        for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
            if (r->patientID == patientID) {
                deptID = r->departmentID;
                break;
            }
        }
    }

    // List wards, filtered by department if available
    cout << "\n可选病房:";
    if (!deptID.empty()) {
        cout << " (筛选自科室 " << deptID << ")";
    }
    cout << endl;

    printf("%-8s %-8s %-10s %-8s %s\n", "ID", "类型", "床位数", "空闲", "科室");
    printLine(60);

    int wardCount = 0;
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        if (!deptID.empty() && w->departmentID != deptID) continue;

        DepartmentNode* d = findDepartmentByID(deptHead, w->departmentID);
        printf("%-8s %-8s %-10d %-8d %s\n",
               w->wardID.c_str(), getWardTypeName(w->type),
               w->bedCount, getFreeBedCount(w),
               d ? d->name.c_str() : "?");
        wardCount++;
    }

    if (wardCount == 0) {
        cout << "暂无可用病房!" << endl;
        pauseScreen();
        return;
    }

    string wardID = readString("\n请输入病房ID (0=取消): ", 10);
    if (wardID == "0") return;

    WardNode* ward = findWardByID(wardHead, wardID);
    if (ward == nullptr) {
        cout << "病房不存在!" << endl;
        pauseScreen();
        return;
    }

    // Show available beds
    int freeBeds = getFreeBedCount(ward);
    if (freeBeds == 0) {
        cout << "该病房无空闲床位!" << endl;
        pauseScreen();
        return;
    }

    cout << "空闲床位: ";
    for (int i = 0; i < ward->bedCount; i++) {
        if (ward->bedStatus[i].empty()) {
            cout << (i + 1) << "号 ";
        }
    }
    cout << endl;

    int bedNo = readIntRange("请选择床位号 (0=取消): ", 0, ward->bedCount);
    if (bedNo == 0) return;
    int bedIdx = bedNo - 1;
    if (bedIdx >= 0 && bedIdx < ward->bedCount && !ward->bedStatus[bedIdx].empty()) {
        cout << "该床位已被占用!" << endl;
        pauseScreen();
        return;
    }

    // Select responsible doctors
    cout << "\n选择主管医生 (最多10名, 输入0结束):" << endl;
    for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
        DepartmentNode* dept = findDepartmentByID(deptHead, d->departmentID);
        cout << "  " << d->doctorID << " - " << d->name
             << " (" << (dept ? dept->name : "?") << ")" << endl;
    }

    string docIDs[10];
    int docCount = 0;
    while (docCount < 10) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt), "医生%d ID (0=结束): ", docCount + 1);
        string docID = readString(prompt, 10);
        if (docID == "0") break;
        DoctorNode* doc = findDoctorByID(doctorHead, docID);
        if (doc == nullptr) {
            cout << "医生不存在!" << endl;
            continue;
        }
        // Check for duplicate
        bool dup = false;
        for (int i = 0; i < docCount; i++) {
            if (docIDs[i] == docID) { dup = true; break; }
        }
        if (dup) {
            cout << "已选择该医生!" << endl;
            continue;
        }
        docIDs[docCount++] = docID;
    }
    if (docCount == 0) {
        cout << "未选择主管医生，已取消入院。" << endl;
        pauseScreen();
        return;
    }

    // Initial deposit
    double deposit = readNonNegDouble("初始押金 (≥0): ");

    // Convert patient to inpatient
    patient->type = PATIENT_INPATIENT;
    patient->deposit = deposit;
    patient->admitDays = 0;

    // Use the user-selected bed (already validated as free above)
    ward->bedStatus[bedIdx] = patientID;
    patient->bedID = wardID + "-" + to_string(bedNo);

    // Create hospitalization record
    HospitalizationNode* hosp = new HospitalizationNode();
    hosp->hospID = generateHospID(weekday);
    hosp->patientID = patientID;
    hosp->wardID = wardID;
    hosp->bedNo = bedIdx;
    hosp->doctorCount = docCount;
    for (int i = 0; i < docCount; i++) hosp->doctorIDs[i] = docIDs[i];
    hosp->admitDay = weekday;
    hosp->dischargeDay = 0;
    hosp->deposit = deposit;
    hosp->status = 0;  // in-hospital
    insertHospitalizationTail(hospHead, hosp);

    cout << "\n入院成功!" << endl;
    cout << "  住院ID: " << hosp->hospID << endl;
    cout << "  患者: " << patient->name << endl;
    cout << "  病房: " << wardID << " " << bedNo << "号床" << endl;
    cout << "  押金: " << deposit << " 元" << endl;
    pauseScreen();
}

static void adminHospDischarge(DepartmentNode* deptHead,
                               PatientNode* patientHead,
                               WardNode* wardHead,
                               HospitalizationNode*& hospHead) {
    printTitle("患者出院");
    cout << "查找方式:" << endl;
    cout << "  1. 按住院ID" << endl;
    cout << "  2. 按患者ID" << endl;
    int method = readIntRange("请选择: ", 1, 2);

    string searchID = readString("请输入ID (0=取消): ", 10);
    if (searchID == "0") return;

    HospitalizationNode* hosp = nullptr;
    if (method == 1) {
        hosp = findHospitalizationByID(hospHead, searchID);
    } else {
        hosp = findActiveHospitalizationByPatient(hospHead, searchID);
    }

    if (hosp == nullptr || hosp->status != 0) {
        cout << "未找到该患者的活动住院记录!" << endl;
        pauseScreen();
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, hosp->patientID);
    WardNode* ward = findWardByID(wardHead, hosp->wardID);

    cout << "\n住院信息:" << endl;
    cout << "  住院ID: " << hosp->hospID << endl;
    cout << "  患者: " << (patient ? patient->name : "?") << " (" << hosp->patientID << ")" << endl;
    cout << "  病房: " << hosp->wardID << " " << (hosp->bedNo + 1) << "号床" << endl;
    cout << "  入院Day: " << hosp->admitDay << endl;
    cout << "  押金: " << hosp->deposit << " 元" << endl;

    // Calculate stay days and charges (guard against data corruption)
    int stayDays = (weekday > hosp->admitDay) ? (int)(weekday - hosp->admitDay) : 1;
    double totalCharge = HOSPITAL_DAILY_FEE * stayDays;
    double balance = hosp->deposit - totalCharge;

    cout << "\n费用结算:" << endl;
    cout << "  住院天数: " << stayDays << " 天" << endl;
    cout << "  每日费用: " << HOSPITAL_DAILY_FEE << " 元" << endl;
    cout << "  总费用: " << totalCharge << " 元" << endl;
    cout << "  押金: " << hosp->deposit << " 元" << endl;

    if (balance >= 0) {
        cout << "  应退: " << balance << " 元" << endl;
    } else {
        cout << "  应补: " << (-balance) << " 元" << endl;
    }

    if (!readConfirm("\n确认办理出院? (y/n): ")) {
        cout << "已取消出院。" << endl;
        pauseScreen();
        return;
    }

    // Release bed
    if (ward != nullptr) {
        releaseBed(ward, hosp->bedNo);
    } else {
        cout << "警告: 未找到原病房记录，床位未能释放!" << endl;
    }

    // Update patient
    if (patient) {
        patient->type = PATIENT_OUTPATIENT;
        patient->bedID = "";
    }

    // Update hospitalization record
    hosp->dischargeDay = weekday;
    hosp->status = 1;  // discharged

    // Update hospital funds
    extern double money;
    money += totalCharge;

    cout << "出院办理成功!" << endl;
    pauseScreen();
}

static void adminHospViewAll(DepartmentNode* deptHead,
                             PatientNode* patientHead,
                             WardNode* wardHead,
                             HospitalizationNode* hospHead) {
    printTitle("在院患者列表");
    if (hospHead == nullptr) {
        cout << "暂无住院记录。" << endl;
        pauseScreen();
        return;
    }

    printf("%-10s %-10s %-16s %-10s %-8s %-10s %s\n",
           "住院ID", "患者ID", "姓名", "病房", "床位", "入院Day", "押金");
    printLine(90);

    int count = 0;
    for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
        if (h->status != 0) continue;  // only show in-hospital

        PatientNode* p = findPatientByID(patientHead, h->patientID);
        string patientName = p ? p->name : "未知";

        printf("%-10s %-10s %-16s %-10s %-8d %-10u %8.2f\n",
               h->hospID.c_str(), h->patientID.c_str(), patientName.c_str(),
               h->wardID.c_str(), h->bedNo + 1, h->admitDay, p ? p->deposit : 0.0);
        count++;
    }
    printLine(90);
    cout << "共 " << count << " 名在院患者。" << endl;
    pauseScreen();
}

static void adminHospMenu(DepartmentNode* deptHead,
                          DoctorNode* doctorHead,
                          PatientNode*& patientHead,
                          WardNode* wardHead,
                          HospitalizationNode*& hospHead,
                          RegistrationNode* regHead) {
    int choice;
    do {
        printTitle("住院管理");
        cout << "  1. 办理入院" << endl;
        cout << "  2. 办理出院" << endl;
        cout << "  3. 查看在院患者" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 3);

        switch (choice) {
            case 1: adminHospAdmit(deptHead, doctorHead, patientHead, wardHead, hospHead, regHead); break;
            case 2: adminHospDischarge(deptHead, patientHead, wardHead, hospHead); break;
            case 3: adminHospViewAll(deptHead, patientHead, wardHead, hospHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Pharmacy Management ====================
static void adminPharmacyStockIn(MedicineNode* medHead) {
    printTitle("药品入库");
    string medID = readString("请输入药品ID (0=取消): ", 10);
    if (medID == "0") return;

    MedicineNode* med = findMedicineByID(medHead, medID);
    if (med == nullptr) {
        cout << "药品不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "药品: " << med->tradeName << "  当前库存: " << med->stock << endl;
    int qty = readIntRange("入库数量 (>0, 0=取消): ", 0, 99999);
    if (qty == 0) return;

    med->stock += qty;
    cout << "入库成功! 当前库存: " << med->stock << endl;
    pauseScreen();
}

static void adminPharmacyStockOut(MedicineNode* medHead) {
    printTitle("药品出库");
    string medID = readString("请输入药品ID (0=取消): ", 10);
    if (medID == "0") return;

    MedicineNode* med = findMedicineByID(medHead, medID);
    if (med == nullptr) {
        cout << "药品不存在!" << endl;
        pauseScreen();
        return;
    }

    cout << "药品: " << med->tradeName << "  当前库存: " << med->stock << endl;
    int qty = readIntRange("出库数量 (>0, 0=取消): ", 0, 99999);
    if (qty == 0) return;

    if (qty > med->stock) {
        cout << "库存不足! 当前库存仅 " << med->stock << ", 无法出库 " << qty << "." << endl;
        pauseScreen();
        return;
    }

    med->stock -= qty;
    med->consumed += qty;
    cout << "出库成功! 当前库存: " << med->stock << endl;
    pauseScreen();
}

static void adminPharmacyViewInventory(MedicineNode* medHead) {
    printTitle("药房库存");
    if (medHead == nullptr) {
        cout << "暂无药品记录。" << endl;
        pauseScreen();
        return;
    }

    printf("%-8s %-20s %-20s %-10s %-10s %s\n",
           "ID", "商品名", "通用名", "规格", "库存", "已消耗");
    printLine(90);

    for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
        printf("%-8s %-20s %-20s %-10s %-10d %d\n",
               m->medID.c_str(), m->tradeName.c_str(),
               m->genericName.c_str(), m->spec.c_str(),
               m->stock, m->consumed);
    }
    printLine(90);
    cout << "共 " << countMedicines(medHead) << " 种药品。" << endl;
    pauseScreen();
}

static void adminPharmacyMenu(MedicineNode*& medHead) {
    int choice;
    do {
        printTitle("药房管理");
        cout << "  1. 药品入库" << endl;
        cout << "  2. 药品出库" << endl;
        cout << "  3. 查看库存" << endl;
        cout << "  0. 返回" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 3);

        switch (choice) {
            case 1: adminPharmacyStockIn(medHead); break;
            case 2: adminPharmacyStockOut(medHead); break;
            case 3: adminPharmacyViewInventory(medHead); break;
            case 0: break;
        }
    } while (choice != 0);
}

// ==================== Time Advancement ====================
static void adminTimeAdvance(PatientNode* patientHead,
                             RegistrationNode* regHead,
                             HospitalizationNode* hospHead) {
    printTitle("时间推进");
    int days = readIntRange("请输入推进天数 (>0, 0=取消): ", 0, 9999);
    if (days == 0) return;

    double totalDeducted = 0.0;
    int expiredCount = 0;

    extern unsigned long long globalTime;
    extern double money;
    for (int d = 0; d < days; d++) {
        // 1. Advance time first
        setTime(globalTime + SECONDS_PER_DAY);

        // 2. Auto-deduct 200 from each in-hospital patient's deposit and add to money
        for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
            if (h->status != 0) continue;  // only active hospitalizations
            h->deposit = round2(h->deposit - HOSPITAL_DAILY_FEE);
            money = round2(money + HOSPITAL_DAILY_FEE);
            totalDeducted = round2(totalDeducted + HOSPITAL_DAILY_FEE);

            // Also sync patient record's deposit and admitDays
            PatientNode* p = findPatientByID(patientHead, h->patientID);
            if (p != nullptr) {
                p->deposit = h->deposit;
                p->admitDays++;
            }
        }

        // 3. Expire pending registrations from days before the current weekday
        for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
            if (r->status == STATUS_PENDING && r->regDay < weekday) {
                r->status = STATUS_EXPIRED;
                expiredCount++;
            }
        }
    }

    cout << "\n时间推进 " << days << " 天完成!" << endl;
    cout << "  当前日期: Day" << weekday << " (" << WEEKDAY_NAMES[((weekday - 1) % 7)] << ")" << endl;
    cout << "  当前时间: " << formatTime() << endl;
    cout << "  扣除住院费用总计: " << totalDeducted << " 元" << endl;
    cout << "  过期挂号数: " << expiredCount << endl;

    // Show patients with negative deposit
    int negativeCount = 0;
    for (PatientNode* p = patientHead; p != nullptr; p = p->next) {
        if (p->type == PATIENT_INPATIENT && p->deposit < 0) {
            if (negativeCount == 0) {
                cout << "\n以下患者押金不足:" << endl;
            }
            cout << "  " << p->patientID << " " << p->name << " 余额: " << p->deposit << " 元" << endl;
            negativeCount++;
        }
    }

    pauseScreen();
}

// ==================== adminMenu Main ====================
bool adminMenu(
    DepartmentNode*& deptHead,
    DoctorNode*& doctorHead,
    PatientNode*& patientHead,
    RegistrationNode*& regHead,
    ConsultationNode*& consultHead,
    ExaminationNode*& examHead,
    PrescriptionNode*& prescHead,
    MedicineNode*& medHead,
    PrescMedicineNode*& prescMedHead,
    DeptMedicineNode*& deptMedHead,
    WardNode*& wardHead,
    HospitalizationNode*& hospHead
) {
    int choice;
    do {
        printTitle("管理员主菜单");
        printStatus();
        cout << "\n";
        cout << "   1. 科室管理" << endl;
        cout << "   2. 病房管理" << endl;
        cout << "   3. 医生管理" << endl;
        cout << "   4. 药品管理" << endl;
        cout << "   5. 患者管理" << endl;
        cout << "   6. 挂号管理" << endl;
        cout << "   7. 住院管理" << endl;
        cout << "   8. 药房管理" << endl;
        cout << "   9. 报表查询" << endl;
        cout << "  10. 时间推进" << endl;
        cout << "  11. 保存数据" << endl;
        cout << "   0. 退出登录" << endl;
        printLine();
        choice = readIntRange("请选择: ", 0, 11);

        switch (choice) {
            case 1:
                adminDeptMenu(deptHead, doctorHead, wardHead, medHead, deptMedHead);
                break;
            case 2:
                adminWardMenu(deptHead, wardHead);
                break;
            case 3:
                adminDoctorMenu(deptHead, doctorHead, regHead);
                break;
            case 4:
                adminMedMenu(medHead);
                break;
            case 5:
                adminPatientMenu(patientHead, hospHead);
                break;
            case 6:
                adminRegMenu(deptHead, doctorHead, patientHead, regHead);
                break;
            case 7:
                adminHospMenu(deptHead, doctorHead, patientHead, wardHead, hospHead, regHead);
                break;
            case 8:
                adminPharmacyMenu(medHead);
                break;
            case 9:
                adminReportMenu(deptHead, doctorHead, patientHead, regHead, consultHead,
                               examHead, prescHead, prescMedHead, medHead, deptMedHead,
                               wardHead, hospHead);
                break;
            case 10:
                adminTimeAdvance(patientHead, regHead, hospHead);
                break;
            case 11:
                if (saveAllData(deptHead, doctorHead, patientHead, regHead, consultHead,
                                examHead, prescHead, medHead, prescMedHead, deptMedHead,
                                wardHead, hospHead, adminHead)) {
                    cout << "数据保存成功!" << endl;
                } else {
                    cout << "数据保存失败!" << endl;
                }
                pauseScreen();
                break;
            case 0:
                if (readConfirm("确认退出登录? (y/n): ")) {
                    return false;
                }
                choice = -1;  // stay in loop
                break;
        }
    } while (true);

    return false;
}
