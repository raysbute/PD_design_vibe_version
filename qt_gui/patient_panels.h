#ifndef PATIENT_PANELS_H
#define PATIENT_PANELS_H
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QTabWidget>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include "../entities.h"

class PatientWidget : public QWidget {
    Q_OBJECT
public:
    explicit PatientWidget(const std::string& patientID, QWidget* parent = nullptr);
    void refreshAll();
    std::string getPatientID() const { return m_patientID; }
private slots:
    void onPayFees();
    void onTopUpDeposit();
    void onRefresh();
private:
    void setupUI();
    void refreshMyRecords();
    void refreshPaymentList();
    void refreshInpatientInfo();

    std::string m_patientID;
    bool m_isInpatient;
    PatientNode* m_patient;

    QTabWidget* m_tabWidget;
    QTableWidget* recordsTable;
    QTableWidget* paymentTable;
    QLabel* totalUnpaidLabel;
    QPushButton* payBtn;

    QWidget* inpatientTab;
    QLabel* wardLabel;
    QLabel* bedLabel;
    QLabel* doctorsLabel;
    QLabel* daysLabel;
    QLabel* depositLabel;
    QTableWidget* feeDetailTable;
    QPushButton* topUpBtn;
};
#endif
