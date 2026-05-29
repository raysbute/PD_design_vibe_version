#ifndef INIT_DATA_H
#define INIT_DATA_H

#include "entities.h"
#include "list_ops.h"

// Initialize all default data
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
    AdminNode*& adminHead
);

#endif
