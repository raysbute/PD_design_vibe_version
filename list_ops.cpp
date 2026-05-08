#include "list_ops.h"
#include <cstdio>
#include <cstdlib>

// ==================== Global ID counters ====================
int nextDeptID = 1;
int nextDoctorID = 1;
int nextPatientID = 1;
int nextRegID = 1;
int nextConsultID = 1;
int nextExamID = 1;
int nextPrescID = 1;
int nextMedID = 1;
int nextWardID = 1;
int nextHospID = 1;

// ==================== ID Generation ====================
std::string generateDeptID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%03d", PREFIX_DEPT, nextDeptID++);
    return std::string(buf);
}
std::string generateDoctorID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%03d", PREFIX_DOCTOR, nextDoctorID++);
    return std::string(buf);
}
std::string generatePatientID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%03d", PREFIX_PATIENT, nextPatientID++);
    return std::string(buf);
}
std::string generateRegID(unsigned int day) {
    char buf[32];
    // R-YYMMDD-序号
    snprintf(buf, sizeof(buf), "R-%04d-%03d", day, nextRegID++);
    return std::string(buf);
}
std::string generateConsultID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%04d", PREFIX_CONSULT, nextConsultID++);
    return std::string(buf);
}
std::string generateExamID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%04d", PREFIX_EXAM, nextExamID++);
    return std::string(buf);
}
std::string generatePrescID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%04d", PREFIX_PRESC, nextPrescID++);
    return std::string(buf);
}
std::string generateMedID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%03d", PREFIX_MED, nextMedID++);
    return std::string(buf);
}
std::string generateWardID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%03d", PREFIX_WARD, nextWardID++);
    return std::string(buf);
}
std::string generateHospID(unsigned int day) {
    char buf[32];
    snprintf(buf, sizeof(buf), "HSP-%04d-%03d", day, nextHospID++);
    return std::string(buf);
}

// Extract numeric portion from ID string for counter update
static int extractNum(const std::string& id, const std::string& prefix) {
    size_t pos = id.find(prefix);
    if (pos == std::string::npos) return 0;
    std::string numStr = id.substr(prefix.length());
    return atoi(numStr.c_str());
}

static int extractLastNum(const std::string& id) {
    // For IDs like "R-0001-003" or "HSP-0001-003", get last number
    size_t lastDash = id.rfind('-');
    if (lastDash == std::string::npos) return 0;
    return atoi(id.substr(lastDash + 1).c_str());
}

void updateIDCountersFromLists(DepartmentNode* dHead, DoctorNode* docHead, PatientNode* pHead,
    RegistrationNode* rHead, ConsultationNode* cHead, ExaminationNode* eHead,
    PrescriptionNode* prHead, MedicineNode* mHead, WardNode* wHead, HospitalizationNode* hHead) {
    int maxVal;
    DepartmentNode* d = dHead;
    maxVal = 0;
    while (d) { int n = extractNum(d->deptID, PREFIX_DEPT); if (n > maxVal) maxVal = n; d = d->next; }
    nextDeptID = maxVal + 1;

    DoctorNode* doc = docHead;
    maxVal = 0;
    while (doc) { int n = extractNum(doc->doctorID, PREFIX_DOCTOR); if (n > maxVal) maxVal = n; doc = doc->next; }
    nextDoctorID = maxVal + 1;

    PatientNode* p = pHead;
    maxVal = 0;
    while (p) { int n = extractNum(p->patientID, PREFIX_PATIENT); if (n > maxVal) maxVal = n; p = p->next; }
    nextPatientID = maxVal + 1;

    RegistrationNode* r = rHead;
    maxVal = 0;
    while (r) { int n = extractLastNum(r->regID); if (n > maxVal) maxVal = n; r = r->next; }
    nextRegID = maxVal + 1;

    ConsultationNode* c = cHead;
    maxVal = 0;
    while (c) { int n = extractNum(c->consultID, PREFIX_CONSULT); if (n > maxVal) maxVal = n; c = c->next; }
    nextConsultID = maxVal + 1;

    ExaminationNode* e = eHead;
    maxVal = 0;
    while (e) { int n = extractNum(e->examID, PREFIX_EXAM); if (n > maxVal) maxVal = n; e = e->next; }
    nextExamID = maxVal + 1;

    PrescriptionNode* pr = prHead;
    maxVal = 0;
    while (pr) { int n = extractNum(pr->prescID, PREFIX_PRESC); if (n > maxVal) maxVal = n; pr = pr->next; }
    nextPrescID = maxVal + 1;

    MedicineNode* m = mHead;
    maxVal = 0;
    while (m) { int n = extractNum(m->medID, PREFIX_MED); if (n > maxVal) maxVal = n; m = m->next; }
    nextMedID = maxVal + 1;

    WardNode* w = wHead;
    maxVal = 0;
    while (w) { int n = extractNum(w->wardID, PREFIX_WARD); if (n > maxVal) maxVal = n; w = w->next; }
    nextWardID = maxVal + 1;

    HospitalizationNode* h = hHead;
    maxVal = 0;
    while (h) { int n = extractLastNum(h->hospID); if (n > maxVal) maxVal = n; h = h->next; }
    nextHospID = maxVal + 1;
}

// ==================== Department List Ops ====================
DepartmentNode* insertDepartmentHead(DepartmentNode*& head, DepartmentNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
DepartmentNode* insertDepartmentTail(DepartmentNode*& head, DepartmentNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    DepartmentNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
DepartmentNode* insertDepartmentSorted(DepartmentNode*& head, DepartmentNode* node) {
    if (!node) return head;
    if (!head || head->deptID > node->deptID) {
        node->next = head;
        head = node;
        return head;
    }
    DepartmentNode* cur = head;
    while (cur->next && cur->next->deptID < node->deptID) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return head;
}
bool deleteDepartmentByID(DepartmentNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->deptID == id) {
        DepartmentNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    DepartmentNode* cur = head;
    while (cur->next && cur->next->deptID != id) cur = cur->next;
    if (!cur->next) return false;
    DepartmentNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
DepartmentNode* findDepartmentByID(DepartmentNode* head, const std::string& id) {
    DepartmentNode* cur = head;
    while (cur) {
        if (cur->deptID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
int countDepartments(DepartmentNode* head) {
    int cnt = 0;
    while (head) { cnt++; head = head->next; }
    return cnt;
}
void freeDepartmentList(DepartmentNode*& head) {
    while (head) {
        DepartmentNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Doctor List Ops ====================
DoctorNode* insertDoctorHead(DoctorNode*& head, DoctorNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
DoctorNode* insertDoctorTail(DoctorNode*& head, DoctorNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    DoctorNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
DoctorNode* insertDoctorSorted(DoctorNode*& head, DoctorNode* node) {
    if (!node) return head;
    if (!head || head->doctorID > node->doctorID) {
        node->next = head;
        head = node;
        return head;
    }
    DoctorNode* cur = head;
    while (cur->next && cur->next->doctorID < node->doctorID) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return head;
}
bool deleteDoctorByID(DoctorNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->doctorID == id) {
        DoctorNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    DoctorNode* cur = head;
    while (cur->next && cur->next->doctorID != id) cur = cur->next;
    if (!cur->next) return false;
    DoctorNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
DoctorNode* findDoctorByID(DoctorNode* head, const std::string& id) {
    DoctorNode* cur = head;
    while (cur) {
        if (cur->doctorID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
int countDoctors(DoctorNode* head) {
    int cnt = 0;
    while (head) { cnt++; head = head->next; }
    return cnt;
}
void freeDoctorList(DoctorNode*& head) {
    while (head) {
        DoctorNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Patient List Ops ====================
PatientNode* insertPatientHead(PatientNode*& head, PatientNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
PatientNode* insertPatientTail(PatientNode*& head, PatientNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    PatientNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
PatientNode* insertPatientSorted(PatientNode*& head, PatientNode* node) {
    if (!node) return head;
    if (!head || head->patientID > node->patientID) {
        node->next = head;
        head = node;
        return head;
    }
    PatientNode* cur = head;
    while (cur->next && cur->next->patientID < node->patientID) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return head;
}
bool deletePatientByID(PatientNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->patientID == id) {
        PatientNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    PatientNode* cur = head;
    while (cur->next && cur->next->patientID != id) cur = cur->next;
    if (!cur->next) return false;
    PatientNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
PatientNode* findPatientByID(PatientNode* head, const std::string& id) {
    PatientNode* cur = head;
    while (cur) {
        if (cur->patientID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
int countPatients(PatientNode* head) {
    int cnt = 0;
    while (head) { cnt++; head = head->next; }
    return cnt;
}
void freePatientList(PatientNode*& head) {
    while (head) {
        PatientNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Registration List Ops ====================
RegistrationNode* insertRegistrationHead(RegistrationNode*& head, RegistrationNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
RegistrationNode* insertRegistrationTail(RegistrationNode*& head, RegistrationNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    RegistrationNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deleteRegistrationByID(RegistrationNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->regID == id) {
        RegistrationNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    RegistrationNode* cur = head;
    while (cur->next && cur->next->regID != id) cur = cur->next;
    if (!cur->next) return false;
    RegistrationNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
RegistrationNode* findRegistrationByID(RegistrationNode* head, const std::string& id) {
    RegistrationNode* cur = head;
    while (cur) {
        if (cur->regID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
void freeRegistrationList(RegistrationNode*& head) {
    while (head) {
        RegistrationNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Consultation List Ops ====================
ConsultationNode* insertConsultationHead(ConsultationNode*& head, ConsultationNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
ConsultationNode* insertConsultationTail(ConsultationNode*& head, ConsultationNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    ConsultationNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deleteConsultationByID(ConsultationNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->consultID == id) {
        ConsultationNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    ConsultationNode* cur = head;
    while (cur->next && cur->next->consultID != id) cur = cur->next;
    if (!cur->next) return false;
    ConsultationNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
ConsultationNode* findConsultationByID(ConsultationNode* head, const std::string& id) {
    ConsultationNode* cur = head;
    while (cur) {
        if (cur->consultID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
void freeConsultationList(ConsultationNode*& head) {
    while (head) {
        ConsultationNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Examination List Ops ====================
ExaminationNode* insertExaminationHead(ExaminationNode*& head, ExaminationNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
ExaminationNode* insertExaminationTail(ExaminationNode*& head, ExaminationNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    ExaminationNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deleteExaminationByID(ExaminationNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->examID == id) {
        ExaminationNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    ExaminationNode* cur = head;
    while (cur->next && cur->next->examID != id) cur = cur->next;
    if (!cur->next) return false;
    ExaminationNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
ExaminationNode* findExaminationByID(ExaminationNode* head, const std::string& id) {
    ExaminationNode* cur = head;
    while (cur) {
        if (cur->examID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
void freeExaminationList(ExaminationNode*& head) {
    while (head) {
        ExaminationNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Prescription List Ops ====================
PrescriptionNode* insertPrescriptionHead(PrescriptionNode*& head, PrescriptionNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
PrescriptionNode* insertPrescriptionTail(PrescriptionNode*& head, PrescriptionNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    PrescriptionNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deletePrescriptionByID(PrescriptionNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->prescID == id) {
        PrescriptionNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    PrescriptionNode* cur = head;
    while (cur->next && cur->next->prescID != id) cur = cur->next;
    if (!cur->next) return false;
    PrescriptionNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
PrescriptionNode* findPrescriptionByID(PrescriptionNode* head, const std::string& id) {
    PrescriptionNode* cur = head;
    while (cur) {
        if (cur->prescID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
void freePrescriptionList(PrescriptionNode*& head) {
    while (head) {
        PrescriptionNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Medicine List Ops ====================
MedicineNode* insertMedicineHead(MedicineNode*& head, MedicineNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
MedicineNode* insertMedicineTail(MedicineNode*& head, MedicineNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    MedicineNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
MedicineNode* insertMedicineSorted(MedicineNode*& head, MedicineNode* node) {
    if (!node) return head;
    if (!head || head->medID > node->medID) {
        node->next = head;
        head = node;
        return head;
    }
    MedicineNode* cur = head;
    while (cur->next && cur->next->medID < node->medID) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
    return head;
}
bool deleteMedicineByID(MedicineNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->medID == id) {
        MedicineNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    MedicineNode* cur = head;
    while (cur->next && cur->next->medID != id) cur = cur->next;
    if (!cur->next) return false;
    MedicineNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
MedicineNode* findMedicineByID(MedicineNode* head, const std::string& id) {
    MedicineNode* cur = head;
    while (cur) {
        if (cur->medID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
int countMedicines(MedicineNode* head) {
    int cnt = 0;
    while (head) { cnt++; head = head->next; }
    return cnt;
}
void freeMedicineList(MedicineNode*& head) {
    while (head) {
        MedicineNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== PrescMedicine List Ops ====================
PrescMedicineNode* insertPrescMedicineHead(PrescMedicineNode*& head, PrescMedicineNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
PrescMedicineNode* insertPrescMedicineTail(PrescMedicineNode*& head, PrescMedicineNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    PrescMedicineNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
void freePrescMedicineList(PrescMedicineNode*& head) {
    while (head) {
        PrescMedicineNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== DeptMedicine List Ops ====================
DeptMedicineNode* insertDeptMedicineHead(DeptMedicineNode*& head, DeptMedicineNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
DeptMedicineNode* insertDeptMedicineTail(DeptMedicineNode*& head, DeptMedicineNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    DeptMedicineNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deleteDeptMedicine(DeptMedicineNode*& head, const std::string& deptID, const std::string& medID) {
    if (!head) return false;
    if (head->deptID == deptID && head->medID == medID) {
        DeptMedicineNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    DeptMedicineNode* cur = head;
    while (cur->next && !(cur->next->deptID == deptID && cur->next->medID == medID)) cur = cur->next;
    if (!cur->next) return false;
    DeptMedicineNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
bool findDeptMedicine(DeptMedicineNode* head, const std::string& deptID, const std::string& medID) {
    DeptMedicineNode* cur = head;
    while (cur) {
        if (cur->deptID == deptID && cur->medID == medID) return true;
        cur = cur->next;
    }
    return false;
}
void freeDeptMedicineList(DeptMedicineNode*& head) {
    while (head) {
        DeptMedicineNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Ward List Ops ====================
WardNode* insertWardHead(WardNode*& head, WardNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
WardNode* insertWardTail(WardNode*& head, WardNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    WardNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deleteWardByID(WardNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->wardID == id) {
        WardNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    WardNode* cur = head;
    while (cur->next && cur->next->wardID != id) cur = cur->next;
    if (!cur->next) return false;
    WardNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
WardNode* findWardByID(WardNode* head, const std::string& id) {
    WardNode* cur = head;
    while (cur) {
        if (cur->wardID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
int getFreeBedCount(WardNode* ward) {
    if (!ward) return 0;
    int cnt = 0;
    for (int i = 0; i < ward->bedCount; i++) {
        if (ward->bedStatus[i].empty()) cnt++;
    }
    return cnt;
}
int getAllocateBed(WardNode* ward, const std::string& patientID) {
    if (!ward) return -1;
    for (int i = 0; i < ward->bedCount; i++) {
        if (ward->bedStatus[i].empty()) {
            ward->bedStatus[i] = patientID;
            return i;
        }
    }
    return -1;
}
bool releaseBed(WardNode* ward, int bedNo) {
    if (!ward || bedNo < 0 || bedNo >= ward->bedCount) return false;
    ward->bedStatus[bedNo] = "";
    return true;
}
void freeWardList(WardNode*& head) {
    while (head) {
        WardNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Hospitalization List Ops ====================
HospitalizationNode* insertHospitalizationHead(HospitalizationNode*& head, HospitalizationNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
HospitalizationNode* insertHospitalizationTail(HospitalizationNode*& head, HospitalizationNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    HospitalizationNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
bool deleteHospitalizationByID(HospitalizationNode*& head, const std::string& id) {
    if (!head) return false;
    if (head->hospID == id) {
        HospitalizationNode* tmp = head;
        head = head->next;
        delete tmp;
        return true;
    }
    HospitalizationNode* cur = head;
    while (cur->next && cur->next->hospID != id) cur = cur->next;
    if (!cur->next) return false;
    HospitalizationNode* tmp = cur->next;
    cur->next = tmp->next;
    delete tmp;
    return true;
}
HospitalizationNode* findHospitalizationByID(HospitalizationNode* head, const std::string& id) {
    HospitalizationNode* cur = head;
    while (cur) {
        if (cur->hospID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
HospitalizationNode* findActiveHospitalizationByPatient(HospitalizationNode* head, const std::string& patientID) {
    HospitalizationNode* cur = head;
    while (cur) {
        if (cur->patientID == patientID && cur->status == 0) return cur;
        cur = cur->next;
    }
    return nullptr;
}
void freeHospitalizationList(HospitalizationNode*& head) {
    while (head) {
        HospitalizationNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ==================== Admin List Ops ====================
AdminNode* insertAdminHead(AdminNode*& head, AdminNode* node) {
    if (!node) return head;
    node->next = head;
    head = node;
    return head;
}
AdminNode* insertAdminTail(AdminNode*& head, AdminNode* node) {
    if (!node) return head;
    if (!head) { head = node; return head; }
    AdminNode* cur = head;
    while (cur->next) cur = cur->next;
    cur->next = node;
    node->next = nullptr;
    return head;
}
AdminNode* findAdminByID(AdminNode* head, const std::string& id) {
    AdminNode* cur = head;
    while (cur) {
        if (cur->adminID == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}
void freeAdminList(AdminNode*& head) {
    while (head) {
        AdminNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}
