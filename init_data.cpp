#include "init_data.h"
#include <cstring>

// Helper to create a department node
static DepartmentNode* makeDept(const std::string& name, const std::string& desc) {
    DepartmentNode* d = new DepartmentNode();
    d->deptID = generateDeptID();
    d->name = name;
    d->description = desc;
    return d;
}

// Helper to create a doctor node
static DoctorNode* makeDoctor(const std::string& name, int level, const std::string& deptID,
                               int* workDays, int wdCount) {
    DoctorNode* d = new DoctorNode();
    d->doctorID = generateDoctorID();
    d->name = name;
    d->level = level;
    d->departmentID = deptID;
    d->workDayCount = wdCount;
    for (int i = 0; i < wdCount; i++) d->workDays[i] = workDays[i];
    return d;
}

// Helper to create a patient node
static PatientNode* makePatient(const std::string& name, int age, const std::string& contact, int type) {
    PatientNode* p = new PatientNode();
    p->patientID = generatePatientID();
    p->name = name;
    p->age = age;
    p->contact = contact;
    p->type = type;
    return p;
}

// Helper to create a medicine node
static MedicineNode* makeMed(const std::string& tradeName, const std::string& genericName,
                              const std::string& alias, const std::string& spec, int stock) {
    MedicineNode* m = new MedicineNode();
    m->medID = generateMedID();
    m->tradeName = tradeName;
    m->genericName = genericName;
    m->alias = alias;
    m->spec = spec;
    m->stock = stock;
    m->consumed = 0;
    return m;
}

// Helper to create a ward node
static WardNode* makeWard(int type, const std::string& deptID, int bedCount) {
    WardNode* w = new WardNode();
    w->wardID = generateWardID();
    w->type = type;
    w->departmentID = deptID;
    w->bedCount = (bedCount <= MAX_BEDS) ? bedCount : MAX_BEDS;
    return w;
}

void initDefaultData(
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
    HospitalizationNode*& hospHead,
    AdminNode*& adminHead) {

    // ==================== Administrators ====================
    AdminNode* admin = new AdminNode();
    admin->adminID = "admin";
    admin->password = "123456";
    insertAdminTail(adminHead, admin);

    AdminNode* admin2 = new AdminNode();
    admin2->adminID = "root";
    admin2->password = "root123";
    insertAdminTail(adminHead, admin2);

    // ==================== Departments ====================
    DepartmentNode* deptIM = makeDept("内科", "诊治内科疾病，包含心血管、呼吸、消化等");
    DepartmentNode* deptSG = makeDept("外科", "诊治外科疾病，包含普外、骨外、神经外等");
    DepartmentNode* deptPD = makeDept("儿科", "诊治儿童疾病及儿童保健");
    DepartmentNode* deptOB = makeDept("妇产科", "诊治妇产科疾病及孕产妇保健");
    DepartmentNode* deptEM = makeDept("急诊科", "处理急危重症患者的紧急救治");

    insertDepartmentTail(deptHead, deptIM);
    insertDepartmentTail(deptHead, deptSG);
    insertDepartmentTail(deptHead, deptPD);
    insertDepartmentTail(deptHead, deptOB);
    insertDepartmentTail(deptHead, deptEM);

    std::string idIM = deptIM->deptID;
    std::string idSG = deptSG->deptID;
    std::string idPD = deptPD->deptID;
    std::string idOB = deptOB->deptID;
    std::string idEM = deptEM->deptID;

    // ==================== Doctors (22 doctors across 5 departments) ====================
    // Internal Medicine - 5 doctors
    int wd135[] = {1, 3, 5}; int wd24[] = {2, 4}; int wd12345[] = {1, 2, 3, 4, 5};
    int wd146[] = {1, 4, 6}; int wd26[] = {2, 6}; int wd123456[] = {1, 2, 3, 4, 5, 6};
    int wd15[] = {1, 5}; int wd34[] = {3, 4};
    // IM doctors
    insertDoctorTail(doctorHead, makeDoctor("张伟明", 0, idIM, wd135, 3));
    insertDoctorTail(doctorHead, makeDoctor("李芳华", 1, idIM, wd24, 2));
    insertDoctorTail(doctorHead, makeDoctor("王建国", 2, idIM, wd12345, 5));
    insertDoctorTail(doctorHead, makeDoctor("陈晓燕", 2, idIM, wd146, 3));
    insertDoctorTail(doctorHead, makeDoctor("刘志强", 3, idIM, wd135, 3));
    // Surgery - 5 doctors
    insertDoctorTail(doctorHead, makeDoctor("赵大明", 0, idSG, wd12345, 5));
    insertDoctorTail(doctorHead, makeDoctor("孙丽萍", 1, idSG, wd26, 2));
    insertDoctorTail(doctorHead, makeDoctor("周建华", 2, idSG, wd135, 3));
    insertDoctorTail(doctorHead, makeDoctor("吴国强", 2, idSG, wd12345, 5));
    insertDoctorTail(doctorHead, makeDoctor("郑晓峰", 3, idSG, wd146, 3));
    // Pediatrics - 4 doctors
    insertDoctorTail(doctorHead, makeDoctor("冯小梅", 0, idPD, wd12345, 5));
    insertDoctorTail(doctorHead, makeDoctor("陈玉兰", 1, idPD, wd15, 2));
    insertDoctorTail(doctorHead, makeDoctor("褚明辉", 2, idPD, wd24, 2));
    insertDoctorTail(doctorHead, makeDoctor("卫思远", 3, idPD, wd135, 3));
    // OB/GYN - 4 doctors
    insertDoctorTail(doctorHead, makeDoctor("蒋美玲", 0, idOB, wd12345, 5));
    insertDoctorTail(doctorHead, makeDoctor("沈秋菊", 1, idOB, wd34, 2));
    insertDoctorTail(doctorHead, makeDoctor("韩秀丽", 2, idOB, wd26, 2));
    insertDoctorTail(doctorHead, makeDoctor("杨丽华", 3, idOB, wd135, 3));
    // Emergency - 4 doctors
    insertDoctorTail(doctorHead, makeDoctor("朱志刚", 0, idEM, wd123456, 6));
    insertDoctorTail(doctorHead, makeDoctor("秦海峰", 1, idEM, wd135, 3));
    insertDoctorTail(doctorHead, makeDoctor("尤建军", 2, idEM, wd24, 2));
    insertDoctorTail(doctorHead, makeDoctor("许文斌", 3, idEM, wd12345, 5));

    // ==================== Medicines (25 medicines) ====================
    insertMedicineTail(medHead, makeMed("阿莫西林胶囊", "阿莫西林", "阿莫仙", "0.25g*24粒", 500));
    insertMedicineTail(medHead, makeMed("头孢克洛缓释片", "头孢克洛", "希刻劳", "0.375g*6片", 300));
    insertMedicineTail(medHead, makeMed("布洛芬缓释胶囊", "布洛芬", "芬必得", "0.3g*20粒", 400));
    insertMedicineTail(medHead, makeMed("对乙酰氨基酚片", "对乙酰氨基酚", "扑热息痛", "0.5g*12片", 600));
    insertMedicineTail(medHead, makeMed("奥美拉唑肠溶胶囊", "奥美拉唑", "洛赛克", "20mg*14粒", 350));
    insertMedicineTail(medHead, makeMed("蒙脱石散", "蒙脱石", "思密达", "3g*10袋", 450));
    insertMedicineTail(medHead, makeMed("复方甘草片", "复方甘草", "", "100片/瓶", 200));
    insertMedicineTail(medHead, makeMed("硝苯地平缓释片", "硝苯地平", "心痛定", "30mg*7片", 250));
    insertMedicineTail(medHead, makeMed("氯沙坦钾片", "氯沙坦钾", "科素亚", "50mg*7片", 180));
    insertMedicineTail(medHead, makeMed("二甲双胍片", "二甲双胍", "格华止", "0.5g*20片", 320));
    insertMedicineTail(medHead, makeMed("胰岛素注射液", "胰岛素", "诺和灵", "300IU/3ml", 150));
    insertMedicineTail(medHead, makeMed("甲硝唑片", "甲硝唑", "灭滴灵", "0.2g*21片", 380));
    insertMedicineTail(medHead, makeMed("盐酸氨溴索片", "氨溴索", "沐舒坦", "30mg*20片", 280));
    insertMedicineTail(medHead, makeMed("氯雷他定片", "氯雷他定", "开瑞坦", "10mg*6片", 220));
    insertMedicineTail(medHead, makeMed("阿司匹林肠溶片", "阿司匹林", "拜阿司匹林", "100mg*30片", 400));
    insertMedicineTail(medHead, makeMed("注射用青霉素钠", "青霉素钠", "", "80万单位/瓶", 500));
    insertMedicineTail(medHead, makeMed("葡萄糖注射液", "葡萄糖", "", "5% 250ml", 1000));
    insertMedicineTail(medHead, makeMed("氯化钠注射液", "氯化钠", "生理盐水", "0.9% 250ml", 1200));
    insertMedicineTail(medHead, makeMed("维生素C注射液", "维生素C", "维C", "1g/5ml", 800));
    insertMedicineTail(medHead, makeMed("地塞米松磷酸钠注射液", "地塞米松", "氟美松", "5mg/1ml", 300));
    insertMedicineTail(medHead, makeMed("阿托伐他汀钙片", "阿托伐他汀", "立普妥", "20mg*7片", 200));
    insertMedicineTail(medHead, makeMed("硝酸甘油片", "硝酸甘油", "", "0.5mg*100片", 150));
    insertMedicineTail(medHead, makeMed("小儿氨酚黄那敏颗粒", "小儿氨酚黄那敏", "护彤", "125mg*12袋", 350));
    insertMedicineTail(medHead, makeMed("蒙脱石混悬液", "蒙脱石", "思密达混悬液", "3g/袋", 200));
    insertMedicineTail(medHead, makeMed("缩宫素注射液", "缩宫素", "催产素", "10IU/1ml", 100));

    // Map medIDs for department linking
    // med1=阿莫西林, med2=头孢克洛, med3=布洛芬, med4=对乙酰氨基酚, med5=奥美拉唑
    // med6=蒙脱石散, med7=复方甘草片, med8=硝苯地平, med9=氯沙坦钾, med10=二甲双胍
    // med11=胰岛素, med12=甲硝唑, med13=氨溴索, med14=氯雷他定, med15=阿司匹林
    // med16=青霉素, med17=葡萄糖, med18=氯化钠, med19=维C, med20=地塞米松
    // med21=阿托伐他汀, med22=硝酸甘油, med23=小儿氨酚, med24=蒙脱石混悬液, med25=缩宫素

    MedicineNode* medList[25];
    MedicineNode* mc = medHead;
    for (int i = 0; i < 25 && mc; i++) { medList[i] = mc; mc = mc->next; }

    // Department-Medicine links
    // Internal Medicine: 1,3,4,5,6,7,8,9,10,11,15,21,22
    int imMeds[] = {1,3,4,5,6,7,8,9,10,11,15,21,22};
    for (int i = 0; i < 13; i++) {
        DeptMedicineNode* dm = new DeptMedicineNode();
        dm->deptID = idIM;
        dm->medID = medList[imMeds[i]-1]->medID;
        insertDeptMedicineTail(deptMedHead, dm);
    }
    // Surgery: 1,2,3,4,12,13,15,16,17,18,19,20
    int sgMeds[] = {1,2,3,4,12,13,15,16,17,18,19,20};
    for (int i = 0; i < 12; i++) {
        DeptMedicineNode* dm = new DeptMedicineNode();
        dm->deptID = idSG;
        dm->medID = medList[sgMeds[i]-1]->medID;
        insertDeptMedicineTail(deptMedHead, dm);
    }
    // Pediatrics: 1,3,4,6,7,13,14,16,18,19,23,24
    int pdMeds[] = {1,3,4,6,7,13,14,16,18,19,23,24};
    for (int i = 0; i < 12; i++) {
        DeptMedicineNode* dm = new DeptMedicineNode();
        dm->deptID = idPD;
        dm->medID = medList[pdMeds[i]-1]->medID;
        insertDeptMedicineTail(deptMedHead, dm);
    }
    // OB/GYN: 1,2,3,4,12,13,17,18,19,20,25
    int obMeds[] = {1,2,3,4,12,13,17,18,19,20,25};
    for (int i = 0; i < 11; i++) {
        DeptMedicineNode* dm = new DeptMedicineNode();
        dm->deptID = idOB;
        dm->medID = medList[obMeds[i]-1]->medID;
        insertDeptMedicineTail(deptMedHead, dm);
    }
    // Emergency: 1,2,3,4,7,15,16,17,18,19,20,22
    int emMeds[] = {1,2,3,4,7,15,16,17,18,19,20,22};
    for (int i = 0; i < 12; i++) {
        DeptMedicineNode* dm = new DeptMedicineNode();
        dm->deptID = idEM;
        dm->medID = medList[emMeds[i]-1]->medID;
        insertDeptMedicineTail(deptMedHead, dm);
    }

    // ==================== Wards ====================
    // IM: 1 regular ward (30 beds)
    insertWardTail(wardHead, makeWard(WARD_REGULAR, idIM, 30));
    // SG: 1 special ward (15 beds) + 1 regular ward (30 beds)
    insertWardTail(wardHead, makeWard(WARD_SPECIAL, idSG, 15));
    insertWardTail(wardHead, makeWard(WARD_REGULAR, idSG, 30));
    // PD: 1 regular ward (30 beds)
    insertWardTail(wardHead, makeWard(WARD_REGULAR, idPD, 30));
    // OB: 1 special ward (20 beds) + 1 regular ward (20 beds)
    insertWardTail(wardHead, makeWard(WARD_SPECIAL, idOB, 20));
    insertWardTail(wardHead, makeWard(WARD_REGULAR, idOB, 20));
    // EM: 1 regular ward (10 beds) for observation
    insertWardTail(wardHead, makeWard(WARD_REGULAR, idEM, 10));

    // ==================== Patients ====================
    // Generate 100+ outpatients
    const char* outNames[] = {
        "王一明", "李二华", "张三强", "刘四梅", "陈五龙", "杨六凤", "赵七虎", "黄八妹",
        "周九川", "吴十全", "徐建国", "孙丽华", "马永强", "朱晓燕", "胡志明", "林美芳",
        "何国强", "郭海峰", "郑文斌", "梁晓东", "谢明辉", "韩建国", "唐丽珍", "冯思远",
        "董春华", "萧秋菊", "程玉兰", "曹冠杰", "袁雪峰", "邓秀丽", "许文龙", "傅天亮",
        "沈月华", "曾志伟", "彭晓琳", "吕明德", "苏慧敏", "卢建成", "蒋秀英", "蔡建平",
        "贾玉凤", "丁国强", "魏美玲", "薛志明", "叶海龙", "阎静怡", "余俊杰", "潘雪梅",
        "杜春明", "戴晓峰", "夏丽华", "钟思源", "汪明达", "田秀芳", "任建国", "姜丽萍",
        "范国强", "方晓燕", "石志刚", "姚海峰", "谭文斌", "廖慧敏", "邹建军", "熊丽丽",
        "金明辉", "陆秀丽", "郝建国", "白雪飞", "崔志成", "康丽华", "毛春生", "邱桂英",
        "秦海龙", "江月明", "史兴华", "顾晓琳", "侯建国", "邵丽珍", "孟永康", "龙曙光",
        "万宝国", "段秋芬", "雷正阳", "钱卫东", "汤明达", "尹秀兰", "易志强", "常建华",
        "武文鑫", "乔海燕", "贺国梁", "赖明辉", "龚建平", "文雪梅"
    };

    for (int i = 0; i < 90; i++) {
        PatientNode* p = makePatient(outNames[i],
            18 + (i * 7) % 70,  // age 18-87
            "1380000" + std::to_string(1000 + i),
            PATIENT_OUTPATIENT);
        insertPatientTail(patientHead, p);
    }

    // 35 inpatients
    const char* inNames[] = {
        "赵铁柱", "钱桂花", "孙大勇", "李素芳", "周永康", "吴美珍", "郑振华", "王瑞英",
        "冯志远", "陈桂香", "褚俊杰", "卫丽华", "蒋大国", "沈小梅", "韩永胜", "杨玉兰",
        "朱士宏", "秦美娟", "尤振邦", "许彩霞", "何建国", "吕桂兰", "施文博", "张春香",
        "孔德明", "曹秀莲", "严海生", "华玉凤", "金永发", "魏红梅", "陶志荣", "姜雅芬",
        "戚俊峰", "谢菊花", "邹文龙"
    };

    // Ward pointers for assigning inpatients
    WardNode* wardIM = findWardByID(wardHead, wardHead->wardID); // first ward (IM)
    WardNode* wardIM2 = wardIM; // no second one for IM, use SG
    WardNode* wardSG = wardIM->next; // second ward (SG special)
    WardNode* wardSG2 = wardSG->next; // third ward (SG regular)
    WardNode* wardPD = wardSG2->next; // fourth ward (PD)
    WardNode* wardOB = wardPD->next; // fifth ward (OB special)
    WardNode* wardOB2 = wardOB->next; // sixth ward (OB regular)

    for (int i = 0; i < 35; i++) {
        PatientNode* p = makePatient(inNames[i],
            25 + (i * 11) % 65,
            "1390000" + std::to_string(2000 + i),
            PATIENT_INPATIENT);
        insertPatientTail(patientHead, p);
    }

    // ==================== Hospitalization Records (for 30 inpatients) ====================
    // Assign each inpatient to a ward bed
    PatientNode* pCur = patientHead;
    while (pCur) {
        if (pCur->type == PATIENT_INPATIENT) {
            HospitalizationNode* h = new HospitalizationNode();
            h->hospID = generateHospID(0);
            h->patientID = pCur->patientID;
            h->admitDay = 0;
            h->dischargeDay = 0;
            h->deposit = 3000.0 + (rand() % 10000);
            h->deposit = round2(h->deposit);
            h->status = 0;

            // Assign to a ward cyclically and try to allocate a bed
            WardNode* targetWards[] = {wardIM, wardSG, wardSG2, wardPD, wardOB, wardOB2};
            for (int wi = 0; wi < 6; wi++) {
                WardNode* wptr = targetWards[wi];
                int bedNo = getAllocateBed(wptr, pCur->patientID);
                if (bedNo >= 0) {
                    h->wardID = wptr->wardID;
                    h->bedNo = bedNo;
                    pCur->bedID = wptr->wardID + "-" + std::to_string(bedNo);
                    break;
                }
            }
            if (h->wardID.empty()) {
                delete h;
                pCur = pCur->next;
                continue;
            }

            // Assign a doctor from same department as ward
            WardNode* assignmentWard = findWardByID(wardHead, h->wardID);
            if (assignmentWard && !assignmentWard->departmentID.empty()) {
                DoctorNode* dCur = doctorHead;
                int docIdx = 0;
                while (dCur) {
                    if (dCur->departmentID == assignmentWard->departmentID && docIdx == 0) {
                        h->doctorIDs[0] = dCur->doctorID;
                        h->doctorCount = 1;
                        // Try to add a second doctor
                        if (dCur->next && dCur->next->departmentID == assignmentWard->departmentID) {
                            h->doctorIDs[1] = dCur->next->doctorID;
                            h->doctorCount = 2;
                        }
                        break;
                    }
                    if (dCur->departmentID == assignmentWard->departmentID) docIdx++;
                    dCur = dCur->next;
                }
            }

            insertHospitalizationTail(hospHead, h);
        }
        pCur = pCur->next;
    }

    // ==================== Sample Registration, Consultation, Prescription ====================
    // Create a few sample records for demo purposes
    // We'll create 15 registrations, 10 consultations, 8 prescriptions, 5 examinations
    // to show the system working

    // Patient indices for sample data (first few patients)
    PatientNode* sampleP[10];
    pCur = patientHead;
    for (int i = 0; i < 10 && pCur; i++) {
        sampleP[i] = pCur;
        pCur = pCur->next;
    }

    // Doctor names for sample data
    DoctorNode* sampleD[5];
    DoctorNode* dCur = doctorHead;
    for (int i = 0; i < 5 && dCur; i++) {
        sampleD[i] = dCur;
        dCur = dCur->next;
    }

    // Create 15 registrations
    for (int i = 0; i < 10; i++) {
        RegistrationNode* r = new RegistrationNode();
        r->regID = generateRegID(0);
        r->patientID = sampleP[i]->patientID;
        DoctorNode* doc = sampleD[i % 5];
        r->doctorID = doc->doctorID;
        r->departmentID = doc->departmentID;
        r->regDay = 0;
        r->status = (i < 3) ? STATUS_SEEN : STATUS_PENDING;
        insertRegistrationTail(regHead, r);
    }

    // Create 5 more registrations for day 1
    for (int i = 0; i < 5; i++) {
        RegistrationNode* r = new RegistrationNode();
        r->regID = generateRegID(1);
        r->patientID = sampleP[i + 5]->patientID;
        DoctorNode* doc = sampleD[i % 5];
        r->doctorID = doc->doctorID;
        r->departmentID = doc->departmentID;
        r->regDay = 1;
        r->status = STATUS_PENDING;
        insertRegistrationTail(regHead, r);
    }

    // Create 3 consultations (for the first 3 registrations marked as SEEN)
    RegistrationNode* rCur = regHead;
    int consultCount = 0;
    while (rCur && consultCount < 3) {
        if (rCur->status == STATUS_SEEN) {
            ConsultationNode* cs = new ConsultationNode();
            cs->consultID = generateConsultID();
            cs->regID = rCur->regID;
            cs->patientID = rCur->patientID;
            cs->doctorID = rCur->doctorID;
            cs->complaint = "头痛发热，咳嗽三天";
            cs->diagnosis = "上呼吸道感染";
            cs->consultDay = 0;
            cs->status = STATUS_VALID;
            insertConsultationTail(consultHead, cs);

            // Create prescription for each consultation
            PrescriptionNode* pr = new PrescriptionNode();
            pr->prescID = generatePrescID();
            pr->consultID = cs->consultID;
            pr->patientID = cs->patientID;
            pr->doctorID = cs->doctorID;
            pr->prescDay = 0;
            pr->status = 0;

            // Add medicines to prescription
            double total = 0;
            MedicineNode* mCur = medHead;
            for (int mi = 0; mi < 2 && mCur; mi++) {
                PrescMedicineNode* pm = new PrescMedicineNode();
                pm->prescID = pr->prescID;
                pm->medID = mCur->medID;
                pm->quantity = 1 + mi;
                pm->unitPrice = 15.0 + mi * 5;
                insertPrescMedicineTail(prescMedHead, pm);
                total += pm->quantity * pm->unitPrice;
                mCur = mCur->next;
            }
            pr->totalAmount = round2(total);
            insertPrescriptionTail(prescHead, pr);

            // Create examination for 1st consultation only
            if (consultCount == 0) {
                ExaminationNode* ex = new ExaminationNode();
                ex->examID = generateExamID();
                ex->consultID = cs->consultID;
                ex->patientID = cs->patientID;
                ex->itemName = "血常规检查";
                ex->cost = 50.00;
                ex->examDay = 0;
                ex->status = STATUS_VALID;
                insertExaminationTail(examHead, ex);
            }
            consultCount++;
        }
        rCur = rCur->next;
    }
}
