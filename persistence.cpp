#include "persistence.h"
#include <cstdio>
#include <vector>
#include <string>

static const char* FILE_GLOBAL = "save_global.txt";
static const char* FILE_DEPT = "save_department.txt";
static const char* FILE_DOCTOR = "save_doctor.txt";
static const char* FILE_PATIENT = "save_patient.txt";
static const char* FILE_REG = "save_registration.txt";
static const char* FILE_CONSULT = "save_consultation.txt";
static const char* FILE_EXAM = "save_examination.txt";
static const char* FILE_PRESC = "save_prescription.txt";
static const char* FILE_MED = "save_medicine.txt";
static const char* FILE_PRESCMED = "save_prescmedicine.txt";
static const char* FILE_DEPTMED = "save_deptmedicine.txt";
static const char* FILE_WARD = "save_ward.txt";
static const char* FILE_HOSP = "save_hospitalization.txt";
static const char* FILE_ADMIN = "save_admin.txt";

// Helper: join workDays array into string
static std::string joinWorkDays(int* days, int count) {
    std::string result;
    for (int i = 0; i < count; i++) {
        if (i > 0) result += ",";
        result += std::to_string(days[i]);
    }
    return result;
}

// Helper: parse workDays from string
static void parseWorkDays(const std::string& str, int* days, int& count) {
    count = 0;
    if (str.empty()) return;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',') && count < MAX_WORKDAYS) {
        days[count++] = atoi(token.c_str());
    }
}

// Helper: join doctor IDs array into string
static std::string joinDoctorIDs(std::string* ids, int count) {
    std::string result;
    for (int i = 0; i < count; i++) {
        if (i > 0) result += ",";
        result += ids[i];
    }
    return result;
}

// Helper: parse doctor IDs from string
static void parseDoctorIDs(const std::string& str, std::string* ids, int& count) {
    count = 0;
    if (str.empty()) return;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ',') && count < 10) {
        ids[count++] = token;
    }
}

// Helper: escape pipe characters in strings
static std::string esc(const std::string& s) {
    std::string r;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == '|') r += "\\|";
        else if (s[i] == '\\') r += "\\\\";
        else if (s[i] == '\n') r += "\\n";
        else r += s[i];
    }
    return r;
}

// Helper: unescape
static std::string unesc(const std::string& s) {
    std::string r;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == '\\' && i + 1 < s.length()) {
            if (s[i+1] == '|') { r += '|'; i++; }
            else if (s[i+1] == '\\') { r += '\\'; i++; }
            else if (s[i+1] == 'n') { r += '\n'; i++; }
            else r += s[i];
        } else {
            r += s[i];
        }
    }
    return r;
}

bool saveAllData(
    DepartmentNode* deptHead,
    DoctorNode* doctorHead,
    PatientNode* patientHead,
    RegistrationNode* regHead,
    ConsultationNode* consultHead,
    ExaminationNode* examHead,
    PrescriptionNode* prescHead,
    MedicineNode* medHead,
    PrescMedicineNode* prescMedHead,
    DeptMedicineNode* deptMedHead,
    WardNode* wardHead,
    HospitalizationNode* hospHead,
    AdminNode* adminHead) {

    // ===== Global State =====
    std::ofstream fg(FILE_GLOBAL);
    if (!fg) { std::cerr << "[错误] 无法写入 " << FILE_GLOBAL << std::endl; return false; }
    fg << globalTime << " " << weekday << " " << std::fixed << std::setprecision(2) << money << std::endl;
    fg << nextDeptID << " " << nextDoctorID << " " << nextPatientID << " " << nextRegID << " ";
    fg << nextConsultID << " " << nextExamID << " " << nextPrescID << " " << nextMedID << " ";
    fg << nextWardID << " " << nextHospID << std::endl;
    fg.close();

    // ===== Department =====
    std::ofstream fd(FILE_DEPT);
    if (!fd) return false;
    DepartmentNode* dc = deptHead;
    while (dc) {
        fd << esc(dc->deptID) << "|" << esc(dc->name) << "|" << esc(dc->description) << std::endl;
        dc = dc->next;
    }
    fd.close();

    // ===== Doctor =====
    std::ofstream fr(FILE_DOCTOR);
    if (!fr) return false;
    DoctorNode* d = doctorHead;
    while (d) {
        fr << esc(d->doctorID) << "|" << esc(d->name) << "|" << d->level << "|"
           << esc(d->departmentID) << "|" << joinWorkDays(d->workDays, d->workDayCount) << std::endl;
        d = d->next;
    }
    fr.close();

    // ===== Patient =====
    std::ofstream fp(FILE_PATIENT);
    if (!fp) return false;
    PatientNode* p = patientHead;
    while (p) {
        fp << esc(p->patientID) << "|" << esc(p->name) << "|" << p->age << "|"
           << esc(p->contact) << "|" << p->type << "|"
           << std::fixed << std::setprecision(2) << p->deposit << "|"
           << p->admitDays << "|" << esc(p->bedID) << std::endl;
        p = p->next;
    }
    fp.close();

    // ===== Registration =====
    std::ofstream freg(FILE_REG);
    if (!freg) return false;
    RegistrationNode* r = regHead;
    while (r) {
        freg << esc(r->regID) << "|" << esc(r->patientID) << "|"
             << esc(r->departmentID) << "|" << esc(r->doctorID) << "|"
             << r->regDay << "|" << r->status << std::endl;
        r = r->next;
    }
    freg.close();

    // ===== Consultation =====
    std::ofstream fcs(FILE_CONSULT);
    if (!fcs) return false;
    ConsultationNode* cs = consultHead;
    while (cs) {
        fcs << esc(cs->consultID) << "|" << esc(cs->regID) << "|"
            << esc(cs->patientID) << "|" << esc(cs->doctorID) << "|"
            << esc(cs->complaint) << "|" << esc(cs->diagnosis) << "|"
            << cs->consultDay << "|" << cs->status << std::endl;
        cs = cs->next;
    }
    fcs.close();

    // ===== Examination =====
    std::ofstream fex(FILE_EXAM);
    if (!fex) return false;
    ExaminationNode* ex = examHead;
    while (ex) {
        fex << esc(ex->examID) << "|" << esc(ex->consultID) << "|"
            << esc(ex->patientID) << "|" << esc(ex->itemName) << "|"
            << std::fixed << std::setprecision(2) << ex->cost << "|"
            << ex->examDay << "|" << ex->status << std::endl;
        ex = ex->next;
    }
    fex.close();

    // ===== Prescription =====
    std::ofstream fpr(FILE_PRESC);
    if (!fpr) return false;
    PrescriptionNode* pr = prescHead;
    while (pr) {
        fpr << esc(pr->prescID) << "|" << esc(pr->consultID) << "|"
            << esc(pr->patientID) << "|" << esc(pr->doctorID) << "|"
            << std::fixed << std::setprecision(2) << pr->totalAmount << "|"
            << pr->prescDay << "|" << pr->status << std::endl;
        pr = pr->next;
    }
    fpr.close();

    // ===== Medicine =====
    std::ofstream fm(FILE_MED);
    if (!fm) return false;
    MedicineNode* m = medHead;
    while (m) {
        fm << esc(m->medID) << "|" << esc(m->tradeName) << "|"
           << esc(m->genericName) << "|" << esc(m->alias) << "|"
           << esc(m->spec) << "|" << m->stock << "|" << m->consumed << std::endl;
        m = m->next;
    }
    fm.close();

    // ===== PrescMedicine =====
    std::ofstream fpm(FILE_PRESCMED);
    if (!fpm) return false;
    PrescMedicineNode* pm = prescMedHead;
    while (pm) {
        fpm << esc(pm->prescID) << "|" << esc(pm->medID) << "|"
            << pm->quantity << "|" << std::fixed << std::setprecision(2) << pm->unitPrice << std::endl;
        pm = pm->next;
    }
    fpm.close();

    // ===== DeptMedicine =====
    std::ofstream fdm(FILE_DEPTMED);
    if (!fdm) return false;
    DeptMedicineNode* dm = deptMedHead;
    while (dm) {
        fdm << esc(dm->deptID) << "|" << esc(dm->medID) << std::endl;
        dm = dm->next;
    }
    fdm.close();

    // ===== Ward =====
    std::ofstream fw(FILE_WARD);
    if (!fw) return false;
    WardNode* w = wardHead;
    while (w) {
        fw << esc(w->wardID) << "|" << w->type << "|" << esc(w->departmentID) << "|"
           << w->bedCount;
        for (int i = 0; i < w->bedCount; i++) {
            fw << "|" << esc(w->bedStatus[i]);
        }
        fw << std::endl;
        w = w->next;
    }
    fw.close();

    // ===== Hospitalization =====
    std::ofstream fh(FILE_HOSP);
    if (!fh) return false;
    HospitalizationNode* h = hospHead;
    while (h) {
        fh << esc(h->hospID) << "|" << esc(h->patientID) << "|"
           << esc(h->wardID) << "|" << h->bedNo << "|"
           << joinDoctorIDs(h->doctorIDs, h->doctorCount) << "|"
           << h->admitDay << "|" << h->dischargeDay << "|"
           << std::fixed << std::setprecision(2) << h->deposit << "|"
           << h->status << std::endl;
        h = h->next;
    }
    fh.close();

    // ===== Admin =====
    std::ofstream fa(FILE_ADMIN);
    if (!fa) return false;
    AdminNode* a = adminHead;
    while (a) {
        fa << esc(a->adminID) << "|" << esc(a->password) << std::endl;
        a = a->next;
    }
    fa.close();

    std::cout << "[提示] 所有数据已保存。" << std::endl;
    return true;
}

// Helper: split string by delimiter
static std::vector<std::string> splitLine(const std::string& line, char delim) {
    std::vector<std::string> result;
    std::string current;
    bool escape = false;
    for (size_t i = 0; i < line.length(); i++) {
        if (escape) {
            if (line[i] == '|') current += '|';
            else if (line[i] == '\\') current += '\\';
            else if (line[i] == 'n') current += '\n';
            else current += line[i];
            escape = false;
        } else if (line[i] == '\\') {
            escape = true;
        } else if (line[i] == delim) {
            result.push_back(current);
            current.clear();
        } else {
            current += line[i];
        }
    }
    result.push_back(current);
    return result;
}

bool loadAllData(
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

    // ===== Global State =====
    std::ifstream fg(FILE_GLOBAL);
    if (!fg) {
        std::cout << "[提示] 存档文件不存在，使用默认初始状态。" << std::endl;
        return false;
    }
    {
        unsigned int savedWeekday;
        if (!(fg >> globalTime >> savedWeekday >> money)) {
            std::cout << "[警告] 全局状态文件格式错误，使用默认初始状态。" << std::endl;
            fg.close();
            return false;
        }
        setTime(globalTime);  // recalculate weekday from globalTime
        fg >> nextDeptID >> nextDoctorID >> nextPatientID >> nextRegID
            >> nextConsultID >> nextExamID >> nextPrescID >> nextMedID
            >> nextWardID >> nextHospID;
    }
    fg.close();

    // ===== Department =====
    std::ifstream fd(FILE_DEPT);
    if (!fd) { std::cout << "[警告] " << FILE_DEPT << " 读取失败。" << std::endl; return false; }
    std::string line;
    while (std::getline(fd, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 3) { fd.close(); return false; }
        DepartmentNode* node = new DepartmentNode();
        node->deptID = parts[0];
        node->name = parts[1];
        node->description = parts[2];
        insertDepartmentTail(deptHead, node);
    }
    fd.close();

    // ===== Doctor =====
    std::ifstream fr(FILE_DOCTOR);
    if (!fr) { std::cout << "[警告] " << FILE_DOCTOR << " 读取失败。" << std::endl; return false; }
    while (std::getline(fr, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 5) { fr.close(); return false; }
        DoctorNode* node = new DoctorNode();
        node->doctorID = parts[0];
        node->name = parts[1];
        node->level = atoi(parts[2].c_str());
        node->departmentID = parts[3];
        parseWorkDays(parts[4], node->workDays, node->workDayCount);
        insertDoctorTail(doctorHead, node);
    }
    fr.close();

    // ===== Patient =====
    std::ifstream fp(FILE_PATIENT);
    if (!fp) { std::cout << "[警告] " << FILE_PATIENT << " 读取失败。" << std::endl; return false; }
    while (std::getline(fp, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 8) { fp.close(); return false; }
        PatientNode* node = new PatientNode();
        node->patientID = parts[0];
        node->name = parts[1];
        node->age = atoi(parts[2].c_str());
        node->contact = parts[3];
        node->type = atoi(parts[4].c_str());
        node->deposit = atof(parts[5].c_str());
        node->admitDays = atoi(parts[6].c_str());
        node->bedID = parts[7];
        insertPatientTail(patientHead, node);
    }
    fp.close();

    // ===== Registration =====
    std::ifstream freg(FILE_REG);
    if (!freg) { std::cout << "[警告] " << FILE_REG << " 读取失败。" << std::endl; return false; }
    while (std::getline(freg, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 6) { freg.close(); return false; }
        RegistrationNode* node = new RegistrationNode();
        node->regID = parts[0];
        node->patientID = parts[1];
        node->departmentID = parts[2];
        node->doctorID = parts[3];
        node->regDay = (unsigned int)atoi(parts[4].c_str());
        node->status = atoi(parts[5].c_str());
        insertRegistrationTail(regHead, node);
    }
    freg.close();

    // ===== Consultation =====
    std::ifstream fcs(FILE_CONSULT);
    if (!fcs) { return false; } // Optional file, just skip if missing
    while (std::getline(fcs, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 8) { fcs.close(); return false; }
        ConsultationNode* node = new ConsultationNode();
        node->consultID = parts[0];
        node->regID = parts[1];
        node->patientID = parts[2];
        node->doctorID = parts[3];
        node->complaint = parts[4];
        node->diagnosis = parts[5];
        node->consultDay = (unsigned int)atoi(parts[6].c_str());
        node->status = atoi(parts[7].c_str());
        insertConsultationTail(consultHead, node);
    }
    fcs.close();

    // ===== Examination =====
    std::ifstream fex(FILE_EXAM);
    if (!fex) { return false; }
    while (std::getline(fex, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 7) { fex.close(); return false; }
        ExaminationNode* node = new ExaminationNode();
        node->examID = parts[0];
        node->consultID = parts[1];
        node->patientID = parts[2];
        node->itemName = parts[3];
        node->cost = atof(parts[4].c_str());
        node->examDay = (unsigned int)atoi(parts[5].c_str());
        node->status = atoi(parts[6].c_str());
        insertExaminationTail(examHead, node);
    }
    fex.close();

    // ===== Prescription =====
    std::ifstream fpr(FILE_PRESC);
    if (!fpr) { return false; }
    while (std::getline(fpr, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 7) { fpr.close(); return false; }
        PrescriptionNode* node = new PrescriptionNode();
        node->prescID = parts[0];
        node->consultID = parts[1];
        node->patientID = parts[2];
        node->doctorID = parts[3];
        node->totalAmount = atof(parts[4].c_str());
        node->prescDay = (unsigned int)atoi(parts[5].c_str());
        node->status = atoi(parts[6].c_str());
        insertPrescriptionTail(prescHead, node);
    }
    fpr.close();

    // ===== Medicine =====
    std::ifstream fm(FILE_MED);
    if (!fm) { std::cout << "[警告] " << FILE_MED << " 读取失败。" << std::endl; return false; }
    while (std::getline(fm, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 7) { fm.close(); return false; }
        MedicineNode* node = new MedicineNode();
        node->medID = parts[0];
        node->tradeName = parts[1];
        node->genericName = parts[2];
        node->alias = parts[3];
        node->spec = parts[4];
        node->stock = atoi(parts[5].c_str());
        node->consumed = atoi(parts[6].c_str());
        insertMedicineTail(medHead, node);
    }
    fm.close();

    // ===== PrescMedicine =====
    std::ifstream fpm(FILE_PRESCMED);
    if (!fpm) { return false; }
    while (std::getline(fpm, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 4) { fpm.close(); return false; }
        PrescMedicineNode* node = new PrescMedicineNode();
        node->prescID = parts[0];
        node->medID = parts[1];
        node->quantity = atoi(parts[2].c_str());
        node->unitPrice = atof(parts[3].c_str());
        insertPrescMedicineTail(prescMedHead, node);
    }
    fpm.close();

    // ===== DeptMedicine =====
    std::ifstream fdm(FILE_DEPTMED);
    if (!fdm) { return false; }
    while (std::getline(fdm, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 2) { fdm.close(); return false; }
        DeptMedicineNode* node = new DeptMedicineNode();
        node->deptID = parts[0];
        node->medID = parts[1];
        insertDeptMedicineTail(deptMedHead, node);
    }
    fdm.close();

    // ===== Ward =====
    std::ifstream fw(FILE_WARD);
    if (!fw) { std::cout << "[警告] " << FILE_WARD << " 读取失败。" << std::endl; return false; }
    while (std::getline(fw, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 4) { fw.close(); return false; }
        WardNode* node = new WardNode();
        node->wardID = parts[0];
        node->type = atoi(parts[1].c_str());
        node->departmentID = parts[2];
        node->bedCount = atoi(parts[3].c_str());
        if (node->bedCount > MAX_BEDS) node->bedCount = MAX_BEDS;
        for (int i = 0; i < node->bedCount && (i + 4) < (int)parts.size(); i++) {
            node->bedStatus[i] = parts[i + 4];
        }
        insertWardTail(wardHead, node);
    }
    fw.close();

    // ===== Hospitalization =====
    std::ifstream fh(FILE_HOSP);
    if (!fh) { return false; }
    while (std::getline(fh, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 9) { fh.close(); return false; }
        HospitalizationNode* node = new HospitalizationNode();
        node->hospID = parts[0];
        node->patientID = parts[1];
        node->wardID = parts[2];
        node->bedNo = atoi(parts[3].c_str());
        parseDoctorIDs(parts[4], node->doctorIDs, node->doctorCount);
        node->admitDay = (unsigned int)atoi(parts[5].c_str());
        node->dischargeDay = (unsigned int)atoi(parts[6].c_str());
        node->deposit = atof(parts[7].c_str());
        node->status = atoi(parts[8].c_str());
        insertHospitalizationTail(hospHead, node);
    }
    fh.close();

    // ===== Admin =====
    std::ifstream fa(FILE_ADMIN);
    if (!fa) { std::cout << "[警告] " << FILE_ADMIN << " 读取失败。" << std::endl; return false; }
    while (std::getline(fa, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts = splitLine(line, '|');
        if (parts.size() < 2) { fa.close(); return false; }
        AdminNode* node = new AdminNode();
        node->adminID = parts[0];
        node->password = parts[1];
        insertAdminTail(adminHead, node);
    }
    fa.close();

    std::cout << "[提示] 数据加载成功。" << std::endl;
    return true;
}
