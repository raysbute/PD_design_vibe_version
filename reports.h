#ifndef REPORTS_H
#define REPORTS_H
#include "entities.h"
#include "list_ops.h"

void adminReportMenu(
    DepartmentNode* deptHead, DoctorNode* doctorHead, PatientNode* patientHead,
    RegistrationNode* regHead, ConsultationNode* consultHead, ExaminationNode* examHead,
    PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead, MedicineNode* medHead,
    DeptMedicineNode* deptMedHead, WardNode* wardHead, HospitalizationNode* hospHead);

void doctorReportMenu(const std::string& doctorID,
    DepartmentNode* deptHead, DoctorNode* doctorHead, PatientNode* patientHead,
    RegistrationNode* regHead, ConsultationNode* consultHead, ExaminationNode* examHead,
    PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead, MedicineNode* medHead,
    WardNode* wardHead, HospitalizationNode* hospHead);

void patientReportMenu(const std::string& patientID,
    PatientNode* patientHead, RegistrationNode* regHead, ConsultationNode* consultHead,
    ExaminationNode* examHead, PrescriptionNode* prescHead, PrescMedicineNode* prescMedHead,
    MedicineNode* medHead, WardNode* wardHead, HospitalizationNode* hospHead, DoctorNode* doctorHead);
#endif
