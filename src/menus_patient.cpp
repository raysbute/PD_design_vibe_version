#include "menus_patient.h"
#include "list_ops.h"
#include "utils.h"
#include "global.h"
#include <cstdio>

// ──────────────────────────────────────────────
// Helper: view all medical records for a patient
// ──────────────────────────────────────────────
static void viewMedicalRecords(
    const std::string& patientID,
    RegistrationNode* regHead,
    ConsultationNode* consultHead,
    ExaminationNode* examHead,
    PrescriptionNode* prescHead,
    PrescMedicineNode* prescMedHead,
    MedicineNode* medHead,
    DoctorNode* doctorHead)
{
    printTitle("我的就诊记录");

    // ── Registrations ──
    std::cout << "\n[挂号记录]" << std::endl;
    printLine(70);
    std::cout << std::left
              << std::setw(14) << "挂号ID"
              << std::setw(10) << "科室ID"
              << std::setw(10) << "医生ID"
              << std::setw(12) << "医生姓名"
              << std::setw(8)  << "日期"
              << std::setw(10) << "状态"
              << std::endl;
    printLine(70);

    bool foundReg = false;
    RegistrationNode* r = regHead;
    while (r) {
        if (r->patientID == patientID) {
            foundReg = true;
            DoctorNode* doc = findDoctorByID(doctorHead, r->doctorID);
            const char* docName = doc ? doc->name.c_str() : "未知";
            std::cout << std::left
                      << std::setw(14) << r->regID
                      << std::setw(10) << r->departmentID
                      << std::setw(10) << r->doctorID
                      << std::setw(12) << docName
                      << std::setw(8)  << ("Day" + std::to_string(r->regDay))
                      << std::setw(10) << getRegStatusStr(r->status)
                      << std::endl;
        }
        r = r->next;
    }
    if (!foundReg) {
        std::cout << "  (无挂号记录)" << std::endl;
    }

    // ── Consultations ──
    std::cout << "\n[就诊记录]" << std::endl;
    printLine(70);
    std::cout << std::left
              << std::setw(14) << "就诊ID"
              << std::setw(10) << "医生ID"
              << std::setw(24) << "主诉"
              << std::setw(12) << "诊断"
              << std::setw(8)  << "日期"
              << std::endl;
    printLine(70);

    bool foundConsult = false;
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->patientID == patientID) {
            foundConsult = true;
            std::cout << std::left
                      << std::setw(14) << c->consultID
                      << std::setw(10) << c->doctorID
                      << std::setw(24) << (c->complaint.length() > 22 ? c->complaint.substr(0, 22) + ".." : c->complaint)
                      << std::setw(12) << (c->diagnosis.length() > 10 ? c->diagnosis.substr(0, 10) + ".." : c->diagnosis)
                      << std::setw(8)  << ("Day" + std::to_string(c->consultDay))
                      << std::endl;
        }
        c = c->next;
    }
    if (!foundConsult) {
        std::cout << "  (无就诊记录)" << std::endl;
    }

    // ── Examinations ──
    std::cout << "\n[检查记录]" << std::endl;
    printLine(60);
    std::cout << std::left
              << std::setw(14) << "检查ID"
              << std::setw(18) << "项目名称"
              << std::setw(10) << "费用"
              << std::setw(8)  << "日期"
              << std::setw(10) << "状态"
              << std::endl;
    printLine(60);

    bool foundExam = false;
    ExaminationNode* e = examHead;
    while (e) {
        if (e->patientID == patientID) {
            foundExam = true;
            std::cout << std::left
                      << std::setw(14) << e->examID
                      << std::setw(18) << (e->itemName.length() > 16 ? e->itemName.substr(0, 16) + ".." : e->itemName)
                      << std::setw(10) << std::fixed << std::setprecision(2) << e->cost
                      << std::setw(8)  << ("Day" + std::to_string(e->examDay))
                      << std::setw(10) << getRecordStatusStr(e->status)
                      << std::endl;
        }
        e = e->next;
    }
    if (!foundExam) {
        std::cout << "  (无检查记录)" << std::endl;
    }

    // ── Prescriptions ──
    std::cout << "\n[处方记录]" << std::endl;
    printLine(70);
    std::cout << std::left
              << std::setw(14) << "处方ID"
              << std::setw(12) << "总金额"
              << std::setw(8)  << "日期"
              << std::setw(10) << "状态"
              << "药品明细"
              << std::endl;
    printLine(70);

    bool foundPresc = false;
    PrescriptionNode* p = prescHead;
    while (p) {
        if (p->patientID == patientID) {
            foundPresc = true;
            std::cout << std::left
                      << std::setw(14) << p->prescID
                      << std::setw(12) << std::fixed << std::setprecision(2) << p->totalAmount
                      << std::setw(8)  << ("Day" + std::to_string(p->prescDay))
                      << std::setw(10) << getPrescStatusStr(p->status);

            // Show medicine details for this prescription
            bool firstMed = true;
            PrescMedicineNode* pm = prescMedHead;
            while (pm) {
                if (pm->prescID == p->prescID) {
                    MedicineNode* med = findMedicineByID(medHead, pm->medID);
                    const char* medName = med ? med->tradeName.c_str() : pm->medID.c_str();
                    if (firstMed) {
                        std::cout << medName << " x" << pm->quantity;
                        firstMed = false;
                    } else {
                        std::cout << ", " << medName << " x" << pm->quantity;
                    }
                }
                pm = pm->next;
            }
            std::cout << std::endl;
        }
        p = p->next;
    }
    if (!foundPresc) {
        std::cout << "  (无处方记录)" << std::endl;
    }

    printLine(70);
    pauseScreen();
}

// ──────────────────────────────────────────────
// Helper: pay fees for a patient
// ──────────────────────────────────────────────
static void payFees(
    const std::string& patientID,
    int patientType,
    PatientNode* patient,
    RegistrationNode* regHead,
    PrescriptionNode* prescHead,
    PrescMedicineNode*& prescMedHead,
    MedicineNode*& medHead,
    HospitalizationNode* hospHead)
{
    printTitle("支付费用");

    // ── Collect unpaid registrations ──
    std::cout << "\n[待缴挂号费]" << std::endl;
    printLine(50);
    int unpaidRegCount = 0;
    RegistrationNode* r = regHead;
    while (r) {
        if (r->patientID == patientID && r->status == STATUS_PENDING) {
            unpaidRegCount++;
            std::cout << "  挂号 " << r->regID
                      << "  科室 " << r->departmentID
                      << "  日期 Day" << r->regDay
                      << "  费用 " << std::fixed << std::setprecision(2) << REGISTRATION_FEE << " 元"
                      << std::endl;
        }
        r = r->next;
    }
    if (unpaidRegCount == 0) {
        std::cout << "  (无待缴挂号费)" << std::endl;
    }
    double totalRegFee = unpaidRegCount * REGISTRATION_FEE;

    // ── Collect unpaid prescriptions ──
    std::cout << "\n[待缴处方费]" << std::endl;
    printLine(50);
    int unpaidPrescCount = 0;
    double totalPrescFee = 0.0;
    PrescriptionNode* p = prescHead;
    while (p) {
        if (p->patientID == patientID && p->status == 0) {  // 0 = unpaid
            unpaidPrescCount++;
            totalPrescFee += p->totalAmount;
            std::cout << "  处方 " << p->prescID
                      << "  日期 Day" << p->prescDay
                      << "  金额 " << std::fixed << std::setprecision(2) << p->totalAmount << " 元"
                      << std::endl;
        }
        p = p->next;
    }
    if (unpaidPrescCount == 0) {
        std::cout << "  (无待缴处方费)" << std::endl;
    }

    if (unpaidRegCount == 0 && unpaidPrescCount == 0) {
        std::cout << "\n没有待缴费用。" << std::endl;
        pauseScreen();
        return;
    }

    // ── Payment options ──
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << "  挂号费合计:  " << std::fixed << std::setprecision(2) << totalRegFee << " 元  (" << unpaidRegCount << " 笔)" << std::endl;
    std::cout << "  处方费合计:  " << std::fixed << std::setprecision(2) << totalPrescFee << " 元  (" << unpaidPrescCount << " 笔)" << std::endl;
    std::cout << "  全部合计:    " << std::fixed << std::setprecision(2) << (totalRegFee + totalPrescFee) << " 元" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    if (patientType == PATIENT_INPATIENT) {
        // Show current deposit
        HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
        double curDeposit = hosp ? hosp->deposit : 0.0;
        std::cout << "  当前住院押金: " << std::fixed << std::setprecision(2) << curDeposit << " 元" << std::endl;
        std::cout << "  (费用将从住院押金中扣除)" << std::endl;
    }

    std::cout << "\n支付选项:" << std::endl;
    std::cout << "  1 = 全部支付" << std::endl;
    std::cout << "  2 = 仅支付挂号费" << std::endl;
    std::cout << "  3 = 支付指定处方" << std::endl;
    std::cout << "  0 = 取消" << std::endl;

    int choice = readIntRange("请选择: ", 0, 3);
    if (choice == 0) {
        std::cout << "已取消支付。" << std::endl;
        pauseScreen();
        return;
    }

    double amountToPay = 0.0;
    std::string selectedPrescID;  // for choice 3, scoped to outer block

    if (choice == 1) {
        // Pay all
        amountToPay = round2(totalRegFee + totalPrescFee);
    } else if (choice == 2) {
        // Pay registration fees only
        if (unpaidRegCount == 0) {
            std::cout << "没有待缴挂号费。" << std::endl;
            pauseScreen();
            return;
        }
        amountToPay = round2(totalRegFee);
    } else if (choice == 3) {
        // Pay specific prescription
        selectedPrescID = readString("请输入处方ID: ", 20);
        PrescriptionNode* target = findPrescriptionByID(prescHead, selectedPrescID);
        if (!target) {
            std::cout << "[错误] 处方 " << selectedPrescID << " 不存在。" << std::endl;
            pauseScreen();
            return;
        }
        if (target->patientID != patientID) {
            std::cout << "[错误] 该处方不属于当前患者。" << std::endl;
            pauseScreen();
            return;
        }
        if (target->status != 0) {
            std::cout << "[错误] 该处方不是待缴费状态(当前状态: " << getPrescStatusStr(target->status) << ")。" << std::endl;
            pauseScreen();
            return;
        }
        amountToPay = round2(target->totalAmount);
    }

    if (amountToPay <= 0.0) {
        std::cout << "支付金额为0，无需支付。" << std::endl;
        pauseScreen();
        return;
    }

    std::cout << "\n支付金额: " << std::fixed << std::setprecision(2) << amountToPay << " 元" << std::endl;
    if (!readConfirm("确认支付? (y/n): ")) {
        std::cout << "已取消支付。" << std::endl;
        pauseScreen();
        return;
    }

    // ── Process payment ──
    if (patientType == PATIENT_OUTPATIENT) {
        // Outpatient: patient pays directly to hospital
        money = round2(money + amountToPay);
        std::cout << "\n[支付成功] 已支付 " << std::fixed << std::setprecision(2) << amountToPay << " 元。" << std::endl;
        std::cout << "医院当前资金: " << std::fixed << std::setprecision(2) << money << " 元" << std::endl;
    } else {
        // Inpatient: deduct from deposit
        HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
        if (!hosp) {
            std::cout << "[错误] 未找到在院记录，无法扣款。" << std::endl;
            pauseScreen();
            return;
        }
        hosp->deposit = round2(hosp->deposit - amountToPay);
        money = round2(money + amountToPay);
        // Sync patient record deposit
        patient->deposit = hosp->deposit;
        std::cout << "\n[支付成功] 已从押金扣除 " << std::fixed << std::setprecision(2) << amountToPay << " 元。" << std::endl;
        std::cout << "剩余押金: " << std::fixed << std::setprecision(2) << hosp->deposit << " 元" << std::endl;
        std::cout << "医院当前资金: " << std::fixed << std::setprecision(2) << money << " 元" << std::endl;
        if (hosp->deposit < 0.0) {
            std::cout << "[警告] 押金已不足，当前欠费 " << std::fixed << std::setprecision(2) << (-hosp->deposit) << " 元，请及时补缴！" << std::endl;
        }
    }

    // ── Update statuses ──
    if (choice == 1 || choice == 2) {
        // Mark all unpaid registrations as seen
        r = regHead;
        while (r) {
            if (r->patientID == patientID && r->status == STATUS_PENDING) {
                r->status = STATUS_SEEN;
            }
            r = r->next;
        }
        std::cout << "挂号费已结清，" << unpaidRegCount << " 笔挂号状态更新为已就诊。" << std::endl;
    }

    if (choice == 1) {
        // Pay all prescriptions
        p = prescHead;
        while (p) {
            if (p->patientID == patientID && p->status == 0) {
                p->status = 2;  // paid
                // Deduct medicine stock
                PrescMedicineNode* pm = prescMedHead;
                while (pm) {
                    if (pm->prescID == p->prescID) {
                        MedicineNode* med = findMedicineByID(medHead, pm->medID);
                        if (med) {
                            med->stock -= pm->quantity;
                            med->consumed += pm->quantity;
                            if (med->stock < 0) med->stock = 0;  // safety guard
                        }
                    }
                    pm = pm->next;
                }
            }
            p = p->next;
        }
        std::cout << "处方费已结清，" << unpaidPrescCount << " 笔处方状态更新为已缴费，药品库存已扣减。" << std::endl;
    } else if (choice == 3) {
        // Pay specific prescription
        PrescriptionNode* target = findPrescriptionByID(prescHead, selectedPrescID);
        if (target) {
            target->status = 2;  // paid
            // Deduct medicine stock
            PrescMedicineNode* pm = prescMedHead;
            while (pm) {
                if (pm->prescID == target->prescID) {
                    MedicineNode* med = findMedicineByID(medHead, pm->medID);
                    if (med) {
                        med->stock -= pm->quantity;
                        med->consumed += pm->quantity;
                        if (med->stock < 0) med->stock = 0;
                    }
                }
                pm = pm->next;
            }
            std::cout << "处方 " << selectedPrescID << " 已缴费，药品库存已扣减。" << std::endl;
        }
    }

    pauseScreen();
}

// ──────────────────────────────────────────────
// Helper: view hospitalization info (inpatient only)
// ──────────────────────────────────────────────
static void viewHospitalizationInfo(
    const std::string& patientID,
    HospitalizationNode* hospHead,
    WardNode* wardHead,
    DoctorNode* doctorHead)
{
    printTitle("住院信息");

    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
    if (!hosp) {
        std::cout << "\n[提示] 当前无在院记录。" << std::endl;
        pauseScreen();
        return;
    }

    WardNode* ward = findWardByID(wardHead, hosp->wardID);
    const char* wardTypeName = ward ? getWardTypeName(ward->type) : "未知";

    unsigned int admitDays = day - hosp->admitDay;
    double totalFees = round2((double)admitDays * HOSPITAL_DAILY_FEE);
    double remainingDeposit = round2(hosp->deposit - totalFees);

    std::cout << std::endl;
    printLine(50);
    std::cout << "  住院ID:     " << hosp->hospID << std::endl;
    std::cout << "  病房ID:     " << hosp->wardID << std::endl;
    std::cout << "  病房类型:   " << wardTypeName << std::endl;
    std::cout << "  床位号:     " << (hosp->bedNo + 1) << std::endl;
    std::cout << "  入院日期:   Day" << hosp->admitDay << std::endl;
    std::cout << "  当前日期:   Day" << day << std::endl;
    std::cout << "  已住院天数: " << admitDays << " 天" << std::endl;
    std::cout << "  押金余额:   " << std::fixed << std::setprecision(2) << hosp->deposit << " 元" << std::endl;
    printLine(50);

    // Responsible doctors
    std::cout << "\n[主管医生]" << std::endl;
    if (hosp->doctorCount > 0) {
        for (int i = 0; i < hosp->doctorCount; i++) {
            DoctorNode* doc = findDoctorByID(doctorHead, hosp->doctorIDs[i]);
            const char* docName = doc ? doc->name.c_str() : "未知";
            const char* docLevel = doc ? getLevelName(doc->level) : "未知";
            std::cout << "  " << (i + 1) << ". " << hosp->doctorIDs[i]
                      << "  " << docName
                      << "  (" << docLevel << ")"
                      << std::endl;
        }
    } else {
        std::cout << "  (无主管医生)" << std::endl;
    }

    // Fee summary
    std::cout << "\n[费用概览]" << std::endl;
    printLine(40);
    std::cout << "  每日费用:   " << std::fixed << std::setprecision(2) << HOSPITAL_DAILY_FEE << " 元/天" << std::endl;
    std::cout << "  已住天数:   " << admitDays << " 天" << std::endl;
    std::cout << "  累计费用:   " << std::fixed << std::setprecision(2) << totalFees << " 元" << std::endl;
    std::cout << "  押金余额:   " << std::fixed << std::setprecision(2) << hosp->deposit << " 元" << std::endl;
    std::cout << "  剩余押金:   " << std::fixed << std::setprecision(2) << remainingDeposit << " 元" << std::endl;
    printLine(40);

    if (remainingDeposit < 0.0) {
        std::cout << "\n[警告] 押金不足，当前欠费 " << std::fixed << std::setprecision(2) << (-remainingDeposit) << " 元，请及时补缴！" << std::endl;
    } else if (remainingDeposit < HOSPITAL_DAILY_FEE) {
        std::cout << "\n[提示] 押金余额不足一天的住院费用，建议尽快补缴。" << std::endl;
    }

    pauseScreen();
}

// ──────────────────────────────────────────────
// Patient menu main entry
// ──────────────────────────────────────────────
bool patientMenu(
    PatientNode* patientHead,
    RegistrationNode* regHead,
    ConsultationNode* consultHead,
    ExaminationNode* examHead,
    PrescriptionNode*& prescHead,
    PrescMedicineNode*& prescMedHead,
    MedicineNode*& medHead,
    WardNode* wardHead,
    HospitalizationNode* hospHead,
    DoctorNode* doctorHead)
{
    printTitle("患者登录");

    std::string patientID = readString("请输入患者ID: ", 10);

    PatientNode* patient = findPatientByID(patientHead, patientID);
    if (!patient) {
        std::cout << "[错误] 患者 " << patientID << " 不存在！" << std::endl;
        pauseScreen();
        return false;
    }

    std::cout << "欢迎, " << patient->name
              << " (" << (patient->type == PATIENT_INPATIENT ? "住院患者" : "门诊患者") << ")!"
              << std::endl;

    bool isInpatient = (patient->type == PATIENT_INPATIENT);

    while (true) {
        std::cout << std::endl;
        printStatus();
        std::cout << "当前患者: " << patient->name << " [" << patientID << "]" << std::endl;
        printLine(40);
        std::cout << "  1. 查看我的就诊记录" << std::endl;
        std::cout << "  2. 支付费用" << std::endl;
        if (isInpatient) {
            std::cout << "  3. 查看住院信息" << std::endl;
        }
        std::cout << "  0. 退出登录" << std::endl;
        printLine(40);

        int maxOption = isInpatient ? 3 : 2;
        int choice = readIntRange("请选择: ", 0, maxOption);

        switch (choice) {
            case 0:
                std::cout << "已退出登录。" << std::endl;
                return true;  // return true to continue main program loop

            case 1:
                viewMedicalRecords(patientID, regHead, consultHead, examHead,
                                   prescHead, prescMedHead, medHead, doctorHead);
                break;

            case 2:
                payFees(patientID, patient->type, patient, regHead,
                        prescHead, prescMedHead, medHead, hospHead);
                break;

            case 3:
                if (isInpatient) {
                    viewHospitalizationInfo(patientID, hospHead, wardHead, doctorHead);
                }
                break;

            default:
                std::cout << "[错误] 无效选项，请重新选择。" << std::endl;
                break;
        }
    }

    return true;
}
