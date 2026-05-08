#include "reports.h"
#include "utils.h"
#include <cstdio>

extern DepartmentNode* deptHead;
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

// ==================== Helper: resolve department name ====================
static std::string getDeptName(DepartmentNode* deptHead, const std::string& deptID) {
    DepartmentNode* d = findDepartmentByID(deptHead, deptID);
    return d ? d->name : deptID;
}

// ==================== Helper: resolve doctor name ====================
static std::string getDocName(DoctorNode* docHead, const std::string& doctorID) {
    DoctorNode* d = findDoctorByID(docHead, doctorID);
    return d ? d->name : doctorID;
}

// ==================== Helper: resolve patient name ====================
static std::string getPatName(PatientNode* patHead, const std::string& patientID) {
    PatientNode* p = findPatientByID(patHead, patientID);
    return p ? p->name : patientID;
}

// ==================== Helper: resolve medicine name ====================
static std::string getMedName(MedicineNode* medHead, const std::string& medID) {
    MedicineNode* m = findMedicineByID(medHead, medID);
    return m ? m->tradeName : medID;
}

// ==================== Helper: display a single patient's complete medical records ====================
static void displayPatientRecords(const std::string& patientID,
    PatientNode* patientHead, RegistrationNode* regHead, ConsultationNode* consultHead,
    ExaminationNode* examHead, PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead,
    MedicineNode* medHead, WardNode* wardHead, HospitalizationNode* hospHead,
    DoctorNode* docHead, DepartmentNode* deptHead)
{
    PatientNode* pat = findPatientByID(patientHead, patientID);
    if (!pat) {
        printf("  患者 %s 不存在。\n", patientID.c_str());
        return;
    }

    printLine(70);
    printf("  患者: %s | 姓名: %s | 年龄: %d | 联系方式: %s\n",
           pat->patientID.c_str(), pat->name.c_str(), pat->age, pat->contact.c_str());
    printf("  类型: %s\n", pat->type == 0 ? "门诊" : "住院");
    printLine(70);

    // --- Registrations ---
    printf("\n  [挂号记录]\n");
    bool foundAny = false;
    RegistrationNode* r = regHead;
    while (r) {
        if (r->patientID == patientID) {
            foundAny = true;
            printf("    %s | Day%u | 科室: %s | 医生: %s | 状态: %s\n",
                   r->regID.c_str(), r->regDay,
                   getDeptName(deptHead, r->departmentID).c_str(),
                   getDocName(docHead, r->doctorID).c_str(),
                   getRegStatusStr(r->status));
        }
        r = r->next;
    }
    if (!foundAny) printf("    (无)\n");

    // --- Consultations ---
    printf("\n  [诊疗记录]\n");
    foundAny = false;
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->patientID == patientID) {
            foundAny = true;
            printf("    %s | Day%u | 医生: %s | 挂号: %s\n",
                   c->consultID.c_str(), c->consultDay,
                   getDocName(docHead, c->doctorID).c_str(), c->regID.c_str());
            printf("      主诉: %s\n", c->complaint.c_str());
            printf("      诊断: %s\n", c->diagnosis.c_str());
            printf("      状态: %s\n", getRecordStatusStr(c->status));
        }
        c = c->next;
    }
    if (!foundAny) printf("    (无)\n");

    // --- Examinations ---
    printf("\n  [检查记录]\n");
    foundAny = false;
    ExaminationNode* e = examHead;
    while (e) {
        if (e->patientID == patientID) {
            foundAny = true;
            printf("    %s | Day%u | 项目: %s | 费用: %.2f | 状态: %s\n",
                   e->examID.c_str(), e->examDay, e->itemName.c_str(),
                   e->cost, getRecordStatusStr(e->status));
        }
        e = e->next;
    }
    if (!foundAny) printf("    (无)\n");

    // --- Prescriptions ---
    printf("\n  [处方记录]\n");
    foundAny = false;
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->patientID == patientID) {
            foundAny = true;
            printf("    %s | Day%u | 医生: %s | 总金额: %.2f | 状态: %s\n",
                   pr->prescID.c_str(), pr->prescDay,
                   getDocName(docHead, pr->doctorID).c_str(),
                   pr->totalAmount, getPrescStatusStr(pr->status));
            // Show medicine details
            PrescMedicineNode* pm = prescMedHead;
            bool medFound = false;
            while (pm) {
                if (pm->prescID == pr->prescID) {
                    if (!medFound) {
                        printf("      药品明细:\n");
                        medFound = true;
                    }
                    MedicineNode* med = findMedicineByID(medHead, pm->medID);
                    printf("        %s (%s) | 数量: %d | 单价: %.2f\n",
                           pm->medID.c_str(),
                           med ? med->tradeName.c_str() : "?",
                           pm->quantity, pm->unitPrice);
                }
                pm = pm->next;
            }
        }
        pr = pr->next;
    }
    if (!foundAny) printf("    (无)\n");

    // --- Hospitalizations ---
    printf("\n  [住院记录]\n");
    foundAny = false;
    HospitalizationNode* h = hospHead;
    while (h) {
        if (h->patientID == patientID) {
            foundAny = true;
            WardNode* ward = findWardByID(wardHead, h->wardID);
            printf("    %s | 病房: %s(%s) | 床位: %d | 入院: Day%u",
                   h->hospID.c_str(),
                   h->wardID.c_str(),
                   ward ? getWardTypeName(ward->type) : "?",
                   h->bedNo + 1, h->admitDay);
            if (h->dischargeDay > 0) {
                printf(" | 出院: Day%u", h->dischargeDay);
            } else {
                printf(" | 在院中");
            }
            printf(" | 押金: %.2f | 状态: %s\n", h->deposit, getHospStatusStr(h->status));
        }
        h = h->next;
    }
    if (!foundAny) printf("    (无)\n");

    printLine(70);
}

// ==================== 1. Drug Query (药品查询) ====================
struct MedDisplayInfo {
    std::string medID;
    std::string tradeName;
    std::string genericName;
    std::string spec;
    int stock;
    int consumed;
};

static void reportDrugQuery(MedicineNode* medHead, DeptMedicineNode* deptMedHead, DepartmentNode* deptHead) {
    printTitle("药品查询");
    printf("  请输入科室ID (输入 \"all\" 查看全部): ");
    std::string deptID = readString("", 20);

    bool showAll = (deptID == "all" || deptID == "ALL" || deptID == "All");
    if (!showAll && !findDepartmentByID(deptHead, deptID)) {
        printf("  科室 %s 不存在。\n", deptID.c_str());
        pauseScreen();
        return;
    }

    // Collect filtered medicines
    std::vector<MedDisplayInfo> meds;
    MedicineNode* m = medHead;
    while (m) {
        if (showAll) {
            MedDisplayInfo info;
            info.medID = m->medID;
            info.tradeName = m->tradeName;
            info.genericName = m->genericName;
            info.spec = m->spec;
            info.stock = m->stock;
            info.consumed = m->consumed;
            meds.push_back(info);
        } else {
            // Check if this medicine is linked to the department
            if (findDeptMedicine(deptMedHead, deptID, m->medID)) {
                MedDisplayInfo info;
                info.medID = m->medID;
                info.tradeName = m->tradeName;
                info.genericName = m->genericName;
                info.spec = m->spec;
                info.stock = m->stock;
                info.consumed = m->consumed;
                meds.push_back(info);
            }
        }
        m = m->next;
    }

    if (meds.empty()) {
        printf("  没有找到药品记录。\n");
        pauseScreen();
        return;
    }

    // Sorting menu
    printf("\n  排序方式:\n");
    printf("    1. 按库存升序\n");
    printf("    2. 按库存降序\n");
    printf("    3. 按消耗量升序\n");
    printf("    4. 按消耗量降序\n");
    printf("    5. 按名称字母序\n");
    printf("    6. 不排序\n");
    int sortChoice = readIntRange("  请选择: ", 1, 6);

    switch (sortChoice) {
        case 1:
            std::sort(meds.begin(), meds.end(), [](const MedDisplayInfo& a, const MedDisplayInfo& b) {
                return a.stock < b.stock;
            });
            break;
        case 2:
            std::sort(meds.begin(), meds.end(), [](const MedDisplayInfo& a, const MedDisplayInfo& b) {
                return a.stock > b.stock;
            });
            break;
        case 3:
            std::sort(meds.begin(), meds.end(), [](const MedDisplayInfo& a, const MedDisplayInfo& b) {
                return a.consumed < b.consumed;
            });
            break;
        case 4:
            std::sort(meds.begin(), meds.end(), [](const MedDisplayInfo& a, const MedDisplayInfo& b) {
                return a.consumed > b.consumed;
            });
            break;
        case 5:
            std::sort(meds.begin(), meds.end(), [](const MedDisplayInfo& a, const MedDisplayInfo& b) {
                return a.tradeName < b.tradeName;
            });
            break;
        case 6:
        default:
            break;
    }

    // Optional: search by name or ID
    printf("\n  搜索过滤 (输入 \".\" 跳过): ");
    std::string keyword = readString("", 50);
    bool doFilter = (keyword != ".");

    // Display
    printf("\n");
    if (!showAll) {
        printf("  科室: %s\n", getDeptName(deptHead, deptID).c_str());
    } else {
        printf("  全部药品\n");
    }
    printLine(90);
    printf("  %-10s %-15s %-15s %-10s %8s %8s\n",
           "药品ID", "商品名", "通用名", "规格", "库存", "消耗量");
    printLine(90);

    int count = 0;
    for (size_t i = 0; i < meds.size(); i++) {
        const MedDisplayInfo& info = meds[i];
        if (doFilter) {
            // Case-insensitive search in ID and tradeName
            std::string lowerID = info.medID;
            std::string lowerName = info.tradeName;
            std::string lowerKW = keyword;
            for (size_t j = 0; j < lowerID.size(); j++) lowerID[j] = tolower(lowerID[j]);
            for (size_t j = 0; j < lowerName.size(); j++) lowerName[j] = tolower(lowerName[j]);
            for (size_t j = 0; j < lowerKW.size(); j++) lowerKW[j] = tolower(lowerKW[j]);
            if (lowerID.find(lowerKW) == std::string::npos &&
                lowerName.find(lowerKW) == std::string::npos) {
                continue;
            }
        }
        printf("  %-10s %-15s %-15s %-10s %8d %8d\n",
               info.medID.c_str(), info.tradeName.c_str(), info.genericName.c_str(),
               info.spec.c_str(), info.stock, info.consumed);
        count++;
    }

    if (count == 0) {
        printf("  (无匹配结果)\n");
    } else {
        printLine(90);
        printf("  共 %d 条记录\n", count);
    }
    pauseScreen();
}

// ==================== 2. Doctor Info Query (医生信息查询) ====================
static void reportDoctorInfo(DoctorNode* doctorHead, DepartmentNode* deptHead) {
    printTitle("医生信息查询");
    printf("  请输入科室ID (输入 \"all\" 查看全部): ");
    std::string deptID = readString("", 20);

    bool showAll = (deptID == "all" || deptID == "ALL" || deptID == "All");
    if (!showAll && !findDepartmentByID(deptHead, deptID)) {
        printf("  科室 %s 不存在。\n", deptID.c_str());
        pauseScreen();
        return;
    }

    // Collect doctors
    std::vector<DoctorNode*> docs;
    DoctorNode* d = doctorHead;
    while (d) {
        if (showAll || d->departmentID == deptID) {
            docs.push_back(d);
        }
        d = d->next;
    }

    if (docs.empty()) {
        printf("  没有找到医生记录。\n");
        pauseScreen();
        return;
    }

    // Sort by doctorID
    std::sort(docs.begin(), docs.end(), [](DoctorNode* a, DoctorNode* b) {
        return a->doctorID < b->doctorID;
    });

    printf("\n");
    if (!showAll) {
        printf("  科室: %s\n", getDeptName(deptHead, deptID).c_str());
    } else {
        printf("  全部医生\n");
    }
    printLine(80);
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "医生ID", "姓名", "职称", "科室", "工作日");
    printLine(80);

    for (size_t i = 0; i < docs.size(); i++) {
        DoctorNode* doc = docs[i];
        printf("  %-10s %-12s %-12s %-15s ",
               doc->doctorID.c_str(), doc->name.c_str(),
               getLevelName(doc->level),
               getDeptName(deptHead, doc->departmentID).c_str());
        // Print work days
        for (int w = 0; w < doc->workDayCount; w++) {
            int day = doc->workDays[w];
            if (day >= 1 && day <= 7) {
                printf("%s ", WEEKDAY_NAMES[day - 1]);
            } else {
                printf("%d ", day);
            }
        }
        printf("\n");
    }
    printLine(80);
    printf("  共 %zu 位医生\n", docs.size());
    pauseScreen();
}

// ==================== 3. Ward Bed Occupancy (病房床位占用情况) ====================
static void reportWardBeds(WardNode* wardHead, HospitalizationNode* hospHead, PatientNode* patientHead, DepartmentNode* deptHead) {
    printTitle("病房床位占用情况");
    printf("  请输入科室ID: ");
    std::string deptID = readString("", 20);

    if (!findDepartmentByID(deptHead, deptID)) {
        printf("  科室 %s 不存在。\n", deptID.c_str());
        pauseScreen();
        return;
    }

    printf("\n  科室: %s\n", getDeptName(deptHead, deptID).c_str());
    printLine(70);

    bool foundWard = false;
    WardNode* w = wardHead;
    while (w) {
        if (w->departmentID == deptID) {
            foundWard = true;
            int occupied = 0;
            int freeCnt = 0;
            // Count occupied and free beds
            for (int i = 0; i < w->bedCount; i++) {
                if (w->bedStatus[i].empty()) freeCnt++; else occupied++;
            }

            printf("\n  病房: %s | 类型: %s | 总床位: %d | 已占用: %d | 空闲: %d\n",
                   w->wardID.c_str(), getWardTypeName(w->type),
                   w->bedCount, occupied, freeCnt);
            printLine(70);
            printf("  %-8s %-15s %s\n", "床号", "状态", "患者");

            for (int i = 0; i < w->bedCount; i++) {
                if (w->bedStatus[i].empty()) {
                    printf("  %-8d %-15s %s\n", i, "空闲", "-");
                } else {
                    PatientNode* pat = findPatientByID(patientHead, w->bedStatus[i]);
                    printf("  %-8d %-15s %s (%s)\n", i, "占用",
                           w->bedStatus[i].c_str(),
                           pat ? pat->name.c_str() : "?");
                }
            }
        }
        w = w->next;
    }

    if (!foundWard) {
        printf("  该科室没有病房。\n");
    }
    pauseScreen();
}

// ==================== 4. Patient Records Query (患者就诊记录查询) ====================
static void reportPatientRecords(
    PatientNode* patientHead, RegistrationNode* regHead, ConsultationNode* consultHead,
    ExaminationNode* examHead, PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead,
    MedicineNode* medHead, WardNode* wardHead, HospitalizationNode* hospHead, DoctorNode* docHead)
{
    printTitle("患者就诊记录查询");
    printf("  请输入患者ID (输入 \"all\" 查看全部): ");
    std::string patID = readString("", 20);

    if (patID == "all" || patID == "ALL" || patID == "All") {
        // Show all patients
        PatientNode* p = patientHead;
        bool found = false;
        while (p) {
            displayPatientRecords(p->patientID, patientHead, regHead, consultHead,
                                  examHead, prescHead, prescMedHead, medHead,
                                  wardHead, hospHead, docHead, deptHead);
            found = true;
            p = p->next;
        }
        if (!found) {
            printf("  没有患者记录。\n");
        }
    } else {
        // Single patient
        PatientNode* pat = findPatientByID(patientHead, patID);
        if (!pat) {
            printf("  患者 %s 不存在。\n", patID.c_str());
            pauseScreen();
            return;
        }
        displayPatientRecords(patID, patientHead, regHead, consultHead,
                              examHead, prescHead, prescMedHead, medHead,
                              wardHead, hospHead, docHead, deptHead);
    }
    pauseScreen();
}

// ==================== 5. Hospital Fund Statistics (医院资金统计) ====================
static void reportFundStats(
    RegistrationNode* regHead, PrescriptionNode* prescHead,
    HospitalizationNode* hospHead)
{
    printTitle("医院资金统计");
    printf("  当前医院资金余额: %.2f\n\n", money);

    // 1. Registration fees: all registrations (any status) x 20
    double regFees = 0.0;
    int regCount = 0;
    RegistrationNode* r = regHead;
    while (r) {
        regFees += REGISTRATION_FEE;
        regCount++;
        r = r->next;
    }
    regFees = round2(regFees);

    // 2. Paid prescription amounts
    double prescIncome = 0.0;
    int prescCount = 0;
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->status == 2) {  // STATUS_PAID
            prescIncome += pr->totalAmount;
            prescCount++;
        }
        pr = pr->next;
    }
    prescIncome = round2(prescIncome);

    // 3. Hospitalization fees
    double hospIncome = 0.0;
    int hospCount = 0;
    HospitalizationNode* h = hospHead;
    while (h) {
        // Calculate days stayed for each hospitalization
        unsigned int endDay = (h->dischargeDay > 0) ? h->dischargeDay : weekday;
        if (endDay > h->admitDay) {
            unsigned int days = endDay - h->admitDay;
            hospIncome += days * HOSPITAL_DAILY_FEE;
            hospCount++;
        }
        h = h->next;
    }
    hospIncome = round2(hospIncome);

    double total = round2(regFees + prescIncome + hospIncome);

    printLine(50);
    printf("  资金构成 (基于现有记录):\n\n");
    printf("  1. 挂号费收入:\n");
    printf("     挂号笔数: %d | 单价: %.2f | 合计: %.2f\n", regCount, REGISTRATION_FEE, regFees);
    printf("\n  2. 药品销售收入 (已支付处方):\n");
    printf("     处方笔数: %d | 合计: %.2f\n", prescCount, prescIncome);
    printf("\n  3. 住院费收入:\n");
    printf("     住院人次: %d | 单价: %.2f/天 | 合计: %.2f\n", hospCount, HOSPITAL_DAILY_FEE, hospIncome);
    printLine(50);
    printf("  各项收入合计: %.2f\n", total);
    printf("  当前资金余额: %.2f (注: 实际资金通过资金流管理，此处为记录统计近似值)\n", money);
    pauseScreen();
}

// ==================== 6. Doctor Workload Statistics (医生工作量统计) ====================
struct DoctorWorkload {
    std::string doctorID;
    std::string name;
    int consultCount;
    int prescCount;
    int total;
};

static void reportDoctorWorkload(DoctorNode* doctorHead, DepartmentNode* deptHead,
    ConsultationNode* consultHead, PrescriptionNode* prescHead)
{
    printTitle("医生工作量统计");

    // Input time range
    int startDay = readIntRange("  请输入起始天数: ", 0, 999999);
    int endDay = readIntRange("  请输入结束天数: ", startDay, 999999);

    // For each doctor, count consultations and prescriptions in range
    std::vector<DoctorWorkload> workloads;
    DoctorNode* d = doctorHead;
    while (d) {
        int consultCnt = 0;
        ConsultationNode* c = consultHead;
        while (c) {
            if (c->doctorID == d->doctorID &&
                (int)c->consultDay >= startDay && (int)c->consultDay <= endDay) {
                consultCnt++;
            }
            c = c->next;
        }

        int prescCnt = 0;
        PrescriptionNode* pr = prescHead;
        while (pr) {
            if (pr->doctorID == d->doctorID &&
                (int)pr->prescDay >= startDay && (int)pr->prescDay <= endDay) {
                prescCnt++;
            }
            pr = pr->next;
        }

        DoctorWorkload wl;
        wl.doctorID = d->doctorID;
        wl.name = d->name;
        wl.consultCount = consultCnt;
        wl.prescCount = prescCnt;
        wl.total = consultCnt + prescCnt;
        // Always add, including zeros (though we could skip zeros)
        workloads.push_back(wl);
        d = d->next;
    }

    // Sort by total workload descending
    std::sort(workloads.begin(), workloads.end(), [](const DoctorWorkload& a, const DoctorWorkload& b) {
        return a.total > b.total;
    });

    printf("\n  统计区间: Day%d ~ Day%d\n", startDay, endDay);
    printLine(80);
    printf("  %-4s %-10s %-12s %-15s %12s %12s %10s\n",
           "排名", "医生ID", "姓名", "科室", "诊疗次数", "处方次数", "合计");
    printLine(80);

    for (size_t i = 0; i < workloads.size(); i++) {
        const DoctorWorkload& wl = workloads[i];
        DoctorNode* doc = findDoctorByID(doctorHead, wl.doctorID);
        printf("  %-4zu %-10s %-12s %-15s %12d %12d %10d\n",
               i + 1, wl.doctorID.c_str(), wl.name.c_str(),
               doc ? getDeptName(deptHead, doc->departmentID).c_str() : "-",
               wl.consultCount, wl.prescCount, wl.total);
    }
    printLine(80);
    printf("  共 %zu 位医生\n", workloads.size());
    pauseScreen();
}

// ==================== 7. Total Revenue Statistics (营业总额统计) ====================
static void reportTotalRevenue(
    RegistrationNode* regHead, PrescriptionNode* prescHead, HospitalizationNode* hospHead)
{
    printTitle("营业总额统计");

    int startDay = readIntRange("  请输入起始天数: ", 0, 999999);
    int endDay = readIntRange("  请输入结束天数: ", startDay, 999999);

    // 1. Registration fees in range
    double regFees = 0.0;
    int regCount = 0;
    RegistrationNode* r = regHead;
    while (r) {
        if ((int)r->regDay >= startDay && (int)r->regDay <= endDay) {
            regFees += REGISTRATION_FEE;
            regCount++;
        }
        r = r->next;
    }
    regFees = round2(regFees);

    // 2. Paid prescriptions in range
    double prescIncome = 0.0;
    int prescCount = 0;
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->status == 2 && (int)pr->prescDay >= startDay && (int)pr->prescDay <= endDay) {
            prescIncome += pr->totalAmount;
            prescCount++;
        }
        pr = pr->next;
    }
    prescIncome = round2(prescIncome);

    // 3. Hospitalization fees in range
    // For each hospitalization, calculate days that fall in the range
    double hospIncome = 0.0;
    int hospDays = 0;
    HospitalizationNode* h = hospHead;
    while (h) {
        unsigned int recStart = h->admitDay;
        unsigned int recEnd = (h->dischargeDay > 0) ? h->dischargeDay : weekday;
        // Intersection with [startDay, endDay]
        unsigned int overlapStart = (recStart > (unsigned int)startDay) ? recStart : (unsigned int)startDay;
        unsigned int overlapEnd = (recEnd < (unsigned int)endDay) ? recEnd : (unsigned int)endDay;
        if (overlapStart <= overlapEnd) {
            unsigned int days = overlapEnd - overlapStart;
            // Note: we charge from the start day, so include both the start and end
            // Actually, if admit on Day5 and discharge on Day7, that's 2 days (Day5-Day6, or Day5-Day7)
            // Let's use: days = overlapEnd - overlapStart (number of nights stayed)
            // But more standard: if start <= end, then days of stay = end - start
            if (days > 0) {
                hospIncome += days * HOSPITAL_DAILY_FEE;
                hospDays += days;
            }
        }
        h = h->next;
    }
    hospIncome = round2(hospIncome);

    double total = round2(regFees + prescIncome + hospIncome);

    printf("\n  统计区间: Day%d ~ Day%d\n", startDay, endDay);
    printLine(50);
    printf("  收入明细:\n\n");
    printf("  1. 挂号费: %d 笔 x %.2f = %.2f\n", regCount, REGISTRATION_FEE, regFees);
    printf("  2. 处方收入(已支付): %d 笔 = %.2f\n", prescCount, prescIncome);
    printf("  3. 住院费: %d 天 x %.2f = %.2f\n", hospDays, HOSPITAL_DAILY_FEE, hospIncome);
    printLine(50);
    printf("  营业总额: %.2f\n", total);
    pauseScreen();
}

// ==================== adminReportMenu ====================
void adminReportMenu(
    DepartmentNode* deptHead, DoctorNode* doctorHead, PatientNode* patientHead,
    RegistrationNode* regHead, ConsultationNode* consultHead, ExaminationNode* examHead,
    PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead, MedicineNode* medHead,
    DeptMedicineNode* deptMedHead, WardNode* wardHead, HospitalizationNode* hospHead)
{
    while (true) {
        printTitle("管理员报表中心");
        printf("  1. 药品查询\n");
        printf("  2. 医生信息查询\n");
        printf("  3. 病房床位占用情况\n");
        printf("  4. 患者就诊记录查询\n");
        printf("  5. 医院资金统计\n");
        printf("  6. 医生工作量统计\n");
        printf("  7. 营业总额统计\n");
        printf("  0. 返回\n");
        int choice = readIntRange("  请选择: ", 0, 7);

        switch (choice) {
            case 1:
                reportDrugQuery(medHead, deptMedHead, deptHead);
                break;
            case 2:
                reportDoctorInfo(doctorHead, deptHead);
                break;
            case 3:
                reportWardBeds(wardHead, hospHead, patientHead, deptHead);
                break;
            case 4:
                reportPatientRecords(patientHead, regHead, consultHead, examHead,
                                     prescHead, prescMedHead, medHead, wardHead, hospHead, doctorHead);
                break;
            case 5:
                reportFundStats(regHead, prescHead, hospHead);
                break;
            case 6:
                reportDoctorWorkload(doctorHead, deptHead, consultHead, prescHead);
                break;
            case 7:
                reportTotalRevenue(regHead, prescHead, hospHead);
                break;
            case 0:
                return;
        }
    }
}

// ==================== Doctor Report - My Outpatients ====================
static void doctorReportOutpatients(const std::string& doctorID,
    DoctorNode* docHead, PatientNode* patientHead, DepartmentNode* deptHead,
    RegistrationNode* regHead, ConsultationNode* consultHead)
{
    printTitle("我的门诊患者及诊疗记录");

    int startDay = readIntRange("  请输入起始天数: ", 0, 999999);
    int endDay = readIntRange("  请输入结束天数: ", startDay, 999999);

    // Find all registrations for this doctor in range
    std::vector<RegistrationNode*> regs;
    RegistrationNode* r = regHead;
    while (r) {
        if (r->doctorID == doctorID &&
            (int)r->regDay >= startDay && (int)r->regDay <= endDay) {
            regs.push_back(r);
        }
        r = r->next;
    }

    printf("\n  医生: %s\n", getDocName(docHead, doctorID).c_str());
    printf("  统计区间: Day%d ~ Day%d\n", startDay, endDay);
    printLine(80);

    if (regs.empty()) {
        printf("  该时间段内没有门诊记录。\n");
        pauseScreen();
        return;
    }

    int count = 0;
    for (size_t i = 0; i < regs.size(); i++) {
        RegistrationNode* reg = regs[i];
        PatientNode* pat = findPatientByID(patientHead, reg->patientID);
        printf("  挂号: %s | Day%u | 患者: %s (%s) | 状态: %s\n",
               reg->regID.c_str(), reg->regDay,
               reg->patientID.c_str(),
               pat ? pat->name.c_str() : "?",
               getRegStatusStr(reg->status));

        // Find consultation for this registration
        ConsultationNode* c = consultHead;
        bool hasConsult = false;
        while (c) {
            if (c->regID == reg->regID) {
                hasConsult = true;
                printf("    诊疗: %s | 主诉: %s\n", c->consultID.c_str(), c->complaint.c_str());
                printf("    诊断: %s\n", c->diagnosis.c_str());
            }
            c = c->next;
        }
        if (!hasConsult) {
            printf("    (暂无诊疗记录)\n");
        }
        count++;
    }

    printLine(80);
    printf("  共 %d 位门诊患者\n", count);
    pauseScreen();
}

// ==================== Doctor Report - My Inpatients ====================
static void doctorReportInpatients(const std::string& doctorID,
    DoctorNode* docHead, PatientNode* patientHead, WardNode* wardHead, HospitalizationNode* hospHead)
{
    printTitle("我负责的住院患者情况");
    printf("  医生: %s\n\n", getDocName(docHead, doctorID).c_str());

    // Find all hospitalizations where this doctorID is in doctorIDs list and status=0
    HospitalizationNode* h = hospHead;
    bool foundAny = false;
    int count = 0;

    while (h) {
        if (h->status != 0) {
            h = h->next;
            continue;
        }
        // Check if this doctor is responsible
        bool isResponsible = false;
        for (int i = 0; i < h->doctorCount; i++) {
            if (h->doctorIDs[i] == doctorID) {
                isResponsible = true;
                break;
            }
        }
        if (!isResponsible) {
            h = h->next;
            continue;
        }

        foundAny = true;
        PatientNode* pat = findPatientByID(patientHead, h->patientID);
        WardNode* ward = findWardByID(wardHead, h->wardID);
        unsigned int daysStayed = weekday - h->admitDay;

        printLine(80);
        printf("  住院号: %s\n", h->hospID.c_str());
        printf("  患者: %s | 姓名: %s | 年龄: %d\n",
               h->patientID.c_str(),
               pat ? pat->name.c_str() : "?",
               pat ? pat->age : 0);
        printf("  病房: %s (%s) | 床位: %d\n",
               h->wardID.c_str(),
               ward ? getWardTypeName(ward->type) : "?",
               h->bedNo + 1);
        printf("  入院日期: Day%u | 已住天数: %u | 押金余额: %.2f\n",
               h->admitDay, daysStayed, h->deposit);
        count++;
        h = h->next;
    }

    if (!foundAny) {
        printf("  当前没有负责的住院患者。\n");
    } else {
        printLine(80);
        printf("  共 %d 位住院患者\n", count);
    }
    pauseScreen();
}

// ==================== Doctor Report - My Prescriptions ====================
static void doctorReportPrescriptions(const std::string& doctorID,
    DoctorNode* docHead, PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead,
    MedicineNode* medHead, PatientNode* patientHead)
{
    printTitle("我开具的处方统计");
    printf("  医生: %s\n\n", getDocName(docHead, doctorID).c_str());

    printf("  是否按日期范围筛选? (1=是, 0=全部): ");
    int filterChoice = readIntRange("", 0, 1);

    int startDay = 0, endDay = 0;
    if (filterChoice == 1) {
        startDay = readIntRange("  请输入起始天数: ", 0, 999999);
        endDay = readIntRange("  请输入结束天数: ", startDay, 999999);
    }

    // Collect all prescriptions for this doctor
    int totalCount = 0;
    double totalAmount = 0.0;
    int paidCount = 0;
    int unpaidCount = 0;
    int cancelledCount = 0;

    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->doctorID != doctorID) {
            pr = pr->next;
            continue;
        }
        if (filterChoice == 1) {
            if ((int)pr->prescDay < startDay || (int)pr->prescDay > endDay) {
                pr = pr->next;
                continue;
            }
        }

        totalCount++;
        totalAmount += pr->totalAmount;

        if (pr->status == 2) paidCount++;
        else if (pr->status == 1) cancelledCount++;
        else unpaidCount++;

        // Display details
        PatientNode* pat = findPatientByID(patientHead, pr->patientID);
        printf("  %s | Day%u | 患者: %s | 金额: %.2f | 状态: %s\n",
               pr->prescID.c_str(), pr->prescDay,
               pat ? pat->name.c_str() : pr->patientID.c_str(),
               pr->totalAmount, getPrescStatusStr(pr->status));

        // Show medicine details
        PrescMedicineNode* pm = prescMedHead;
        while (pm) {
            if (pm->prescID == pr->prescID) {
                MedicineNode* med = findMedicineByID(medHead, pm->medID);
                printf("    -> %s (%s) x%d @%.2f\n",
                       pm->medID.c_str(),
                       med ? med->tradeName.c_str() : "?",
                       pm->quantity, pm->unitPrice);
            }
            pm = pm->next;
        }

        pr = pr->next;
    }

    totalAmount = round2(totalAmount);

    printLine(70);
    printf("  统计汇总:\n");
    if (filterChoice == 1) {
        printf("  日期范围: Day%d ~ Day%d\n", startDay, endDay);
    } else {
        printf("  日期范围: 全部\n");
    }
    printf("  处方总数: %d\n", totalCount);
    printf("    已支付: %d\n", paidCount);
    printf("    未支付: %d\n", unpaidCount);
    printf("    已取消: %d\n", cancelledCount);
    printf("  总金额: %.2f\n", totalAmount);
    pauseScreen();
}

// ==================== doctorReportMenu ====================
void doctorReportMenu(const std::string& doctorID,
    DepartmentNode* deptHead, DoctorNode* doctorHead, PatientNode* patientHead,
    RegistrationNode* regHead, ConsultationNode* consultHead, ExaminationNode* examHead,
    PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead, MedicineNode* medHead,
    WardNode* wardHead, HospitalizationNode* hospHead)
{
    while (true) {
        printTitle("医生报表中心");
        printf("  1. 我的门诊患者及诊疗记录\n");
        printf("  2. 我负责的住院患者情况\n");
        printf("  3. 我开具的处方统计\n");
        printf("  0. 返回\n");
        int choice = readIntRange("  请选择: ", 0, 3);

        switch (choice) {
            case 1:
                doctorReportOutpatients(doctorID, doctorHead, patientHead, deptHead,
                                        regHead, consultHead);
                break;
            case 2:
                doctorReportInpatients(doctorID, doctorHead, patientHead, wardHead, hospHead);
                break;
            case 3:
                doctorReportPrescriptions(doctorID, doctorHead, prescHead, prescMedHead,
                                          medHead, patientHead);
                break;
            case 0:
                return;
        }
    }
}

// ==================== patientReportMenu ====================
void patientReportMenu(const std::string& patientID,
    PatientNode* patientHead, RegistrationNode* regHead, ConsultationNode* consultHead,
    ExaminationNode* examHead, PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead,
    MedicineNode* medHead, WardNode* wardHead, HospitalizationNode* hospHead, DoctorNode* doctorHead)
{
    PatientNode* pat = findPatientByID(patientHead, patientID);
    if (!pat) {
        printTitle("患者个人报表");
        printf("  患者 %s 不存在。\n", patientID.c_str());
        pauseScreen();
        return;
    }

    while (true) {
        printTitle("患者个人报表");
        printf("  患者: %s (%s)\n\n", pat->patientID.c_str(), pat->name.c_str());

        // Show quick summary
        // Count registrations
        int regCnt = 0;
        RegistrationNode* r = regHead;
        while (r) { if (r->patientID == patientID) regCnt++; r = r->next; }

        // Count consultations
        int consultCnt = 0;
        ConsultationNode* c = consultHead;
        while (c) { if (c->patientID == patientID) consultCnt++; c = c->next; }

        // Count examinations
        int examCnt = 0;
        ExaminationNode* e = examHead;
        while (e) { if (e->patientID == patientID) examCnt++; e = e->next; }

        // Count prescriptions
        int prescCnt = 0;
        PrescriptionNode* pr = prescHead;
        while (pr) { if (pr->patientID == patientID) prescCnt++; pr = pr->next; }

        printf("  就诊摘要:\n");
        printf("    挂号次数: %d\n", regCnt);
        printf("    诊疗次数: %d\n", consultCnt);
        printf("    检查次数: %d\n", examCnt);
        printf("    处方次数: %d\n", prescCnt);

        // Check if inpatient
        HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
        if (hosp) {
            WardNode* ward = findWardByID(wardHead, hosp->wardID);
            unsigned int daysStayed = weekday - hosp->admitDay;
            printf("\n  当前住院:\n");
            printf("    住院号: %s\n", hosp->hospID.c_str());
            printf("    病房: %s (%s) 床位: %d\n",
                   hosp->wardID.c_str(),
                   ward ? getWardTypeName(ward->type) : "?",
                   hosp->bedNo + 1);
            printf("    入院: Day%u | 已住: %u天 | 押金: %.2f\n",
                   hosp->admitDay, daysStayed, hosp->deposit);
            printf("    负责医生: ");
            for (int i = 0; i < hosp->doctorCount; i++) {
                printf("%s(%s) ", hosp->doctorIDs[i].c_str(),
                       getDocName(doctorHead, hosp->doctorIDs[i]).c_str());
            }
            printf("\n");
        } else {
            printf("\n  当前未住院\n");
        }

        printf("\n  1. 查看完整就诊记录\n");
        printf("  0. 返回\n");
        int choice = readIntRange("  请选择: ", 0, 1);

        switch (choice) {
            case 1:
                displayPatientRecords(patientID, patientHead, regHead, consultHead,
                                      examHead, prescHead, prescMedHead, medHead,
                                      wardHead, hospHead, doctorHead, deptHead);
                pauseScreen();
                break;
            case 0:
                return;
        }
    }
}
