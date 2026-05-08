#ifndef ENTITIES_H
#define ENTITIES_H

#include "global.h"

// Forward declarations for all node types
struct DepartmentNode;
struct DoctorNode;
struct PatientNode;
struct RegistrationNode;
struct ConsultationNode;
struct ExaminationNode;
struct PrescriptionNode;
struct MedicineNode;
struct PrescMedicineNode;
struct WardNode;
struct HospitalizationNode;
struct AdminNode;

// 1. Department
struct DepartmentNode {
    std::string deptID;
    std::string name;
    std::string description;
    DepartmentNode* next;
    DepartmentNode() : next(nullptr) {}
};

// 2. Doctor
struct DoctorNode {
    std::string doctorID;
    std::string name;
    int level;  // 0=主任, 1=副主任, 2=主治, 3=住院
    std::string departmentID;
    int workDays[MAX_WORKDAYS];
    int workDayCount;
    DoctorNode* next;
    DoctorNode() : level(0), workDayCount(0), next(nullptr) {
        for (int i = 0; i < MAX_WORKDAYS; i++) workDays[i] = 0;
    }
};

// 3. Patient
// For linked list traversal we need a unified patient list
// Use type field to distinguish outpatient/inpatient
struct PatientNode {
    std::string patientID;
    std::string name;
    int age;
    std::string contact;
    int type;  // 0=outpatient, 1=inpatient
    // Inpatient-specific fields
    double deposit;     // 住院押金余额
    int admitDays;      // 入院天数
    std::string bedID;  // 所在床位 (wardID + "-" + bedNo)
    PatientNode* next;
    PatientNode() : age(0), type(0), deposit(0.0), admitDays(0), next(nullptr) {}
};

// 4. Registration
struct RegistrationNode {
    std::string regID;
    std::string patientID;
    std::string departmentID;
    std::string doctorID;
    unsigned int regDay;  // weekday when registered
    int status;  // 0=pending, 1=seen, 2=expired, 3=cancelled
    RegistrationNode* next;
    RegistrationNode() : regDay(0), status(0), next(nullptr) {}
};

// 5. Consultation
struct ConsultationNode {
    std::string consultID;
    std::string regID;
    std::string patientID;
    std::string doctorID;
    std::string complaint;
    std::string diagnosis;
    unsigned int consultDay;
    int status;  // 0=valid, 1=cancelled
    ConsultationNode* next;
    ConsultationNode() : consultDay(0), status(0), next(nullptr) {}
};

// 6. Examination
struct ExaminationNode {
    std::string examID;
    std::string consultID;  // can be consultID or hospID
    std::string patientID;
    std::string itemName;
    double cost;
    unsigned int examDay;
    int status;  // 0=valid, 1=cancelled
    ExaminationNode* next;
    ExaminationNode() : cost(0.0), examDay(0), status(0), next(nullptr) {}
};

// 7. Prescription
struct PrescriptionNode {
    std::string prescID;
    std::string consultID;
    std::string patientID;
    std::string doctorID;
    double totalAmount;
    unsigned int prescDay;
    int status;  // 0=valid (unpaid), 1=cancelled, 2=paid
    PrescriptionNode* next;
    PrescriptionNode() : totalAmount(0.0), prescDay(0), status(0), next(nullptr) {}
};

// 8. Medicine
struct MedicineNode {
    std::string medID;
    std::string tradeName;
    std::string genericName;
    std::string alias;
    std::string spec;
    int stock;
    int consumed;
    MedicineNode* next;
    MedicineNode() : stock(0), consumed(0), next(nullptr) {}
};

// 8b. Prescription-Medicine link (medicines in a prescription)
struct PrescMedicineNode {
    std::string prescID;
    std::string medID;
    int quantity;
    double unitPrice;
    PrescMedicineNode* next;
    PrescMedicineNode() : quantity(0), unitPrice(0.0), next(nullptr) {}
};

// 9. Department-Medicine link
struct DeptMedicineNode {
    std::string deptID;
    std::string medID;
    DeptMedicineNode* next;
    DeptMedicineNode() : next(nullptr) {}
};

// 10. Ward
struct WardNode {
    std::string wardID;
    int type;  // 0=regular, 1=special
    std::string departmentID;
    int bedCount;
    std::string bedStatus[MAX_BEDS];  // bed occupied by patientID (empty string = free)
    WardNode* next;
    WardNode() : type(0), bedCount(0), next(nullptr) {
        for (int i = 0; i < MAX_BEDS; i++) bedStatus[i] = "";
    }
};

// 11. Hospitalization
struct HospitalizationNode {
    std::string hospID;
    std::string patientID;
    std::string wardID;
    int bedNo;
    std::string doctorIDs[10];  // responsible doctors
    int doctorCount;
    unsigned int admitDay;
    unsigned int dischargeDay;  // 0 = not discharged
    double deposit;
    int status;  // 0=in-hospital, 1=discharged, 2=cancelled
    HospitalizationNode* next;
    HospitalizationNode() : bedNo(-1), doctorCount(0), admitDay(0), dischargeDay(0), deposit(0.0), status(0), next(nullptr) {}
};

// 12. Admin
struct AdminNode {
    std::string adminID;
    std::string password;
    AdminNode* next;
    AdminNode() : next(nullptr) {}
};

#endif
