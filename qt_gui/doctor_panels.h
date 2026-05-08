#ifndef DOCTOR_PANELS_H
#define DOCTOR_PANELS_H
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QTabWidget>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include "../entities.h"

class DoctorWidget : public QWidget {
    Q_OBJECT
public:
    explicit DoctorWidget(const std::string& doctorID, QWidget* parent = nullptr);
    void refreshAll();
    std::string getDoctorID() const { return m_doctorID; }
private slots:
    void onConsult();           // 接诊
    void onPrescribeMedicine(); // 开处方
    void onOrderExam();         // 开检查
    void onCompleteConsult();   // 完成看诊
    void onViewInpatient();     // 查看住院患者详情
    void onPrescribeForInpatient();
    void onSearchPatientRecords();
    void onSearchDoctorRecords(); // for 处方与记录查询
    void onOutpatientPatientSelected();
    void onInpatientDoubleClicked(int row, int col);
private:
    void setupUI();
    void refreshWaitingList();
    void refreshOutpatientWork();
    void refreshMyInpatients();
    void refreshRecordSearch(const std::string& patientID);

    std::string m_doctorID;
    std::string m_currentConsultID;  // currently active consultation
    std::string m_currentPatientID;  // currently viewing patient

    QTabWidget* m_tabWidget;

    // Tab 1: Waiting list
    QTableWidget* waitingTable;
    QPushButton* consultBtn;

    // Tab 2: Outpatient work
    QListWidget* outpatientPatientList;
    QTableWidget* outpatientRecordTable;
    QPushButton* outpatientPrescribeBtn;
    QPushButton* outpatientExamBtn;
    QPushButton* outpatientCompleteBtn;
    QLabel* outpatientInfoLabel;
    std::string m_outpatientCurrentPatientID;
    std::string m_outpatientCurrentConsultID;

    // Tab 3: Inpatient list
    QTableWidget* inpatientTable;
    QPushButton* viewInpatientBtn;
    QPushButton* prescribeForInpBtn;

    // Tab 4: Record search
    QLineEdit* recordSearchEdit;
    QPushButton* recordSearchBtn;
    QTableWidget* recordTable;
};
#endif
