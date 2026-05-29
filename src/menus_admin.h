#ifndef MENUS_ADMIN_H
#define MENUS_ADMIN_H

#include "entities.h"
#include "list_ops.h"

// Admin menu main entry - returns false to logout/exit
bool adminMenu(
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
    HospitalizationNode*& hospHead
);

// Admin login - returns true on success
bool adminLogin(AdminNode* adminHead);

#endif
