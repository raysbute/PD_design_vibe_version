#ifndef MENUS_DOCTOR_H
#define MENUS_DOCTOR_H
#include "entities.h"
bool doctorMenu(DoctorNode*& doctorHead, PatientNode* patientHead, RegistrationNode*& regHead,
    ConsultationNode*& consultHead, ExaminationNode*& examHead, PrescriptionNode*& prescHead,
    PrescMedicineNode*& prescMedHead, MedicineNode* medHead, DeptMedicineNode* deptMedHead,
    WardNode* wardHead, HospitalizationNode* hospHead);
#endif
