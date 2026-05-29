#include "global.h"
#include "entities.h"
#include "list_ops.h"
#include "init_data.h"
#include "persistence.h"
#include "utils.h"
#include "menus_admin.h"
#include "menus_doctor.h"
#include "menus_patient.h"
#include "reports.h"
#include "batch_input.h"

// Linked list heads
DepartmentNode* deptHead = nullptr;
DoctorNode* doctorHead = nullptr;
PatientNode* patientHead = nullptr;
RegistrationNode* regHead = nullptr;
ConsultationNode* consultHead = nullptr;
ExaminationNode* examHead = nullptr;
PrescriptionNode* prescHead = nullptr;
MedicineNode* medHead = nullptr;
PrescMedicineNode* prescMedHead = nullptr;
DeptMedicineNode* deptMedHead = nullptr;
WardNode* wardHead = nullptr;
HospitalizationNode* hospHead = nullptr;
AdminNode* adminHead = nullptr;

// Cleanup all memory
void cleanupAll() {
    freeDepartmentList(deptHead);
    freeDoctorList(doctorHead);
    freePatientList(patientHead);
    freeRegistrationList(regHead);
    freeConsultationList(consultHead);
    freeExaminationList(examHead);
    freePrescriptionList(prescHead);
    freeMedicineList(medHead);
    freePrescMedicineList(prescMedHead);
    freeDeptMedicineList(deptMedHead);
    freeWardList(wardHead);
    freeHospitalizationList(hospHead);
    freeAdminList(adminHead);
}

// Main menu
void mainMenu() {
    while (true) {
        printTitle("小型医院医疗管理系统");
        printStatus();
        std::cout << std::endl;
        std::cout << "  1. 管理员登录" << std::endl;
        std::cout << "  2. 医生登录" << std::endl;
        std::cout << "  3. 患者登录" << std::endl;
        std::cout << "  4. 批量数据导入" << std::endl;
        std::cout << "  0. 退出系统" << std::endl;
        printLine(50);
        int choice = readIntRange("请选择: ", 0, 4);

        switch (choice) {
            case 1: {
                if (adminLogin(adminHead)) {
                    while (adminMenu(deptHead, doctorHead, patientHead,
                               regHead, consultHead, examHead,
                               prescHead, medHead, prescMedHead,
                               deptMedHead, wardHead, hospHead)) {
                        // Continue in admin menu
                    }
                }
                break;
            }
            case 2: {
                if (doctorMenu(doctorHead, patientHead, regHead, consultHead,
                          examHead, prescHead, prescMedHead, medHead,
                          deptMedHead, wardHead, hospHead)) {
                    // Doctor menu handles its own loop
                }
                break;
            }
            case 3: {
                patientMenu(patientHead, regHead, consultHead, examHead,
                       prescHead, prescMedHead, medHead, wardHead,
                       hospHead, doctorHead);
                break;
            }
            case 4: {
                int batchMode = readIntRange("导入方式: 1=从文件, 2=从键盘标准输入, 0=取消: ", 0, 2);
                if (batchMode == 1) {
                    std::string filename = readString("请输入文件名: ", 100);
                    batchImportFromFile(filename.c_str(), deptHead, doctorHead, patientHead,
                                       regHead, medHead, deptMedHead, wardHead, adminHead);
                    pauseScreen();
                } else if (batchMode == 2) {
                    clearInputBuffer();  // consume leftover newline
                    batchImportFromStdin(deptHead, doctorHead, patientHead,
                                        regHead, medHead, deptMedHead, wardHead, adminHead);
                    pauseScreen();
                }
                break;
            }
            case 0:
                std::cout << "[提示] 是否在退出前保存数据?" << std::endl;
                if (readConfirm("保存数据? (y/n): ")) {
                    saveAllData(deptHead, doctorHead, patientHead,
                               regHead, consultHead, examHead,
                               prescHead, medHead, prescMedHead,
                               deptMedHead, wardHead, hospHead, adminHead);
                }
                std::cout << "[提示] 系统退出，感谢使用。" << std::endl;
                return;
        }
    }
}

int main() {
    // Set console to use UTF-8 on Windows
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    std::cout << "[提示] 正在启动医疗管理系统..." << std::endl;

    // Try to load saved data first
    if (!loadAllData(deptHead, doctorHead, patientHead,
                     regHead, consultHead, examHead,
                     prescHead, medHead, prescMedHead,
                     deptMedHead, wardHead, hospHead, adminHead)) {
        // Load failed, initialize with defaults
        std::cout << "[提示] 使用默认初始数据初始化系统。" << std::endl;
        setTime(0);
        money = 10000.00;
        initDefaultData(deptHead, doctorHead, patientHead,
                       regHead, consultHead, examHead,
                       prescHead, medHead, prescMedHead,
                       deptMedHead, wardHead, hospHead, adminHead);
    }

    std::cout << "[提示] 系统就绪。" << std::endl;
    std::cout << "  当前科室数: " << countDepartments(deptHead) << std::endl;
    std::cout << "  当前医生数: " << countDoctors(doctorHead) << std::endl;
    std::cout << "  当前患者数: " << countPatients(patientHead) << std::endl;
    std::cout << "  当前药品数: " << countMedicines(medHead) << std::endl;
    std::cout << std::endl;

    // Run main menu
    mainMenu();

    // Cleanup
    cleanupAll();

    return 0;
}
