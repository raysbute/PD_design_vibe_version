#ifndef MENUS_PATIENT_H
#define MENUS_PATIENT_H
#include "entities.h"
bool patientMenu(PatientNode* patientHead, RegistrationNode* regHead, ConsultationNode* consultHead,
    ExaminationNode* examHead, PrescriptionNode*& prescHead, PrescMedicineNode*& prescMedHead,
    MedicineNode*& medHead, WardNode* wardHead, HospitalizationNode* hospHead,
    DoctorNode* doctorHead);
#endif
