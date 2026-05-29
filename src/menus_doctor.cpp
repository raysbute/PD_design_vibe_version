#include "menus_doctor.h"
#include "list_ops.h"
#include "utils.h"
#include "reports.h"
// ==================== Helper Functions ====================

// Display waiting patients for a specific doctor
// Returns the count of waiting patients found
static int showWaitingPatients(DoctorNode* doctor, RegistrationNode* regHead, PatientNode* patientHead) {
    if (!doctor) return 0;

    printTitle("待诊患者列表");
    std::cout << "医生: " << doctor->name << " (" << doctor->doctorID << ")" << std::endl;
    std::cout << "当前工作日: " << WEEKDAY_NAMES[week - 1] << std::endl;
    printLine(80);
    std::cout << std::left
              << std::setw(14) << "挂号ID"
              << std::setw(14) << "患者ID"
              << std::setw(12) << "患者姓名"
              << std::setw(14) << "科室ID"
              << std::setw(8)  << "日期"
              << std::setw(10) << "状态"
              << std::endl;
    printLine(80);

    int count = 0;
    RegistrationNode* cur = regHead;
    while (cur) {
        if (cur->doctorID == doctor->doctorID && cur->status == STATUS_PENDING && cur->regDay == day) {
            PatientNode* patient = findPatientByID(patientHead, cur->patientID);
            std::string patientName = patient ? patient->name : "未知";
            std::cout << std::left
                      << std::setw(14) << cur->regID
                      << std::setw(14) << cur->patientID
                      << std::setw(12) << patientName
                      << std::setw(14) << cur->departmentID
                      << std::setw(8)  << ("Day" + std::to_string(cur->regDay))
                      << std::setw(10) << getRegStatusStr(cur->status)
                      << std::endl;
            count++;
        }
        cur = cur->next;
    }

    if (count == 0) {
        std::cout << "  (暂无待诊患者)" << std::endl;
    }
    printLine(80);
    std::cout << "共 " << count << " 位待诊患者" << std::endl;
    return count;
}

// Display medicines linked to a doctor's department
static int showDeptMedicines(DoctorNode* doctor, MedicineNode* medHead, DeptMedicineNode* deptMedHead) {
    if (!doctor) return 0;

    printTitle("科室药品列表");
    std::cout << "科室: " << doctor->departmentID << std::endl;
    printLine(80);
    std::cout << std::left
              << std::setw(5)  << "序号"
              << std::setw(12) << "药品ID"
              << std::setw(20) << "商品名"
              << std::setw(10) << "库存"
              << std::endl;
    printLine(80);

    int count = 0;
    // Collect department medicine IDs for quick lookup
    DeptMedicineNode* dm = deptMedHead;
    MedicineNode* med = medHead;
    while (med) {
        bool inDept = false;
        dm = deptMedHead;
        while (dm) {
            if (dm->deptID == doctor->departmentID && dm->medID == med->medID) {
                inDept = true;
                break;
            }
            dm = dm->next;
        }
        if (inDept) {
            std::cout << std::left
                      << std::setw(5)  << (count + 1)
                      << std::setw(12) << med->medID
                      << std::setw(20) << med->tradeName
                      << std::setw(10) << med->stock
                      << std::endl;
            count++;
        }
        med = med->next;
    }

    if (count == 0) {
        std::cout << "  (该科室暂无关联药品)" << std::endl;
    }
    printLine(80);
    std::cout << "共 " << count << " 种药品" << std::endl;
    return count;
}

// Display consultations for a specific patient
static void showPatientConsultations(const std::string& patientID, DoctorNode*,
    ConsultationNode* consultHead) {
    printTitle("就诊记录");
    printLine(80);
    std::cout << std::left
              << std::setw(14) << "就诊ID"
              << std::setw(14) << "挂号ID"
              << std::setw(14) << "医生ID"
              << std::setw(20) << "主诉"
              << std::setw(20) << "诊断"
              << std::endl;
    printLine(80);

    ConsultationNode* cur = consultHead;
    bool found = false;
    while (cur) {
        if (cur->patientID == patientID && cur->status == STATUS_VALID) {
            std::cout << std::left
                      << std::setw(14) << cur->consultID
                      << std::setw(14) << cur->regID
                      << std::setw(14) << cur->doctorID
                      << std::setw(20) << cur->complaint
                      << std::setw(20) << cur->diagnosis
                      << std::endl;
            found = true;
        }
        cur = cur->next;
    }
    if (!found) std::cout << "  (暂无就诊记录)" << std::endl;
    printLine(80);
}

// Display prescriptions for a specific patient
static void showPatientPrescriptions(const std::string& patientID,
    PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead, MedicineNode* medHead) {
    printTitle("处方记录");
    printLine(80);
    std::cout << std::left
              << std::setw(14) << "处方ID"
              << std::setw(10) << "总金额"
              << std::setw(10) << "状态"
              << "药品明细"
              << std::endl;
    printLine(80);

    PrescriptionNode* cur = prescHead;
    bool found = false;
    while (cur) {
        if (cur->patientID == patientID) {
            std::cout << std::left
                      << std::setw(14) << cur->prescID
                      << std::setw(10) << cur->totalAmount
                      << std::setw(10) << getPrescStatusStr(cur->status);
            // Show medicines in this prescription
            PrescMedicineNode* pm = prescMedHead;
            bool first = true;
            while (pm) {
                if (pm->prescID == cur->prescID) {
                    MedicineNode* med = findMedicineByID(medHead, pm->medID);
                    if (!first) std::cout << "              ";
                    std::cout << (med ? med->tradeName : pm->medID)
                              << " x" << pm->quantity
                              << " @" << pm->unitPrice;
                    first = false;
                }
                pm = pm->next;
            }
            std::cout << std::endl;
            found = true;
        }
        cur = cur->next;
    }
    if (!found) std::cout << "  (暂无处方记录)" << std::endl;
    printLine(80);
}

// Display examinations for a specific patient
static void showPatientExaminations(const std::string& patientID, ExaminationNode* examHead) {
    printTitle("检查记录");
    printLine(80);
    std::cout << std::left
              << std::setw(14) << "检查ID"
              << std::setw(24) << "检查项目"
              << std::setw(10) << "费用"
              << std::setw(10) << "状态"
              << std::endl;
    printLine(80);

    ExaminationNode* cur = examHead;
    bool found = false;
    while (cur) {
        if (cur->patientID == patientID) {
            std::cout << std::left
                      << std::setw(14) << cur->examID
                      << std::setw(24) << cur->itemName
                      << std::setw(10) << std::fixed << std::setprecision(2) << cur->cost
                      << std::setw(10) << getRecordStatusStr(cur->status)
                      << std::endl;
            found = true;
        }
        cur = cur->next;
    }
    if (!found) std::cout << "  (暂无检查记录)" << std::endl;
    printLine(80);
}

// Display doctor's inpatients
// Returns count of inpatients found
static int showDoctorInpatients(DoctorNode* doctor, HospitalizationNode* hospHead, PatientNode* patientHead) {
    if (!doctor) return 0;

    printTitle("我的住院患者");
    printLine(90);
    std::cout << std::left
              << std::setw(14) << "住院ID"
              << std::setw(14) << "患者ID"
              << std::setw(12) << "患者姓名"
              << std::setw(12) << "病房ID"
              << std::setw(8)  << "床位号"
              << std::setw(8)  << "入院日"
              << std::setw(12) << "押金"
              << std::endl;
    printLine(90);

    int count = 0;
    HospitalizationNode* cur = hospHead;
    while (cur) {
        if (cur->status == 0) {
            // Check if this doctor is in doctorIDs array
            bool isMyPatient = false;
            for (int i = 0; i < cur->doctorCount; i++) {
                if (cur->doctorIDs[i] == doctor->doctorID) {
                    isMyPatient = true;
                    break;
                }
            }
            if (isMyPatient) {
                PatientNode* patient = findPatientByID(patientHead, cur->patientID);
                std::string patientName = patient ? patient->name : "未知";
                std::cout << std::left
                          << std::setw(14) << cur->hospID
                          << std::setw(14) << cur->patientID
                          << std::setw(12) << patientName
                          << std::setw(12) << cur->wardID
                          << std::setw(8)  << (cur->bedNo + 1)
                          << std::setw(8)  << ("Day" + std::to_string(cur->admitDay))
                          << std::setw(12) << std::fixed << std::setprecision(2) << cur->deposit
                          << std::endl;
                count++;
            }
        }
        cur = cur->next;
    }

    if (count == 0) {
        std::cout << "  (暂无您的住院患者)" << std::endl;
    }
    printLine(90);
    std::cout << "共 " << count << " 位住院患者" << std::endl;
    return count;
}

// Helper to check if a medicine is in the doctor's department
static bool isMedInDoctorDept(DoctorNode* doctor, const std::string& medID, DeptMedicineNode* deptMedHead) {
    if (!doctor) return false;
    DeptMedicineNode* dm = deptMedHead;
    while (dm) {
        if (dm->deptID == doctor->departmentID && dm->medID == medID) return true;
        dm = dm->next;
    }
    return false;
}

// ==================== Menu Options ====================

// Option 1: View waiting patients
static void menuViewWaitingPatients(DoctorNode* doctor, RegistrationNode* regHead, PatientNode* patientHead) {
    showWaitingPatients(doctor, regHead, patientHead);
}

// Option 2: Consult a patient
static void menuConsultPatient(DoctorNode* doctor, RegistrationNode*& regHead,
    ConsultationNode*& consultHead, PatientNode* patientHead) {
    printTitle("接诊患者");

    int waitingCount = showWaitingPatients(doctor, regHead, patientHead);
    if (waitingCount == 0) {
        std::cout << "没有待诊患者，无法接诊。" << std::endl;
        return;
    }

    std::cout << std::endl;
    std::string regID = readString("请输入挂号ID (输入0返回): ", 20);
    if (regID == "0") {
        std::cout << "已取消接诊。" << std::endl;
        return;
    }

    // Validate registration
    RegistrationNode* reg = findRegistrationByID(regHead, regID);
    if (!reg) {
        std::cout << "[错误] 挂号记录不存在！" << std::endl;
        return;
    }
    if (reg->doctorID != doctor->doctorID) {
        std::cout << "[错误] 该挂号记录不属于您！" << std::endl;
        return;
    }
    if (reg->status != STATUS_PENDING) {
        std::cout << "[错误] 该挂号状态为 " << getRegStatusStr(reg->status) << "，无法接诊！" << std::endl;
        return;
    }

    // Show patient info
    PatientNode* patient = findPatientByID(patientHead, reg->patientID);
    if (patient) {
        std::cout << "患者: " << patient->name << " (年龄: " << patient->age << ")" << std::endl;
    }

    // Input consultation details
    std::cout << std::endl;
    std::string complaint = readString("请输入主诉: ", MAX_DESC_LEN);
    std::string diagnosis = readString("请输入诊断: ", MAX_DESC_LEN);

    // Confirm
    std::cout << std::endl;
    std::cout << "===== 接诊信息确认 =====" << std::endl;
    std::cout << "挂号ID: " << regID << std::endl;
    std::cout << "患者ID: " << reg->patientID << std::endl;
    if (patient) std::cout << "患者: " << patient->name << std::endl;
    std::cout << "主诉: " << complaint << std::endl;
    std::cout << "诊断: " << diagnosis << std::endl;
    std::cout << "就诊日期: Day" << day << std::endl;

    if (!readConfirm("确认接诊? (y/n): ")) {
        std::cout << "已取消接诊。" << std::endl;
        return;
    }

    // Create consultation node
    ConsultationNode* cs = new ConsultationNode();
    cs->consultID = generateConsultID();
    cs->regID = reg->regID;
    cs->patientID = reg->patientID;
    cs->doctorID = doctor->doctorID;
    cs->complaint = complaint;
    cs->diagnosis = diagnosis;
    cs->consultDay = day;
    cs->status = STATUS_VALID;

    insertConsultationTail(consultHead, cs);

    // Update registration status
    reg->status = STATUS_SEEN;

    std::cout << std::endl;
    std::cout << "[成功] 接诊完成！就诊ID: " << cs->consultID << std::endl;
}

// Option 3: Prescribe medicine
static void menuPrescribe(DoctorNode* doctor, ConsultationNode* consultHead,
    PrescriptionNode*& prescHead, PrescMedicineNode*& prescMedHead,
    MedicineNode* medHead, DeptMedicineNode* deptMedHead) {
    printTitle("开具处方");

    std::string consultID = readString("请输入就诊ID (输入0返回): ", 20);
    if (consultID == "0") {
        std::cout << "已取消。" << std::endl;
        return;
    }

    // Validate consultation
    ConsultationNode* consult = findConsultationByID(consultHead, consultID);
    if (!consult) {
        std::cout << "[错误] 就诊记录不存在！" << std::endl;
        return;
    }
    if (consult->doctorID != doctor->doctorID) {
        std::cout << "[错误] 该就诊记录不属于您！" << std::endl;
        return;
    }
    if (consult->status != STATUS_VALID) {
        std::cout << "[错误] 该就诊记录状态不是有效状态！" << std::endl;
        return;
    }

    std::cout << "患者ID: " << consult->patientID << std::endl;
    std::cout << "诊断: " << consult->diagnosis << std::endl;
    std::cout << std::endl;

    // Show department medicines
    int medCount = showDeptMedicines(doctor, medHead, deptMedHead);
    if (medCount == 0) {
        std::cout << "[提示] 科室暂无可用药品，无法开具处方。" << std::endl;
        return;
    }

    std::cout << std::endl;

    // Create prescription
    PrescriptionNode* pr = new PrescriptionNode();
    pr->prescID = generatePrescID();
    pr->consultID = consultID;
    pr->patientID = consult->patientID;
    pr->doctorID = doctor->doctorID;
    pr->prescDay = day;
    pr->status = 0;
    pr->totalAmount = 0.0;

    bool addedAny = false;
    double totalAmount = 0.0;

    while (true) {
        std::cout << std::endl;
        std::string medID = readString("请输入药品ID (输入0结束添加): ", 20);
        if (medID == "0") {
            if (!addedAny) {
                std::cout << "尚未添加任何药品，处方将不会创建。" << std::endl;
                if (!readConfirm("是否放弃本次处方? (y/n): ")) {
                    continue;
                }
                delete pr;
                std::cout << "已取消处方。" << std::endl;
                return;
            }
            break;
        }

        // Validate medicine
        MedicineNode* med = findMedicineByID(medHead, medID);
        if (!med) {
            std::cout << "[错误] 药品ID不存在，请重新输入！" << std::endl;
            continue;
        }

        // Check if medicine belongs to doctor's department
        if (!isMedInDoctorDept(doctor, medID, deptMedHead)) {
            std::cout << "[错误] 该药品不属于您所在科室！" << std::endl;
            continue;
        }

        std::cout << "药品: " << med->tradeName << " (" << med->genericName << ")" << std::endl;
        std::cout << "规格: " << med->spec << "  库存: " << med->stock << std::endl;

        // Input quantity
        int quantity = readIntRange("请输入数量 (正数): ", 1, 99999);

        // Check stock
        if (quantity > med->stock) {
            std::cout << "[警告] 库存不足！当前库存为 " << med->stock
                      << "，您需要 " << quantity << "。" << std::endl;
            if (!readConfirm("仍然继续开具该药品? (y/n): ")) {
                continue;
            }
        }

        // Input unit price
        double unitPrice;
        std::cout << "请输入单价 (默认15.0, 输入0使用默认): ";
        unitPrice = readDouble("");
        if (unitPrice <= 0.0) {
            unitPrice = 15.0;
        }
        unitPrice = round2(unitPrice);

        // Create PrescMedicineNode
        PrescMedicineNode* pm = new PrescMedicineNode();
        pm->prescID = pr->prescID;
        pm->medID = medID;
        pm->quantity = quantity;
        pm->unitPrice = unitPrice;

        insertPrescMedicineTail(prescMedHead, pm);

        double subtotal = round2(quantity * unitPrice);
        totalAmount = round2(totalAmount + subtotal);

        std::cout << "[已添加] " << med->tradeName << " x" << quantity
                  << "  @" << std::fixed << std::setprecision(2) << unitPrice
                  << " = " << subtotal << " 元" << std::endl;

        addedAny = true;
    }

    if (!addedAny) {
        delete pr;
        std::cout << "未添加任何药品，处方未创建。" << std::endl;
        return;
    }

    pr->totalAmount = totalAmount;

    std::cout << std::endl;
    std::cout << "===== 处方确认 =====" << std::endl;
    std::cout << "处方ID: " << pr->prescID << std::endl;
    std::cout << "就诊ID: " << pr->consultID << std::endl;
    std::cout << "药品数量: " << addedAny << " 种" << std::endl;
    std::cout << "总金额: " << std::fixed << std::setprecision(2) << totalAmount << " 元" << std::endl;

    if (!readConfirm("确认开具此处方? (y/n): ")) {
        // Clean up orphaned PrescMedicineNodes
        PrescMedicineNode* pm = prescMedHead;
        PrescMedicineNode* prev = nullptr;
        while (pm) {
            if (pm->prescID == pr->prescID) {
                PrescMedicineNode* toDel = pm;
                if (prev) prev->next = pm->next;
                else prescMedHead = pm->next;
                pm = pm->next;
                delete toDel;
            } else {
                prev = pm;
                pm = pm->next;
            }
        }
        delete pr;
        std::cout << "已取消处方。" << std::endl;
        return;
    }

    insertPrescriptionTail(prescHead, pr);
    std::cout << "[成功] 处方开具完成！处方ID: " << pr->prescID << std::endl;
}

// Option 4: Order examination
static void menuOrderExamination(DoctorNode* doctor, ConsultationNode* consultHead,
    ExaminationNode*& examHead) {
    printTitle("开具检查");

    std::string consultID = readString("请输入就诊ID (输入0返回): ", 20);
    if (consultID == "0") {
        std::cout << "已取消。" << std::endl;
        return;
    }

    // Validate consultation
    ConsultationNode* consult = findConsultationByID(consultHead, consultID);
    if (!consult) {
        std::cout << "[错误] 就诊记录不存在！" << std::endl;
        return;
    }
    if (consult->doctorID != doctor->doctorID) {
        std::cout << "[错误] 该就诊记录不属于您！" << std::endl;
        return;
    }
    if (consult->status != STATUS_VALID) {
        std::cout << "[错误] 该就诊记录状态不是有效状态！" << std::endl;
        return;
    }

    std::cout << "患者ID: " << consult->patientID << std::endl;
    std::cout << "诊断: " << consult->diagnosis << std::endl;
    std::cout << std::endl;

    // Input examination details
    std::string itemName = readString("请输入检查项目名称: ", MAX_ITEM_LEN);
    double cost = readNonNegDouble("请输入检查费用: ");

    std::cout << std::endl;
    std::cout << "===== 检查确认 =====" << std::endl;
    std::cout << "就诊ID: " << consultID << std::endl;
    std::cout << "患者ID: " << consult->patientID << std::endl;
    std::cout << "检查项目: " << itemName << std::endl;
    std::cout << "费用: " << std::fixed << std::setprecision(2) << cost << " 元" << std::endl;
    std::cout << "日期: Day" << day << std::endl;

    if (!readConfirm("确认开具此检查? (y/n): ")) {
        std::cout << "已取消检查。" << std::endl;
        return;
    }

    // Create examination node
    ExaminationNode* exam = new ExaminationNode();
    exam->examID = generateExamID();
    exam->consultID = consultID;
    exam->patientID = consult->patientID;
    exam->itemName = itemName;
    exam->cost = round2(cost);
    exam->examDay = day;
    exam->status = STATUS_VALID;

    insertExaminationTail(examHead, exam);
    std::cout << "[成功] 检查开具完成！检查ID: " << exam->examID << std::endl;
}

// Option 5: View my inpatients
static void menuViewInpatients(DoctorNode* doctor, HospitalizationNode* hospHead,
    PatientNode* patientHead, ConsultationNode* consultHead,
    PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead,
    MedicineNode* medHead, ExaminationNode* examHead) {
    printTitle("我的住院患者");

    int count = showDoctorInpatients(doctor, hospHead, patientHead);
    if (count == 0) return;

    std::cout << std::endl;
    std::string patientID = readString("输入患者ID以查看详情 (输入0返回): ", 20);
    if (patientID == "0") {
        std::cout << "已返回。" << std::endl;
        return;
    }

    // Find the hospitalization record for this patient
    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
    if (!hosp) {
        std::cout << "[错误] 该患者不是您的在院患者或无活跃住院记录！" << std::endl;
        return;
    }

    // Verify the doctor is responsible
    bool isMyPatient = false;
    for (int i = 0; i < hosp->doctorCount; i++) {
        if (hosp->doctorIDs[i] == doctor->doctorID) {
            isMyPatient = true;
            break;
        }
    }
    if (!isMyPatient) {
        std::cout << "[错误] 该患者不是您负责的！" << std::endl;
        return;
    }

    // Display patient details
    PatientNode* patient = findPatientByID(patientHead, patientID);
    std::cout << std::endl;
    printTitle("患者详细信息");
    if (patient) {
        std::cout << "患者ID: " << patient->patientID << std::endl;
        std::cout << "姓名: " << patient->name << std::endl;
        std::cout << "年龄: " << patient->age << std::endl;
        std::cout << "联系方式: " << patient->contact << std::endl;
    }
    std::cout << "住院ID: " << hosp->hospID << std::endl;
    std::cout << "病房ID: " << hosp->wardID << "  床位号: " << (hosp->bedNo + 1) << std::endl;
    std::cout << "入院日期: Day" << hosp->admitDay << std::endl;
    std::cout << "押金余额: " << std::fixed << std::setprecision(2) << hosp->deposit << " 元" << std::endl;
    std::cout << std::endl;

    // Show all medical records for this patient
    showPatientConsultations(patientID, doctor, consultHead);
    std::cout << std::endl;
    showPatientPrescriptions(patientID, prescHead, prescMedHead, medHead);
    std::cout << std::endl;
    showPatientExaminations(patientID, examHead);
}

// Option 6: Prescribe or examine for inpatients
static void menuPrescribeForInpatients(DoctorNode* doctor, HospitalizationNode* hospHead,
    PatientNode* patientHead, ConsultationNode* consultHead,
    PrescriptionNode*& prescHead, PrescMedicineNode*& prescMedHead,
    MedicineNode* medHead, DeptMedicineNode* deptMedHead, ExaminationNode*& examHead) {
    printTitle("为住院患者开处方/检查");

    int count = showDoctorInpatients(doctor, hospHead, patientHead);
    if (count == 0) return;

    std::cout << std::endl;
    std::string patientID = readString("请选择患者ID (输入0返回): ", 20);
    if (patientID == "0") {
        std::cout << "已返回。" << std::endl;
        return;
    }

    // Find and validate the hospitalization record
    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
    if (!hosp) {
        std::cout << "[错误] 该患者无活跃住院记录！" << std::endl;
        return;
    }

    bool isMyPatient = false;
    for (int i = 0; i < hosp->doctorCount; i++) {
        if (hosp->doctorIDs[i] == doctor->doctorID) {
            isMyPatient = true;
            break;
        }
    }
    if (!isMyPatient) {
        std::cout << "[错误] 该患者不是您负责的！" << std::endl;
        return;
    }

    // Show patient info
    PatientNode* patient = findPatientByID(patientHead, patientID);
    std::cout << "患者: " << (patient ? patient->name : "未知")
              << "  住院ID: " << hosp->hospID
              << "  床位: " << (hosp->bedNo + 1) << std::endl;

    // Sub-menu
    while (true) {
        std::cout << std::endl;
        std::cout << "----- 住院患者操作 -----" << std::endl;
        std::cout << "1. 开具处方" << std::endl;
        std::cout << "2. 开具检查" << std::endl;
        std::cout << "0. 返回上级" << std::endl;
        std::cout << "------------------------" << std::endl;

        int choice = readIntRange("请选择操作: ", 0, 2);

        if (choice == 0) {
            break;
        } else if (choice == 1) {
            // Prescribe for inpatient - consultID is optional, use hospID as reference
            std::cout << std::endl;
            printTitle("为住院患者开具处方");

            // Show department medicines
            int medCount = showDeptMedicines(doctor, medHead, deptMedHead);
            if (medCount == 0) {
                std::cout << "[提示] 科室暂无可用药品，无法开具处方。" << std::endl;
                continue;
            }

            std::cout << std::endl;

            // Optional consultID
            std::string consultID = readOptionalString("是否关联就诊ID? (直接回车跳过): ", 20);
            if (!consultID.empty()) {
                // Validate if provided
                ConsultationNode* consult = findConsultationByID(consultHead, consultID);
                if (!consult || consult->patientID != patientID) {
                    std::cout << "[警告] 就诊ID无效或不属于该患者，将不关联就诊记录。" << std::endl;
                    consultID = "";
                }
            }

            // Create prescription
            PrescriptionNode* pr = new PrescriptionNode();
            pr->prescID = generatePrescID();
            pr->consultID = consultID;  // May be empty
            pr->patientID = patientID;
            pr->doctorID = doctor->doctorID;
            pr->prescDay = day;
            pr->status = 0;
            pr->totalAmount = 0.0;

            bool addedAny = false;
            double totalAmount = 0.0;

            while (true) {
                std::cout << std::endl;
                std::string medID = readString("请输入药品ID (输入0结束添加): ", 20);
                if (medID == "0") {
                    if (!addedAny) {
                        std::cout << "尚未添加任何药品，处方将不会创建。" << std::endl;
                        if (!readConfirm("是否放弃本次处方? (y/n): ")) {
                            continue;
                        }
                        delete pr;
                        std::cout << "已取消处方。" << std::endl;
                        break;
                    }
                    break;
                }

                MedicineNode* med = findMedicineByID(medHead, medID);
                if (!med) {
                    std::cout << "[错误] 药品ID不存在，请重新输入！" << std::endl;
                    continue;
                }

                if (!isMedInDoctorDept(doctor, medID, deptMedHead)) {
                    std::cout << "[错误] 该药品不属于您所在科室！" << std::endl;
                    continue;
                }

                std::cout << "药品: " << med->tradeName << " (" << med->genericName << ")" << std::endl;
                std::cout << "规格: " << med->spec << "  库存: " << med->stock << std::endl;

                int quantity = readIntRange("请输入数量 (正数): ", 1, 99999);

                if (quantity > med->stock) {
                    std::cout << "[警告] 库存不足！当前库存为 " << med->stock
                              << "，您需要 " << quantity << "。" << std::endl;
                    if (!readConfirm("仍然继续开具该药品? (y/n): ")) {
                        continue;
                    }
                }

                double unitPrice;
                std::cout << "请输入单价 (默认15.0, 输入0使用默认): ";
                unitPrice = readDouble("");
                if (unitPrice <= 0.0) {
                    unitPrice = 15.0;
                }
                unitPrice = round2(unitPrice);

                PrescMedicineNode* pm = new PrescMedicineNode();
                pm->prescID = pr->prescID;
                pm->medID = medID;
                pm->quantity = quantity;
                pm->unitPrice = unitPrice;

                insertPrescMedicineTail(prescMedHead, pm);

                double subtotal = round2(quantity * unitPrice);
                totalAmount = round2(totalAmount + subtotal);

                std::cout << "[已添加] " << med->tradeName << " x" << quantity
                          << "  @" << std::fixed << std::setprecision(2) << unitPrice
                          << " = " << subtotal << " 元" << std::endl;

                addedAny = true;
            }

            if (!addedAny) {
                // pr has already been deleted in the cancel path above,
                // so we just continue the outer sub-menu loop
                continue;
            }

            pr->totalAmount = totalAmount;

            std::cout << std::endl;
            std::cout << "===== 处方确认 =====" << std::endl;
            std::cout << "处方ID: " << pr->prescID << std::endl;
            std::cout << "患者ID: " << pr->patientID << std::endl;
            if (!consultID.empty()) std::cout << "关联就诊ID: " << consultID << std::endl;
            else std::cout << "关联就诊ID: (无)" << std::endl;
            std::cout << "药品数量: " << addedAny << " 种" << std::endl;
            std::cout << "总金额: " << std::fixed << std::setprecision(2) << totalAmount << " 元" << std::endl;

            if (!readConfirm("确认开具此处方? (y/n): ")) {
                // Clean up orphaned PrescMedicineNodes
                PrescMedicineNode* pm = prescMedHead;
                PrescMedicineNode* prev = nullptr;
                while (pm) {
                    if (pm->prescID == pr->prescID) {
                        PrescMedicineNode* toDel = pm;
                        if (prev) prev->next = pm->next;
                        else prescMedHead = pm->next;
                        pm = pm->next;
                        delete toDel;
                    } else {
                        prev = pm;
                        pm = pm->next;
                    }
                }
                delete pr;
                std::cout << "已取消处方。" << std::endl;
                continue;
            }

            insertPrescriptionTail(prescHead, pr);
            std::cout << "[成功] 处方开具完成！处方ID: " << pr->prescID << std::endl;

        } else if (choice == 2) {
            // Order examination for inpatient
            std::cout << std::endl;
            printTitle("为住院患者开具检查");

            // Optional consultID
            std::string consultID = readOptionalString("是否关联就诊ID? (直接回车跳过): ", 20);
            if (!consultID.empty()) {
                ConsultationNode* consult = findConsultationByID(consultHead, consultID);
                if (!consult || consult->patientID != patientID) {
                    std::cout << "[警告] 就诊ID无效或不属于该患者，将不关联就诊记录。" << std::endl;
                    consultID = "";
                }
            }

            std::string itemName = readString("请输入检查项目名称: ", MAX_ITEM_LEN);
            double cost = readNonNegDouble("请输入检查费用: ");

            std::cout << std::endl;
            std::cout << "===== 检查确认 =====" << std::endl;
            std::cout << "患者ID: " << patientID << std::endl;
            if (!consultID.empty()) std::cout << "关联就诊ID: " << consultID << std::endl;
            else std::cout << "关联就诊ID: (无)" << std::endl;
            std::cout << "检查项目: " << itemName << std::endl;
            std::cout << "费用: " << std::fixed << std::setprecision(2) << cost << " 元" << std::endl;
            std::cout << "日期: Day" << day << std::endl;

            if (!readConfirm("确认开具此检查? (y/n): ")) {
                std::cout << "已取消检查。" << std::endl;
                continue;
            }

            ExaminationNode* exam = new ExaminationNode();
            exam->examID = generateExamID();
            exam->consultID = consultID;  // May be empty
            exam->patientID = patientID;
            exam->itemName = itemName;
            exam->cost = round2(cost);
            exam->examDay = day;
            exam->status = STATUS_VALID;

            insertExaminationTail(examHead, exam);
            std::cout << "[成功] 检查开具完成！检查ID: " << exam->examID << std::endl;
        }
    }
}

// ==================== Main Doctor Menu ====================

bool doctorMenu(DoctorNode*& doctorHead, PatientNode* patientHead, RegistrationNode*& regHead,
    ConsultationNode*& consultHead, ExaminationNode*& examHead, PrescriptionNode*& prescHead,
    PrescMedicineNode*& prescMedHead, MedicineNode* medHead, DeptMedicineNode* deptMedHead,
    WardNode* wardHead, HospitalizationNode* hospHead) {

    // 1. Doctor login - ask for doctor ID
    printTitle("医生登录");
    std::string doctorID = readString("请输入医生ID (输入0退出): ", 20);

    if (doctorID == "0") {
        std::cout << "已取消登录。" << std::endl;
        return false;
    }

    DoctorNode* doctor = findDoctorByID(doctorHead, doctorID);
    if (!doctor) {
        std::cout << "[错误] 医生ID不存在！" << std::endl;
        return false;
    }

    // Check if today is a working day for this doctor
    if (!doctorWorksOnDay(doctor, (int)week)) {
        std::cout << "[错误] 今天(" << WEEKDAY_NAMES[week - 1] << ")不是您的工作日，不能登录！" << std::endl;
        std::cout << "您的工作日为: ";
        for (int i = 0; i < doctor->workDayCount; i++) {
            if (i > 0) std::cout << "、";
            std::cout << WEEKDAY_NAMES[doctor->workDays[i] - 1];
        }
        std::cout << std::endl;
        return false;
    }

    std::cout << std::endl;
    std::cout << "欢迎您，" << doctor->name << " " << getLevelName(doctor->level)
              << " (科室: " << doctor->departmentID << ")" << std::endl;
    std::cout << "当前: " << formatTime() << " " << WEEKDAY_NAMES[week - 1] << std::endl;
    pauseScreen();

    // 2. Main menu loop
    bool loggedIn = true;
    while (loggedIn) {
        std::cout << std::endl;
        printTitle("医生工作台");
        printStatus();
        std::cout << "当前医生: " << doctor->name << " (" << doctor->doctorID << ")"
                  << "  科室: " << doctor->departmentID << std::endl;
        std::cout << std::endl;
        std::cout << "===== 功能菜单 =====" << std::endl;
        std::cout << "1. 查看待诊患者" << std::endl;
        std::cout << "2. 接诊" << std::endl;
        std::cout << "3. 开具处方" << std::endl;
        std::cout << "4. 开具检查" << std::endl;
        std::cout << "5. 查看我的住院患者" << std::endl;
        std::cout << "6. 为住院患者开处方/检查" << std::endl;
        std::cout << "7. 查询报表" << std::endl;
        std::cout << "0. 退出登录" << std::endl;
        std::cout << "===================" << std::endl;

        int choice = readIntRange("请选择功能: ", 0, 7);

        std::cout << std::endl;

        switch (choice) {
            case 0:
                loggedIn = false;
                std::cout << "已退出登录，再见 " << doctor->name << " 医生！" << std::endl;
                break;

            case 1:
                menuViewWaitingPatients(doctor, regHead, patientHead);
                pauseScreen();
                break;

            case 2:
                menuConsultPatient(doctor, regHead, consultHead, patientHead);
                pauseScreen();
                break;

            case 3:
                menuPrescribe(doctor, consultHead, prescHead, prescMedHead, medHead, deptMedHead);
                pauseScreen();
                break;

            case 4:
                menuOrderExamination(doctor, consultHead, examHead);
                pauseScreen();
                break;

            case 5:
                menuViewInpatients(doctor, hospHead, patientHead, consultHead,
                    prescHead, prescMedHead, medHead, examHead);
                pauseScreen();
                break;

            case 6:
                menuPrescribeForInpatients(doctor, hospHead, patientHead, consultHead,
                    prescHead, prescMedHead, medHead, deptMedHead, examHead);
                pauseScreen();
                break;

            case 7:
                // Delegate to reports.h
                doctorReportMenu(doctorID, nullptr, doctorHead, patientHead, regHead,
                    consultHead, examHead, prescHead, prescMedHead, medHead,
                    wardHead, hospHead);
                pauseScreen();
                break;

            default:
                std::cout << "[错误] 无效选择。" << std::endl;
                break;
        }
    }

    return true;
}
