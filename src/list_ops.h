#ifndef LIST_OPS_H
#define LIST_OPS_H

#include "entities.h"

// ==================== Department ====================
DepartmentNode* insertDepartmentHead(DepartmentNode*& head, DepartmentNode* node);
DepartmentNode* insertDepartmentTail(DepartmentNode*& head, DepartmentNode* node);
DepartmentNode* insertDepartmentSorted(DepartmentNode*& head, DepartmentNode* node);
bool deleteDepartmentByID(DepartmentNode*& head, const std::string& id);
DepartmentNode* findDepartmentByID(DepartmentNode* head, const std::string& id);
int countDepartments(DepartmentNode* head);
void freeDepartmentList(DepartmentNode*& head);

// ==================== Doctor ====================
DoctorNode* insertDoctorHead(DoctorNode*& head, DoctorNode* node);
DoctorNode* insertDoctorTail(DoctorNode*& head, DoctorNode* node);
DoctorNode* insertDoctorSorted(DoctorNode*& head, DoctorNode* node);
bool deleteDoctorByID(DoctorNode*& head, const std::string& id);
DoctorNode* findDoctorByID(DoctorNode* head, const std::string& id);
int countDoctors(DoctorNode* head);
void freeDoctorList(DoctorNode*& head);

// ==================== Patient ====================
PatientNode* insertPatientHead(PatientNode*& head, PatientNode* node);
PatientNode* insertPatientTail(PatientNode*& head, PatientNode* node);
PatientNode* insertPatientSorted(PatientNode*& head, PatientNode* node);
bool deletePatientByID(PatientNode*& head, const std::string& id);
PatientNode* findPatientByID(PatientNode* head, const std::string& id);
int countPatients(PatientNode* head);
void freePatientList(PatientNode*& head);

// ==================== Registration ====================
RegistrationNode* insertRegistrationHead(RegistrationNode*& head, RegistrationNode* node);
RegistrationNode* insertRegistrationTail(RegistrationNode*& head, RegistrationNode* node);
bool deleteRegistrationByID(RegistrationNode*& head, const std::string& id);
RegistrationNode* findRegistrationByID(RegistrationNode* head, const std::string& id);
void freeRegistrationList(RegistrationNode*& head);

// ==================== Consultation ====================
ConsultationNode* insertConsultationHead(ConsultationNode*& head, ConsultationNode* node);
ConsultationNode* insertConsultationTail(ConsultationNode*& head, ConsultationNode* node);
bool deleteConsultationByID(ConsultationNode*& head, const std::string& id);
ConsultationNode* findConsultationByID(ConsultationNode* head, const std::string& id);
void freeConsultationList(ConsultationNode*& head);

// ==================== Examination ====================
ExaminationNode* insertExaminationHead(ExaminationNode*& head, ExaminationNode* node);
ExaminationNode* insertExaminationTail(ExaminationNode*& head, ExaminationNode* node);
bool deleteExaminationByID(ExaminationNode*& head, const std::string& id);
ExaminationNode* findExaminationByID(ExaminationNode* head, const std::string& id);
void freeExaminationList(ExaminationNode*& head);

// ==================== Prescription ====================
PrescriptionNode* insertPrescriptionHead(PrescriptionNode*& head, PrescriptionNode* node);
PrescriptionNode* insertPrescriptionTail(PrescriptionNode*& head, PrescriptionNode* node);
bool deletePrescriptionByID(PrescriptionNode*& head, const std::string& id);
PrescriptionNode* findPrescriptionByID(PrescriptionNode* head, const std::string& id);
void freePrescriptionList(PrescriptionNode*& head);

// ==================== Medicine ====================
MedicineNode* insertMedicineHead(MedicineNode*& head, MedicineNode* node);
MedicineNode* insertMedicineTail(MedicineNode*& head, MedicineNode* node);
MedicineNode* insertMedicineSorted(MedicineNode*& head, MedicineNode* node);
bool deleteMedicineByID(MedicineNode*& head, const std::string& id);
MedicineNode* findMedicineByID(MedicineNode* head, const std::string& id);
int countMedicines(MedicineNode* head);
void freeMedicineList(MedicineNode*& head);

// ==================== PrescMedicine ====================
PrescMedicineNode* insertPrescMedicineHead(PrescMedicineNode*& head, PrescMedicineNode* node);
PrescMedicineNode* insertPrescMedicineTail(PrescMedicineNode*& head, PrescMedicineNode* node);
void freePrescMedicineList(PrescMedicineNode*& head);

// ==================== DeptMedicine ====================
DeptMedicineNode* insertDeptMedicineHead(DeptMedicineNode*& head, DeptMedicineNode* node);
DeptMedicineNode* insertDeptMedicineTail(DeptMedicineNode*& head, DeptMedicineNode* node);
bool deleteDeptMedicine(DeptMedicineNode*& head, const std::string& deptID, const std::string& medID);
bool findDeptMedicine(DeptMedicineNode* head, const std::string& deptID, const std::string& medID);
void freeDeptMedicineList(DeptMedicineNode*& head);

// ==================== Ward ====================
WardNode* insertWardHead(WardNode*& head, WardNode* node);
WardNode* insertWardTail(WardNode*& head, WardNode* node);
bool deleteWardByID(WardNode*& head, const std::string& id);
WardNode* findWardByID(WardNode* head, const std::string& id);
int getFreeBedCount(WardNode* ward);
int getAllocateBed(WardNode* ward, const std::string& patientID);
bool releaseBed(WardNode* ward, int bedNo);
void freeWardList(WardNode*& head);

// ==================== Hospitalization ====================
HospitalizationNode* insertHospitalizationHead(HospitalizationNode*& head, HospitalizationNode* node);
HospitalizationNode* insertHospitalizationTail(HospitalizationNode*& head, HospitalizationNode* node);
bool deleteHospitalizationByID(HospitalizationNode*& head, const std::string& id);
HospitalizationNode* findHospitalizationByID(HospitalizationNode* head, const std::string& id);
HospitalizationNode* findActiveHospitalizationByPatient(HospitalizationNode* head, const std::string& patientID);
void freeHospitalizationList(HospitalizationNode*& head);

// ==================== Admin ====================
AdminNode* insertAdminHead(AdminNode*& head, AdminNode* node);
AdminNode* insertAdminTail(AdminNode*& head, AdminNode* node);
AdminNode* findAdminByID(AdminNode* head, const std::string& id);
void freeAdminList(AdminNode*& head);

// ==================== Utility: next ID counters ====================
extern int nextDeptID;
extern int nextDoctorID;
extern int nextPatientID;
extern int nextRegID;
extern int nextConsultID;
extern int nextExamID;
extern int nextPrescID;
extern int nextMedID;
extern int nextWardID;
extern int nextHospID;

std::string generateDeptID();
std::string generateDoctorID();
std::string generatePatientID();
std::string generateRegID(unsigned int day);
std::string generateConsultID();
std::string generateExamID();
std::string generatePrescID();
std::string generateMedID();
std::string generateWardID();
std::string generateHospID(unsigned int day);

// Reset all ID counters (used after loading from file)
void updateIDCountersFromLists(DepartmentNode* dHead, DoctorNode* docHead, PatientNode* pHead,
    RegistrationNode* rHead, ConsultationNode* cHead, ExaminationNode* eHead,
    PrescriptionNode* prHead, MedicineNode* mHead, WardNode* wHead, HospitalizationNode* hHead);

#endif
