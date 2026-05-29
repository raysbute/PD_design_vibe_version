#ifndef BATCH_INPUT_H
#define BATCH_INPUT_H

#include "entities.h"
#include "list_ops.h"

// Parse batch input from a text file
// Returns the number of records successfully processed, or -1 on file error
int batchImportFromFile(const char* filename,
    DepartmentNode*& deptHead,
    DoctorNode*& doctorHead,
    PatientNode*& patientHead,
    RegistrationNode*& regHead,
    MedicineNode*& medHead,
    DeptMedicineNode*& deptMedHead,
    WardNode*& wardHead,
    AdminNode*& adminHead);

// Batch input from standard input (stdin) until EOF or empty line
int batchImportFromStdin(
    DepartmentNode*& deptHead,
    DoctorNode*& doctorHead,
    PatientNode*& patientHead,
    RegistrationNode*& regHead,
    MedicineNode*& medHead,
    DeptMedicineNode*& deptMedHead,
    WardNode*& wardHead,
    AdminNode*& adminHead);

#endif
