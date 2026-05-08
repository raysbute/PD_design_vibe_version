#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include "entities.h"
#include "list_ops.h"

// Save all data to files
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
    AdminNode* adminHead
);

// Load all data from files. Returns false if files missing/corrupt => use defaults.
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
    AdminNode*& adminHead
);

#endif
