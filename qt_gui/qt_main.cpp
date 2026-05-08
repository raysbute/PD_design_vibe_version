#include <QApplication>
#include "main_window.h"
#include "../persistence.h"
#include "../init_data.h"

// Declare the linked list heads as extern
extern DepartmentNode* deptHead;
extern DoctorNode* doctorHead;
extern PatientNode* patientHead;
extern RegistrationNode* regHead;
extern ConsultationNode* consultHead;
extern ExaminationNode* examHead;
extern PrescriptionNode* prescHead;
extern MedicineNode* medHead;
extern PrescMedicineNode* prescMedHead;
extern DeptMedicineNode* deptMedHead;
extern WardNode* wardHead;
extern HospitalizationNode* hospHead;
extern AdminNode* adminHead;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("医疗管理系统");

    // Try to load saved data, else init defaults
    if (!loadAllData(deptHead, doctorHead, patientHead, regHead, consultHead,
                     examHead, prescHead, medHead, prescMedHead, deptMedHead,
                     wardHead, hospHead, adminHead)) {
        initDefaultData(deptHead, doctorHead, patientHead, regHead, consultHead,
                       examHead, prescHead, medHead, prescMedHead, deptMedHead,
                       wardHead, hospHead, adminHead);
    }

    MainWindow w;
    w.show();

    int ret = app.exec();

    // Save on exit
    saveAllData(deptHead, doctorHead, patientHead, regHead, consultHead,
                examHead, prescHead, medHead, prescMedHead, deptMedHead,
                wardHead, hospHead, adminHead);

    // Free all memory (call cleanup functions from list_ops.h)
    freeDepartmentList(deptHead);
    freeDoctorList(doctorHead);
    freePatientList(patientHead);
    freeRegistrationList(regHead);
    freeConsultationList(consultHead);
    freeExaminationList(examHead);
    freePrescriptionList(prescHead);
    freeMedicineList(medHead);
    freePrescMedicineList(prescMedHead);
    freeDeptMedicineList(deptMedHead);
    freeWardList(wardHead);
    freeHospitalizationList(hospHead);
    freeAdminList(adminHead);

    return ret;
}
