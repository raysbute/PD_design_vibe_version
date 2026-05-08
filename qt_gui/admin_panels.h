#ifndef ADMIN_PANELS_H
#define ADMIN_PANELS_H
#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "../entities.h"

class AdminWidget : public QWidget {
    Q_OBJECT
public:
    explicit AdminWidget(QWidget* parent = nullptr);
    void refreshAll();
private slots:
    // Department
    void onAddDept();
    void onModifyDept();
    void onDeleteDept();
    void onDeptDoubleClicked(int row, int col);
    // Doctor
    void onAddDoctor();
    void onModifyDoctor();
    void onDeleteDoctor();
    void onFilterDoctorByDept(const QString& deptID);
    // Patient
    void onAddPatient();
    void onModifyPatient();
    void onDeletePatient();
    void onSearchPatient();
    void onPatientDoubleClicked(int row, int col);
    // Registration
    void onDeptSelectedForReg();
    void onDoctorSelectedForReg();
    void onRegister();
    // Hospitalization
    void onAdmitPatient();
    void onDischargePatient();
    void onHospPatientDoubleClicked(int row, int col);
    // Medicine
    void onAddMedicine();
    void onModifyMedicine();
    void onDeleteMedicine();
    void onStockIn();
    void onStockOut();
    // Reports
    void refreshReportTabs();
    void onTimeAdvance();
private:
    void setupDeptPanel();
    void setupDoctorPanel();
    void setupPatientPanel();
    void setupRegPanel();
    void setupHospPanel();
    void setupMedPanel();
    void setupReportPanel();
    void refreshDeptTable();
    void refreshDoctorTable();
    void refreshPatientTable(const QString& filter = "");
    void refreshRegTable();
    void refreshMedTable();
    void refreshWardGrid();
    void refreshHospTable();

    QTabWidget* tabWidget;
    // Department panel widgets
    QTableWidget* deptTable;
    // Doctor panel widgets
    QTableWidget* doctorTable;
    QComboBox* doctorDeptFilter;
    // Patient panel widgets
    QTableWidget* patientTable;
    QLineEdit* patientSearch;
    // Registration panel widgets
    QComboBox* regDeptCombo;
    QComboBox* regDoctorCombo;
    QLineEdit* regPatientSearch;
    QLabel* regDoctorLoad;
    QTableWidget* regTable;
    // Hospitalization panel widgets
    QTableWidget* wardGridTable;
    QComboBox* hospWardCombo;
    QComboBox* hospBedCombo;
    QLineEdit* hospPatientSearch;
    QTableWidget* hospDoctorList;
    QLineEdit* hospDeposit;
    QTableWidget* hospTable;
    // Medicine panel widgets
    QTableWidget* medTable;
    // Report panel widgets
    QTabWidget* reportTabs;
    QTableWidget* medReportTable;
    QTableWidget* doctorReportTable;
    QTableWidget* revenueReportTable;
};
#endif
