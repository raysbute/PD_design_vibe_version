#include "doctor_panels.h"
#include "../list_ops.h"
#include "../utils.h"
#include "../global.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QSet>

// External linked list heads
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

// ==================== DoctorWidget ====================

DoctorWidget::DoctorWidget(const std::string& doctorID, QWidget* parent)
    : QWidget(parent), m_doctorID(doctorID),
      m_tabWidget(nullptr),
      waitingTable(nullptr), consultBtn(nullptr),
      outpatientPatientList(nullptr), outpatientRecordTable(nullptr),
      outpatientPrescribeBtn(nullptr), outpatientExamBtn(nullptr),
      outpatientCompleteBtn(nullptr), outpatientInfoLabel(nullptr),
      inpatientTable(nullptr), viewInpatientBtn(nullptr), prescribeForInpBtn(nullptr),
      recordSearchEdit(nullptr), recordSearchBtn(nullptr), recordTable(nullptr)
{
    setupUI();
}

void DoctorWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Doctor info header
    DoctorNode* doctor = findDoctorByID(doctorHead, m_doctorID);
    QLabel* headerLabel = new QLabel();
    if (doctor) {
        headerLabel->setText(QString::fromUtf8("医生工作台  \u2014  %1  %2  (\u79D1\u5BA4: %3)")
            .arg(QString::fromStdString(doctor->name))
            .arg(QString::fromUtf8(getLevelName(doctor->level)))
            .arg(QString::fromStdString(doctor->departmentID)));
    } else {
        headerLabel->setText(QString::fromUtf8("\u533B\u751F\u5DE5\u4F5C\u53F0"));
    }
    headerLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 8px;");
    mainLayout->addWidget(headerLabel);

    // Tab widget
    m_tabWidget = new QTabWidget();
    mainLayout->addWidget(m_tabWidget);

    // ============ Tab 1: 我的待诊 ============
    QWidget* tab1 = new QWidget();
    QVBoxLayout* tab1Layout = new QVBoxLayout(tab1);
    waitingTable = new QTableWidget();
    waitingTable->setColumnCount(5);
    waitingTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u6302\u53F7ID"),
        QString::fromUtf8("\u60A3\u8005ID"),
        QString::fromUtf8("\u59D3\u540D"),
        QString::fromUtf8("\u5E74\u9F84"),
        QString::fromUtf8("\u6302\u53F7\u65E5\u671F")
    });
    waitingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    waitingTable->setSelectionMode(QAbstractItemView::SingleSelection);
    waitingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    waitingTable->horizontalHeader()->setStretchLastSection(true);
    tab1Layout->addWidget(waitingTable);

    QHBoxLayout* btnLayout1 = new QHBoxLayout();
    consultBtn = new QPushButton(QString::fromUtf8("\u63A5\u8BCA"));
    consultBtn->setMinimumHeight(36);
    btnLayout1->addStretch();
    btnLayout1->addWidget(consultBtn);
    tab1Layout->addLayout(btnLayout1);
    connect(consultBtn, &QPushButton::clicked, this, &DoctorWidget::onConsult);

    // ============ Tab 2: 门诊工作 ============
    QWidget* tab2 = new QWidget();
    QVBoxLayout* tab2Layout = new QVBoxLayout(tab2);
    QSplitter* splitter2 = new QSplitter(Qt::Horizontal);

    // Left panel: patient list
    QWidget* leftPanel2 = new QWidget();
    QVBoxLayout* leftLayout2 = new QVBoxLayout(leftPanel2);
    QLabel* leftLabel2 = new QLabel(QString::fromUtf8("\u5DF2\u63A5\u8BCA\u60A3\u8005:"));
    leftLabel2->setStyleSheet("font-weight: bold;");
    leftLayout2->addWidget(leftLabel2);
    outpatientPatientList = new QListWidget();
    leftLayout2->addWidget(outpatientPatientList);
    leftLayout2->addStretch();

    // Right panel: records
    QWidget* rightPanel2 = new QWidget();
    QVBoxLayout* rightLayout2 = new QVBoxLayout(rightPanel2);
    outpatientInfoLabel = new QLabel(QString::fromUtf8("\u8BF7\u4ECE\u5DE6\u4FA7\u9009\u62E9\u60A3\u8005"));
    outpatientInfoLabel->setStyleSheet("font-weight: bold;");
    rightLayout2->addWidget(outpatientInfoLabel);
    outpatientRecordTable = new QTableWidget();
    outpatientRecordTable->setColumnCount(5);
    outpatientRecordTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u8BB0\u5F55\u7C7B\u578B"),
        QString::fromUtf8("\u8BB0\u5F55ID"),
        QString::fromUtf8("\u5185\u5BB9\u6458\u8981"),
        QString::fromUtf8("\u65E5\u671F"),
        QString::fromUtf8("\u72B6\u6001")
    });
    outpatientRecordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    outpatientRecordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    outpatientRecordTable->horizontalHeader()->setStretchLastSection(true);
    rightLayout2->addWidget(outpatientRecordTable);

    QHBoxLayout* btnLayout2b = new QHBoxLayout();
    outpatientPrescribeBtn = new QPushButton(QString::fromUtf8("\u5F00\u5904\u65B9"));
    outpatientExamBtn = new QPushButton(QString::fromUtf8("\u5F00\u68C0\u67E5"));
    outpatientCompleteBtn = new QPushButton(QString::fromUtf8("\u5B8C\u6210\u770B\u8BCA"));
    btnLayout2b->addWidget(outpatientPrescribeBtn);
    btnLayout2b->addWidget(outpatientExamBtn);
    btnLayout2b->addWidget(outpatientCompleteBtn);
    rightLayout2->addLayout(btnLayout2b);

    splitter2->addWidget(leftPanel2);
    splitter2->addWidget(rightPanel2);
    splitter2->setStretchFactor(0, 1);
    splitter2->setStretchFactor(1, 3);
    tab2Layout->addWidget(splitter2);

    connect(outpatientPatientList, &QListWidget::itemClicked,
            this, &DoctorWidget::onOutpatientPatientSelected);
    connect(outpatientPrescribeBtn, &QPushButton::clicked,
            this, &DoctorWidget::onPrescribeMedicine);
    connect(outpatientExamBtn, &QPushButton::clicked,
            this, &DoctorWidget::onOrderExam);
    connect(outpatientCompleteBtn, &QPushButton::clicked,
            this, &DoctorWidget::onCompleteConsult);

    // ============ Tab 3: 我的住院患者 ============
    QWidget* tab3 = new QWidget();
    QVBoxLayout* tab3Layout = new QVBoxLayout(tab3);
    inpatientTable = new QTableWidget();
    inpatientTable->setColumnCount(7);
    inpatientTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u4F4F\u9662ID"),
        QString::fromUtf8("\u60A3\u8005ID"),
        QString::fromUtf8("\u59D3\u540D"),
        QString::fromUtf8("\u75C5\u623F"),
        QString::fromUtf8("\u5E8A\u4F4D"),
        QString::fromUtf8("\u5165\u9662\u5929\u6570"),
        QString::fromUtf8("\u62BC\u91D1\u4F59\u989D")
    });
    inpatientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    inpatientTable->setSelectionMode(QAbstractItemView::SingleSelection);
    inpatientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    inpatientTable->horizontalHeader()->setStretchLastSection(true);
    tab3Layout->addWidget(inpatientTable);

    QHBoxLayout* btnLayout3 = new QHBoxLayout();
    viewInpatientBtn = new QPushButton(QString::fromUtf8("\u67E5\u770B\u8BE6\u60C5"));
    prescribeForInpBtn = new QPushButton(QString::fromUtf8("\u5F00\u5904\u65B9/\u68C0\u67E5"));
    btnLayout3->addStretch();
    btnLayout3->addWidget(viewInpatientBtn);
    btnLayout3->addWidget(prescribeForInpBtn);
    tab3Layout->addLayout(btnLayout3);
    connect(viewInpatientBtn, &QPushButton::clicked, this, &DoctorWidget::onViewInpatient);
    connect(prescribeForInpBtn, &QPushButton::clicked, this, &DoctorWidget::onPrescribeForInpatient);
    connect(inpatientTable, &QTableWidget::cellDoubleClicked,
            this, &DoctorWidget::onInpatientDoubleClicked);

    // ============ Tab 4: 处方与记录查询 ============
    QWidget* tab4 = new QWidget();
    QVBoxLayout* tab4Layout = new QVBoxLayout(tab4);
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel(QString::fromUtf8("\u60A3\u8005\u59D3\u540D/ID:")));
    recordSearchEdit = new QLineEdit();
    recordSearchEdit->setPlaceholderText(
        QString::fromUtf8("\u8F93\u5165\u60A3\u8005\u59D3\u540D\u6216ID\u8FDB\u884C\u641C\u7D22"));
    searchLayout->addWidget(recordSearchEdit);
    recordSearchBtn = new QPushButton(QString::fromUtf8("\u641C\u7D22"));
    searchLayout->addWidget(recordSearchBtn);
    tab4Layout->addLayout(searchLayout);

    recordTable = new QTableWidget();
    recordTable->setColumnCount(8);
    recordTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u8BB0\u5F55\u7C7B\u578B"),
        QString::fromUtf8("\u8BB0\u5F55ID"),
        QString::fromUtf8("\u60A3\u8005ID"),
        QString::fromUtf8("\u60A3\u8005\u59D3\u540D"),
        QString::fromUtf8("\u5185\u5BB9\u6458\u8981"),
        QString::fromUtf8("\u65E5\u671F"),
        QString::fromUtf8("\u91D1\u989D"),
        QString::fromUtf8("\u72B6\u6001")
    });
    recordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recordTable->horizontalHeader()->setStretchLastSection(true);
    tab4Layout->addWidget(recordTable);
    connect(recordSearchBtn, &QPushButton::clicked, this, &DoctorWidget::onSearchDoctorRecords);
    // Also search on Enter
    connect(recordSearchEdit, &QLineEdit::returnPressed, this, &DoctorWidget::onSearchDoctorRecords);

    m_tabWidget->addTab(tab1, QString::fromUtf8("\u6211\u7684\u5F85\u8BCA"));
    m_tabWidget->addTab(tab2, QString::fromUtf8("\u95E8\u8BCA\u5DE5\u4F5C"));
    m_tabWidget->addTab(tab3, QString::fromUtf8("\u6211\u7684\u4F4F\u9662\u60A3\u8005"));
    m_tabWidget->addTab(tab4, QString::fromUtf8("\u5904\u65B9\u4E0E\u8BB0\u5F55\u67E5\u8BE2"));

    refreshAll();
}

void DoctorWidget::refreshAll() {
    refreshWaitingList();
    refreshOutpatientWork();
    refreshMyInpatients();
}

// ======================================================================
// Tab 1: Waiting List
// ======================================================================

void DoctorWidget::refreshWaitingList() {
    waitingTable->setRowCount(0);
    int row = 0;
    RegistrationNode* r = regHead;
    while (r) {
        if (r->doctorID == m_doctorID && r->status == STATUS_PENDING) {
            waitingTable->insertRow(row);
            waitingTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromStdString(r->regID)));
            waitingTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(r->patientID)));
            PatientNode* p = findPatientByID(patientHead, r->patientID);
            waitingTable->setItem(row, 2, new QTableWidgetItem(
                p ? QString::fromStdString(p->name) : QString::fromUtf8("\u672A\u77E5")));
            waitingTable->setItem(row, 3, new QTableWidgetItem(
                p ? QString::number(p->age) : "?"));
            waitingTable->setItem(row, 4, new QTableWidgetItem(
                QString("Day%1").arg(r->regDay)));
            row++;
        }
        r = r->next;
    }
    if (row == 0) {
        waitingTable->insertRow(0);
        QTableWidgetItem* emptyItem = new QTableWidgetItem(
            QString::fromUtf8("\u6682\u65E0\u5F85\u8BCA\u60A3\u8005"));
        emptyItem->setTextAlignment(Qt::AlignCenter);
        waitingTable->setSpan(0, 0, 1, 5);
        waitingTable->setItem(0, 0, emptyItem);
    }
    waitingTable->resizeColumnsToContents();
}

// ======================================================================
// 接诊 (Consultation) dialog
// ======================================================================

void DoctorWidget::onConsult() {
    // Determine which patient to consult
    std::string patientID;
    RegistrationNode* reg = nullptr;

    if (m_tabWidget->currentIndex() == 0) {
        // From Tab 1: Waiting list
        int row = waitingTable->currentRow();
        if (row < 0) {
            QMessageBox::information(this,
                QString::fromUtf8("\u63D0\u793A"),
                QString::fromUtf8("\u8BF7\u5148\u4ECE\u5F85\u8BCA\u5217\u8868\u4E2D\u9009\u62E9\u4E00\u4F4D\u60A3\u8005\u3002"));
            return;
        }
        QTableWidgetItem* item = waitingTable->item(row, 0);
        if (!item || item->text() == QString::fromUtf8("\u6682\u65E0\u5F85\u8BCA\u60A3\u8005"))
            return;
        std::string regID = item->text().toStdString();
        reg = findRegistrationByID(regHead, regID);
        if (!reg) {
            QMessageBox::warning(this,
                QString::fromUtf8("\u9519\u8BEF"),
                QString::fromUtf8("\u6302\u53F7\u8BB0\u5F55\u4E0D\u5B58\u5728\u3002"));
            return;
        }
        if (reg->status != STATUS_PENDING) {
            QMessageBox::warning(this,
                QString::fromUtf8("\u9519\u8BEF"),
                QString::fromUtf8("\u8BE5\u6302\u53F7\u5DF2\u7ECF\u63A5\u8BCA\u8FC7\u4E86\u3002"));
            return;
        }
        patientID = reg->patientID;
    } else if (m_tabWidget->currentIndex() == 1) {
        // From Tab 2: Outpatient work
        QListWidgetItem* item = outpatientPatientList->currentItem();
        if (!item) {
            QMessageBox::information(this,
                QString::fromUtf8("\u63D0\u793A"),
                QString::fromUtf8("\u8BF7\u5148\u4ECE\u5DE6\u4FA7\u9009\u62E9\u4E00\u4F4D\u60A3\u8005\u3002"));
            return;
        }
        patientID = item->data(Qt::UserRole).toString().toStdString();
        // For already-seen patients, find any active registration
        RegistrationNode* r = regHead;
        while (r) {
            if (r->patientID == patientID && r->doctorID == m_doctorID) {
                if (r->status == STATUS_PENDING) {
                    reg = r;
                    break;
                }
            }
            r = r->next;
        }
    } else {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u8BF7\u5728\u201C\u6211\u7684\u5F85\u8BCA\u201D\u6216\u201C\u95E8\u8BCA\u5DE5\u4F5C\u201D\u6807\u7B7E\u9875\u4E2D\u8FDB\u884C\u63A5\u8BCA\u3002"));
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, patientID);
    if (!patient) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u9519\u8BEF"),
            QString::fromUtf8("\u60A3\u8005\u4FE1\u606F\u4E0D\u5B58\u5728\u3002"));
        return;
    }

    // Build consultation dialog
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("\u63A5\u8BCA"));
    dlg.resize(520, 480);
    QVBoxLayout* dlgLayout = new QVBoxLayout(&dlg);

    // Patient info display
    QGroupBox* infoGroup = new QGroupBox(QString::fromUtf8("\u60A3\u8005\u4FE1\u606F"));
    QFormLayout* infoForm = new QFormLayout(infoGroup);
    infoForm->addRow(QString::fromUtf8("\u60A3\u8005ID:"),
                     new QLabel(QString::fromStdString(patient->patientID)));
    infoForm->addRow(QString::fromUtf8("\u59D3\u540D:"),
                     new QLabel(QString::fromStdString(patient->name)));
    infoForm->addRow(QString::fromUtf8("\u5E74\u9F84:"),
                     new QLabel(QString::number(patient->age)));
    if (!patient->contact.empty())
        infoForm->addRow(QString::fromUtf8("\u8054\u7CFB\u65B9\u5F0F:"),
                         new QLabel(QString::fromStdString(patient->contact)));
    if (reg)
        infoForm->addRow(QString::fromUtf8("\u6302\u53F7ID:"),
                         new QLabel(QString::fromStdString(reg->regID)));
    dlgLayout->addWidget(infoGroup);

    // Complaint
    dlgLayout->addWidget(new QLabel(
        QString::fromUtf8("\u4E3B\u8BC9:")));
    QTextEdit* complaintEdit = new QTextEdit();
    complaintEdit->setMaximumHeight(80);
    complaintEdit->setPlaceholderText(
        QString::fromUtf8("\u8BF7\u8F93\u5165\u60A3\u8005\u4E3B\u8BC9..."));
    dlgLayout->addWidget(complaintEdit);

    // Diagnosis
    dlgLayout->addWidget(new QLabel(
        QString::fromUtf8("\u8BCA\u65AD:")));
    QTextEdit* diagnosisEdit = new QTextEdit();
    diagnosisEdit->setMaximumHeight(80);
    diagnosisEdit->setPlaceholderText(
        QString::fromUtf8("\u8BF7\u8F93\u5165\u8BCA\u65AD\u7ED3\u679C..."));
    dlgLayout->addWidget(diagnosisEdit);

    // Generate consult ID for this new consultation
    std::string consultID = generateConsultID();
    bool consultCreated = false;
    m_currentConsultID = consultID;
    m_currentPatientID = patientID;

    // Buttons at bottom
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnExam = new QPushButton(
        QString::fromUtf8("\u5F00\u68C0\u67E5"));
    QPushButton* btnPresc = new QPushButton(
        QString::fromUtf8("\u5F00\u5904\u65B9"));
    QPushButton* btnComplete = new QPushButton(
        QString::fromUtf8("\u5B8C\u6210\u770B\u8BCA"));
    QPushButton* btnCancel = new QPushButton(
        QString::fromUtf8("\u53D6\u6D88"));
    btnLayout->addWidget(btnExam);
    btnLayout->addWidget(btnPresc);
    btnLayout->addWidget(btnComplete);
    btnLayout->addWidget(btnCancel);
    dlgLayout->addLayout(btnLayout);

    // --- 开检查 ---
    connect(btnExam, &QPushButton::clicked, [&]() {
        QDialog examDlg(&dlg);
        examDlg.setWindowTitle(QString::fromUtf8("\u5F00\u5177\u68C0\u67E5"));
        QFormLayout* examForm = new QFormLayout(&examDlg);
        QLineEdit* nameEdit = new QLineEdit();
        QDoubleSpinBox* costSpin = new QDoubleSpinBox();
        costSpin->setRange(0.0, 999999.99);
        costSpin->setDecimals(2);
        costSpin->setPrefix(QString::fromUtf8("\u00A5 "));
        examForm->addRow(QString::fromUtf8("\u68C0\u67E5\u540D\u79F0:"), nameEdit);
        examForm->addRow(QString::fromUtf8("\u8D39\u7528:"), costSpin);
        QDialogButtonBox* examBtns = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        examBtns->button(QDialogButtonBox::Ok)->setText(
            QString::fromUtf8("\u786E\u5B9A"));
        examBtns->button(QDialogButtonBox::Cancel)->setText(
            QString::fromUtf8("\u53D6\u6D88"));
        examForm->addRow(examBtns);
        connect(examBtns, &QDialogButtonBox::accepted, &examDlg, &QDialog::accept);
        connect(examBtns, &QDialogButtonBox::rejected, &examDlg, &QDialog::reject);

        if (examDlg.exec() == QDialog::Accepted) {
            QString itemName = nameEdit->text().trimmed();
            if (itemName.isEmpty()) {
                QMessageBox::warning(&examDlg,
                    QString::fromUtf8("\u9519\u8BEF"),
                    QString::fromUtf8("\u68C0\u67E5\u540D\u79F0\u4E0D\u80FD\u4E3A\u7A7A\u3002"));
                return;
            }
            ExaminationNode* exam = new ExaminationNode();
            exam->examID = generateExamID();
            exam->consultID = consultID;
            exam->patientID = patientID;
            exam->itemName = itemName.toStdString();
            exam->cost = round2(costSpin->value());
            exam->examDay = weekday;
            exam->status = STATUS_VALID;
            insertExaminationTail(examHead, exam);
            QMessageBox::information(&dlg,
                QString::fromUtf8("\u6210\u529F"),
                QString::fromUtf8("\u68C0\u67E5\u5F00\u5177\u6210\u529F\uFF01\n\u68C0\u67E5ID: %1\n\u9879\u76EE: %2\n\u8D39\u7528: \u00A5%3")
                    .arg(QString::fromStdString(exam->examID))
                    .arg(itemName)
                    .arg(exam->cost, 0, 'f', 2));
        }
    });

    // --- 开处方 ---
    connect(btnPresc, &QPushButton::clicked, [&]() {
        QDialog prescDlg(&dlg);
        prescDlg.setWindowTitle(QString::fromUtf8("\u5F00\u5177\u5904\u65B9"));
        prescDlg.resize(750, 550);
        QVBoxLayout* prescLayout = new QVBoxLayout(&prescDlg);

        // Patient info
        QLabel* prescInfoLabel = new QLabel(
            QString::fromUtf8("\u60A3\u8005: %1 (%2)  \u8BCA\u65AD: %3")
                .arg(QString::fromStdString(patient->name))
                .arg(QString::fromStdString(patientID))
                .arg(diagnosisEdit->toPlainText()));
        prescInfoLabel->setStyleSheet("font-weight: bold;");
        prescLayout->addWidget(prescInfoLabel);

        // Search
        QHBoxLayout* searchRow = new QHBoxLayout();
        searchRow->addWidget(new QLabel(
            QString::fromUtf8("\u641C\u7D22\u836F\u54C1:")));
        QLineEdit* medSearchEdit = new QLineEdit();
        medSearchEdit->setPlaceholderText(
            QString::fromUtf8("\u8F93\u5165\u836F\u54C1\u540D\u79F0\u8FC7\u6EE4..."));
        searchRow->addWidget(medSearchEdit);
        prescLayout->addLayout(searchRow);

        QSplitter* prescSplitter = new QSplitter(Qt::Horizontal);

        // Left: medicine list
        QWidget* medListWidget = new QWidget();
        QVBoxLayout* medListLayout = new QVBoxLayout(medListWidget);
        medListLayout->addWidget(new QLabel(
            QString::fromUtf8("\u836F\u54C1\u5E93:")));
        QTableWidget* medTable = new QTableWidget();
        medTable->setColumnCount(3);
        medTable->setHorizontalHeaderLabels({
            QString::fromUtf8("\u836F\u54C1ID"),
            QString::fromUtf8("\u5546\u54C1\u540D"),
            QString::fromUtf8("\u5E93\u5B58")
        });
        medTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        medTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        medTable->horizontalHeader()->setStretchLastSection(true);
        medListLayout->addWidget(medTable);

        // Right: selected medicines
        QWidget* selectedWidget = new QWidget();
        QVBoxLayout* selectedLayout = new QVBoxLayout(selectedWidget);
        selectedLayout->addWidget(new QLabel(
            QString::fromUtf8("\u5DF2\u9009\u836F\u54C1:")));
        QTableWidget* selectedTable = new QTableWidget();
        selectedTable->setColumnCount(4);
        selectedTable->setHorizontalHeaderLabels({
            QString::fromUtf8("\u836F\u54C1\u540D"),
            QString::fromUtf8("\u6570\u91CF"),
            QString::fromUtf8("\u5355\u4EF7"),
            QString::fromUtf8("\u5C0F\u8BA1")
        });
        selectedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        selectedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        selectedTable->horizontalHeader()->setStretchLastSection(true);
        selectedLayout->addWidget(selectedTable);

        prescSplitter->addWidget(medListWidget);
        prescSplitter->addWidget(selectedWidget);
        prescSplitter->setStretchFactor(0, 2);
        prescSplitter->setStretchFactor(1, 1);
        prescLayout->addWidget(prescSplitter);

        // Bottom: total and confirm
        QHBoxLayout* bottomRow = new QHBoxLayout();
        QLabel* totalLabel = new QLabel(
            QString::fromUtf8("\u603B\u91D1\u989D: \u00A50.00"));
        totalLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
        bottomRow->addWidget(totalLabel);
        bottomRow->addStretch();
        QPushButton* confirmBtn = new QPushButton(
            QString::fromUtf8("\u786E\u8BA4\u5F00\u5177\u5904\u65B9"));
        confirmBtn->setMinimumHeight(36);
        bottomRow->addWidget(confirmBtn);
        QPushButton* cancelPrescBtn = new QPushButton(
            QString::fromUtf8("\u53D6\u6D88"));
        bottomRow->addWidget(cancelPrescBtn);
        prescLayout->addLayout(bottomRow);

        // Data storage for selected items
        struct SelectedItem { std::string medID; int qty; double price; };
        QVector<SelectedItem> selectedItems;

        auto populateMedTable = [&](const QString& filter) {
            medTable->setRowCount(0);
            int r = 0;
            MedicineNode* m = medHead;
            while (m) {
                QString tradeName = QString::fromStdString(m->tradeName);
                if (filter.isEmpty()
                    || tradeName.contains(filter, Qt::CaseInsensitive)) {
                    medTable->insertRow(r);
                    medTable->setItem(r, 0, new QTableWidgetItem(
                        QString::fromStdString(m->medID)));
                    medTable->setItem(r, 1, new QTableWidgetItem(tradeName));
                    medTable->setItem(r, 2, new QTableWidgetItem(
                        QString::number(m->stock)));
                    r++;
                }
                m = m->next;
            }
            medTable->resizeColumnsToContents();
        };
        populateMedTable("");

        auto refreshSelectedTable = [&]() {
            selectedTable->setRowCount(0);
            double total = 0.0;
            for (int i = 0; i < selectedItems.size(); i++) {
                const SelectedItem& si = selectedItems[i];
                MedicineNode* m = findMedicineByID(medHead, si.medID);
                selectedTable->insertRow(i);
                selectedTable->setItem(i, 0, new QTableWidgetItem(
                    m ? QString::fromStdString(m->tradeName)
                      : QString::fromStdString(si.medID)));
                selectedTable->setItem(i, 1, new QTableWidgetItem(
                    QString::number(si.qty)));
                selectedTable->setItem(i, 2, new QTableWidgetItem(
                    QString::fromUtf8("\u00A5%1").arg(si.price, 0, 'f', 2)));
                double sub = round2(si.qty * si.price);
                selectedTable->setItem(i, 3, new QTableWidgetItem(
                    QString::fromUtf8("\u00A5%1").arg(sub, 0, 'f', 2)));
                total = round2(total + sub);
            }
            totalLabel->setText(
                QString::fromUtf8("\u603B\u91D1\u989D: \u00A5%1")
                    .arg(total, 0, 'f', 2));
        };

        connect(medSearchEdit, &QLineEdit::textChanged, [&](const QString& txt) {
            populateMedTable(txt);
        });

        connect(medTable, &QTableWidget::cellDoubleClicked, [&](int row, int col) {
            Q_UNUSED(col);
            QTableWidgetItem* idItem = medTable->item(row, 0);
            if (!idItem) return;
            std::string medID = idItem->text().toStdString();
            MedicineNode* m = findMedicineByID(medHead, medID);
            if (!m) return;

            bool ok = false;
            int qty = QInputDialog::getInt(&prescDlg,
                QString::fromUtf8("\u6570\u91CF"),
                QString::fromUtf8("\u836F\u54C1: %1\n\u5E93\u5B58: %2\n\u8BF7\u8F93\u5165\u6570\u91CF:")
                    .arg(QString::fromStdString(m->tradeName))
                    .arg(m->stock),
                1, 1, 99999, 1, &ok);
            if (!ok) return;

            if (qty > m->stock) {
                QMessageBox::StandardButton reply = QMessageBox::warning(&prescDlg,
                    QString::fromUtf8("\u5E93\u5B58\u4E0D\u8DB3"),
                    QString::fromUtf8("\u5E93\u5B58\u4E0D\u8DB3\uFF01\u5F53\u524D\u5E93\u5B58\u4E3A %1\uFF0C\u60A8\u9700\u8981 %2\u3002\n\u662F\u5426\u4ECD\u7136\u6DFB\u52A0\u6B64\u836F\u54C1\uFF1F")
                        .arg(m->stock).arg(qty),
                    QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::No) return;
            }

            double unitPrice = 15.0;
            bool priceOk = false;
            double p = QInputDialog::getDouble(&prescDlg,
                QString::fromUtf8("\u5355\u4EF7"),
                QString::fromUtf8("\u836F\u54C1: %1\n\u8BF7\u8F93\u5165\u5355\u4EF7:")
                    .arg(QString::fromStdString(m->tradeName)),
                15.0, 0.01, 99999.99, 2, &priceOk);
            if (priceOk) unitPrice = round2(p);

            SelectedItem si;
            si.medID = medID;
            si.qty = qty;
            si.price = unitPrice;
            selectedItems.append(si);
            refreshSelectedTable();
        });

        connect(confirmBtn, &QPushButton::clicked, [&]() {
            if (selectedItems.isEmpty()) {
                QMessageBox::warning(&prescDlg,
                    QString::fromUtf8("\u63D0\u793A"),
                    QString::fromUtf8("\u8BF7\u81F3\u5C11\u6DFB\u52A0\u4E00\u79CD\u836F\u54C1\u3002"));
                return;
            }
            PrescriptionNode* pr = new PrescriptionNode();
            pr->prescID = generatePrescID();
            pr->consultID = consultID;
            pr->patientID = patientID;
            pr->doctorID = m_doctorID;
            pr->prescDay = weekday;
            pr->status = 0;
            pr->totalAmount = 0.0;

            double total = 0.0;
            for (const SelectedItem& si : selectedItems) {
                PrescMedicineNode* pm = new PrescMedicineNode();
                pm->prescID = pr->prescID;
                pm->medID = si.medID;
                pm->quantity = si.qty;
                pm->unitPrice = si.price;
                insertPrescMedicineTail(prescMedHead, pm);
                total = round2(total + round2(si.qty * si.price));
            }
            pr->totalAmount = total;
            insertPrescriptionTail(prescHead, pr);

            QMessageBox::information(&prescDlg,
                QString::fromUtf8("\u6210\u529F"),
                QString::fromUtf8("\u5904\u65B9\u5F00\u5177\u6210\u529F\uFF01\n\u5904\u65B9ID: %1\n\u603B\u91D1\u989D: \u00A5%2")
                    .arg(QString::fromStdString(pr->prescID))
                    .arg(total, 0, 'f', 2));
            prescDlg.accept();
        });

        connect(cancelPrescBtn, &QPushButton::clicked, &prescDlg, &QDialog::reject);
        prescDlg.exec();
    });

    // --- 完成看诊 ---
    connect(btnComplete, &QPushButton::clicked, [&]() {
        QString complaint = complaintEdit->toPlainText().trimmed();
        QString diagnosis = diagnosisEdit->toPlainText().trimmed();

        if (complaint.isEmpty()) {
            QMessageBox::warning(&dlg,
                QString::fromUtf8("\u63D0\u793A"),
                QString::fromUtf8("\u8BF7\u8F93\u5165\u4E3B\u8BC9\u3002"));
            return;
        }
        if (diagnosis.isEmpty()) {
            QMessageBox::warning(&dlg,
                QString::fromUtf8("\u63D0\u793A"),
                QString::fromUtf8("\u8BF7\u8F93\u5165\u8BCA\u65AD\u3002"));
            return;
        }

        // Create ConsultationNode
        ConsultationNode* cs = new ConsultationNode();
        cs->consultID = consultID;
        cs->regID = reg ? reg->regID : "";
        cs->patientID = patientID;
        cs->doctorID = m_doctorID;
        cs->complaint = complaint.toStdString();
        cs->diagnosis = diagnosis.toStdString();
        cs->consultDay = weekday;
        cs->status = STATUS_VALID;
        insertConsultationTail(consultHead, cs);

        // Update registration status
        if (reg) {
            reg->status = STATUS_SEEN;
        }

        consultCreated = true;
        QMessageBox::information(&dlg,
            QString::fromUtf8("\u6210\u529F"),
            QString::fromUtf8("\u770B\u8BCA\u5B8C\u6210\uFF01\n\u5C31\u8BCAID: %1")
                .arg(QString::fromStdString(consultID)));
        dlg.accept();
    });

    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);

    dlg.exec();
    refreshAll();
}

// ======================================================================
// Tab 2: Outpatient Work
// ======================================================================

void DoctorWidget::refreshOutpatientWork() {
    outpatientPatientList->clear();

    QSet<QString> uniquePatients;
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->doctorID == m_doctorID) {
            QString pid = QString::fromStdString(c->patientID);
            if (!uniquePatients.contains(pid)) {
                uniquePatients.insert(pid);
                PatientNode* p = findPatientByID(patientHead, c->patientID);
                QString display = p
                    ? QString::fromUtf8("%1 - %2").arg(pid).arg(
                        QString::fromStdString(p->name))
                    : pid;
                QListWidgetItem* item = new QListWidgetItem(display);
                item->setData(Qt::UserRole, pid);
                outpatientPatientList->addItem(item);
            }
        }
        c = c->next;
    }
    // Also check registrations
    RegistrationNode* r = regHead;
    while (r) {
        if (r->doctorID == m_doctorID && r->status == STATUS_SEEN) {
            QString pid = QString::fromStdString(r->patientID);
            if (!uniquePatients.contains(pid)) {
                uniquePatients.insert(pid);
                PatientNode* p = findPatientByID(patientHead, r->patientID);
                QString display = p
                    ? QString::fromUtf8("%1 - %2 (\u5DF2\u6302\u53F7)").arg(pid).arg(
                        QString::fromStdString(p->name))
                    : pid;
                QListWidgetItem* item = new QListWidgetItem(display);
                item->setData(Qt::UserRole, pid);
                outpatientPatientList->addItem(item);
            }
        }
        r = r->next;
    }
}

void DoctorWidget::onOutpatientPatientSelected() {
    QListWidgetItem* item = outpatientPatientList->currentItem();
    if (!item) return;

    QString patientID = item->data(Qt::UserRole).toString();
    m_outpatientCurrentPatientID = patientID.toStdString();

    PatientNode* p = findPatientByID(patientHead, m_outpatientCurrentPatientID);
    outpatientInfoLabel->setText(
        QString::fromUtf8("\u60A3\u8005: %1 (%2)  \u5E74\u9F84: %3")
            .arg(p ? QString::fromStdString(p->name)
                   : QString::fromUtf8("\u672A\u77E5"))
            .arg(QString::fromStdString(m_outpatientCurrentPatientID))
            .arg(p ? p->age : 0));

    // Find latest valid consultation for this patient by this doctor
    m_outpatientCurrentConsultID.clear();
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->patientID == m_outpatientCurrentPatientID
            && c->doctorID == m_doctorID
            && c->status == STATUS_VALID) {
            m_outpatientCurrentConsultID = c->consultID;
        }
        c = c->next;
    }

    // Populate records table
    outpatientRecordTable->setRowCount(0);
    int row = 0;

    // Consultations
    c = consultHead;
    while (c) {
        if (c->patientID == m_outpatientCurrentPatientID
            && c->doctorID == m_doctorID) {
            outpatientRecordTable->insertRow(row);
            outpatientRecordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u770B\u8BCA")));
            outpatientRecordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(c->consultID)));
            outpatientRecordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(c->diagnosis)));
            outpatientRecordTable->setItem(row, 3, new QTableWidgetItem(
                QString("Day%1").arg(c->consultDay)));
            const char* statusStr = (c->status == STATUS_CANCELLED)
                ? "\u5DF2\u64A4\u9500" : "\u6709\u6548";
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(statusStr));
            si->setForeground(c->status == STATUS_CANCELLED
                ? QColor("red") : QColor("green"));
            outpatientRecordTable->setItem(row, 4, si);
            row++;
        }
        c = c->next;
    }

    // Prescriptions
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->patientID == m_outpatientCurrentPatientID
            && pr->doctorID == m_doctorID) {
            outpatientRecordTable->insertRow(row);
            outpatientRecordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u5904\u65B9")));
            outpatientRecordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(pr->prescID)));
            outpatientRecordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromUtf8("\u91D1\u989D: \u00A5%1")
                    .arg(pr->totalAmount, 0, 'f', 2)));
            outpatientRecordTable->setItem(row, 3, new QTableWidgetItem(
                QString("Day%1").arg(pr->prescDay)));
            QString pStatus;
            if (pr->status == 0) pStatus = QString::fromUtf8("\u5F85\u7F34\u8D39");
            else if (pr->status == 1) pStatus = QString::fromUtf8("\u5DF2\u64A4\u9500");
            else if (pr->status == 2) pStatus = QString::fromUtf8("\u5DF2\u7F34\u8D39");
            else pStatus = QString::fromUtf8("\u672A\u77E5");
            QTableWidgetItem* psi = new QTableWidgetItem(pStatus);
            psi->setForeground(pr->status == 1 ? QColor("red") : QColor("green"));
            outpatientRecordTable->setItem(row, 4, psi);
            row++;
        }
        pr = pr->next;
    }

    // Examinations
    ExaminationNode* e = examHead;
    while (e) {
        if (e->patientID == m_outpatientCurrentPatientID
            && !e->consultID.empty()) {
            ConsultationNode* ec = findConsultationByID(consultHead, e->consultID);
            if (ec && ec->doctorID == m_doctorID) {
                outpatientRecordTable->insertRow(row);
                outpatientRecordTable->setItem(row, 0, new QTableWidgetItem(
                    QString::fromUtf8("\u68C0\u67E5")));
                outpatientRecordTable->setItem(row, 1, new QTableWidgetItem(
                    QString::fromStdString(e->examID)));
                outpatientRecordTable->setItem(row, 2, new QTableWidgetItem(
                    QString::fromStdString(e->itemName)));
                outpatientRecordTable->setItem(row, 3, new QTableWidgetItem(
                    QString("Day%1").arg(e->examDay)));
                const char* s = (e->status == STATUS_CANCELLED)
                    ? "\u5DF2\u64A4\u9500" : "\u6709\u6548";
                QTableWidgetItem* esi = new QTableWidgetItem(
                    QString::fromUtf8(s));
                esi->setForeground(e->status == STATUS_CANCELLED
                    ? QColor("red") : QColor("green"));
                outpatientRecordTable->setItem(row, 4, esi);
                row++;
            }
        }
        e = e->next;
    }

    outpatientRecordTable->resizeColumnsToContents();
}

void DoctorWidget::onPrescribeMedicine() {
    std::string patientID;
    std::string consultID;

    if (m_tabWidget->currentIndex() == 1) {
        patientID = m_outpatientCurrentPatientID;
        consultID = m_outpatientCurrentConsultID;
    }

    if (patientID.empty()) {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u8BF7\u5148\u9009\u62E9\u4E00\u4E2A\u60A3\u8005\u3002"));
        return;
    }

    // If no consultation exists, auto-create one
    if (consultID.empty()) {
        consultID = generateConsultID();
        ConsultationNode* cs = new ConsultationNode();
        cs->consultID = consultID;
        cs->regID = "";
        cs->patientID = patientID;
        cs->doctorID = m_doctorID;
        cs->complaint = "(\u95E8\u8BCA\u5DE5\u4F5C\u76F4\u63A5\u5F00\u836F)";
        cs->diagnosis = "";
        cs->consultDay = weekday;
        cs->status = STATUS_VALID;
        insertConsultationTail(consultHead, cs);
        m_outpatientCurrentConsultID = consultID;
    }

    PatientNode* patient = findPatientByID(patientHead, patientID);
    if (!patient) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u9519\u8BEF"),
            QString::fromUtf8("\u60A3\u8005\u4FE1\u606F\u4E0D\u5B58\u5728\u3002"));
        return;
    }

    // Medical selection dialog
    QDialog prescDlg(this);
    prescDlg.setWindowTitle(QString::fromUtf8("\u5F00\u5177\u5904\u65B9"));
    prescDlg.resize(750, 550);
    QVBoxLayout* prescLayout = new QVBoxLayout(&prescDlg);

    QLabel* prescInfoLabel = new QLabel(
        QString::fromUtf8("\u60A3\u8005: %1 (%2)")
            .arg(QString::fromStdString(patient->name))
            .arg(QString::fromStdString(patientID)));
    prescInfoLabel->setStyleSheet("font-weight: bold;");
    prescLayout->addWidget(prescInfoLabel);

    QHBoxLayout* searchRow = new QHBoxLayout();
    searchRow->addWidget(new QLabel(
        QString::fromUtf8("\u641C\u7D22\u836F\u54C1:")));
    QLineEdit* medSearchEdit = new QLineEdit();
    medSearchEdit->setPlaceholderText(
        QString::fromUtf8("\u8F93\u5165\u836F\u54C1\u540D\u79F0\u8FC7\u6EE4..."));
    searchRow->addWidget(medSearchEdit);
    prescLayout->addLayout(searchRow);

    QSplitter* prescSplitter = new QSplitter(Qt::Horizontal);
    QWidget* medListWidget = new QWidget();
    QVBoxLayout* medListLayout = new QVBoxLayout(medListWidget);
    medListLayout->addWidget(new QLabel(
        QString::fromUtf8("\u836F\u54C1\u5E93:")));
    QTableWidget* medTable = new QTableWidget();
    medTable->setColumnCount(3);
    medTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u836F\u54C1ID"),
        QString::fromUtf8("\u5546\u54C1\u540D"),
        QString::fromUtf8("\u5E93\u5B58")
    });
    medTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    medTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    medTable->horizontalHeader()->setStretchLastSection(true);
    medListLayout->addWidget(medTable);

    QWidget* selectedWidget = new QWidget();
    QVBoxLayout* selectedLayout = new QVBoxLayout(selectedWidget);
    selectedLayout->addWidget(new QLabel(
        QString::fromUtf8("\u5DF2\u9009\u836F\u54C1:")));
    QTableWidget* selectedTable = new QTableWidget();
    selectedTable->setColumnCount(4);
    selectedTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u836F\u54C1\u540D"),
        QString::fromUtf8("\u6570\u91CF"),
        QString::fromUtf8("\u5355\u4EF7"),
        QString::fromUtf8("\u5C0F\u8BA1")
    });
    selectedTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    selectedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    selectedTable->horizontalHeader()->setStretchLastSection(true);
    selectedLayout->addWidget(selectedTable);

    prescSplitter->addWidget(medListWidget);
    prescSplitter->addWidget(selectedWidget);
    prescSplitter->setStretchFactor(0, 2);
    prescSplitter->setStretchFactor(1, 1);
    prescLayout->addWidget(prescSplitter);

    QHBoxLayout* bottomRow = new QHBoxLayout();
    QLabel* totalLabel = new QLabel(
        QString::fromUtf8("\u603B\u91D1\u989D: \u00A50.00"));
    totalLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    bottomRow->addWidget(totalLabel);
    bottomRow->addStretch();
    QPushButton* confirmBtn = new QPushButton(
        QString::fromUtf8("\u786E\u8BA4\u5F00\u5177\u5904\u65B9"));
    confirmBtn->setMinimumHeight(36);
    bottomRow->addWidget(confirmBtn);
    QPushButton* cancelBtn = new QPushButton(
        QString::fromUtf8("\u53D6\u6D88"));
    bottomRow->addWidget(cancelBtn);
    prescLayout->addLayout(bottomRow);

    struct SelectedItem { std::string medID; int qty; double price; };
    QVector<SelectedItem> selectedItems;

    auto populateMedTable = [&](const QString& filter) {
        medTable->setRowCount(0);
        int r = 0;
        MedicineNode* m = medHead;
        while (m) {
            QString tn = QString::fromStdString(m->tradeName);
            if (filter.isEmpty() || tn.contains(filter, Qt::CaseInsensitive)) {
                medTable->insertRow(r);
                medTable->setItem(r, 0, new QTableWidgetItem(
                    QString::fromStdString(m->medID)));
                medTable->setItem(r, 1, new QTableWidgetItem(tn));
                medTable->setItem(r, 2, new QTableWidgetItem(
                    QString::number(m->stock)));
                r++;
            }
            m = m->next;
        }
        medTable->resizeColumnsToContents();
    };
    populateMedTable("");

    auto refreshSel = [&]() {
        selectedTable->setRowCount(0);
        double total = 0.0;
        for (int i = 0; i < selectedItems.size(); i++) {
            const SelectedItem& si = selectedItems[i];
            MedicineNode* m = findMedicineByID(medHead, si.medID);
            selectedTable->insertRow(i);
            selectedTable->setItem(i, 0, new QTableWidgetItem(
                m ? QString::fromStdString(m->tradeName)
                  : QString::fromStdString(si.medID)));
            selectedTable->setItem(i, 1, new QTableWidgetItem(
                QString::number(si.qty)));
            selectedTable->setItem(i, 2, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(si.price, 0, 'f', 2)));
            double sub = round2(si.qty * si.price);
            selectedTable->setItem(i, 3, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(sub, 0, 'f', 2)));
            total = round2(total + sub);
        }
        totalLabel->setText(
            QString::fromUtf8("\u603B\u91D1\u989D: \u00A5%1").arg(total, 0, 'f', 2));
    };

    connect(medSearchEdit, &QLineEdit::textChanged, [&](const QString& t) {
        populateMedTable(t);
    });

    connect(medTable, &QTableWidget::cellDoubleClicked, [&](int row, int col) {
        Q_UNUSED(col);
        QTableWidgetItem* idItem = medTable->item(row, 0);
        if (!idItem) return;
        std::string medID = idItem->text().toStdString();
        MedicineNode* m = findMedicineByID(medHead, medID);
        if (!m) return;

        bool ok = false;
        int qty = QInputDialog::getInt(&prescDlg,
            QString::fromUtf8("\u6570\u91CF"),
            QString::fromUtf8("\u836F\u54C1: %1\n\u5E93\u5B58: %2\n\u8BF7\u8F93\u5165\u6570\u91CF:")
                .arg(QString::fromStdString(m->tradeName)).arg(m->stock),
            1, 1, 99999, 1, &ok);
        if (!ok) return;
        if (qty > m->stock) {
            QMessageBox::StandardButton reply = QMessageBox::warning(&prescDlg,
                QString::fromUtf8("\u5E93\u5B58\u4E0D\u8DB3"),
                QString::fromUtf8("\u5E93\u5B58\u4E0D\u8DB3\uFF01\u5F53\u524D\u5E93\u5B58\u4E3A %1\uFF0C\u60A8\u9700\u8981 %2\u3002\n\u662F\u5426\u4ECD\u7136\u6DFB\u52A0\u6B64\u836F\u54C1\uFF1F")
                    .arg(m->stock).arg(qty),
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) return;
        }

        double unitPrice = 15.0;
        bool pOk = false;
        double pr = QInputDialog::getDouble(&prescDlg,
            QString::fromUtf8("\u5355\u4EF7"),
            QString::fromUtf8("\u836F\u54C1: %1\n\u8BF7\u8F93\u5165\u5355\u4EF7:")
                .arg(QString::fromStdString(m->tradeName)),
            15.0, 0.01, 99999.99, 2, &pOk);
        if (pOk) unitPrice = round2(pr);

        SelectedItem si;
        si.medID = medID;
        si.qty = qty;
        si.price = unitPrice;
        selectedItems.append(si);
        refreshSel();
    });

    connect(confirmBtn, &QPushButton::clicked, [&]() {
        if (selectedItems.isEmpty()) {
            QMessageBox::warning(&prescDlg,
                QString::fromUtf8("\u63D0\u793A"),
                QString::fromUtf8("\u8BF7\u81F3\u5C11\u6DFB\u52A0\u4E00\u79CD\u836F\u54C1\u3002"));
            return;
        }
        PrescriptionNode* prn = new PrescriptionNode();
        prn->prescID = generatePrescID();
        prn->consultID = consultID;
        prn->patientID = patientID;
        prn->doctorID = m_doctorID;
        prn->prescDay = weekday;
        prn->status = 0;
        prn->totalAmount = 0.0;

        double total = 0.0;
        for (const SelectedItem& si : selectedItems) {
            PrescMedicineNode* pm = new PrescMedicineNode();
            pm->prescID = prn->prescID;
            pm->medID = si.medID;
            pm->quantity = si.qty;
            pm->unitPrice = si.price;
            insertPrescMedicineTail(prescMedHead, pm);
            total = round2(total + round2(si.qty * si.price));
        }
        prn->totalAmount = total;
        insertPrescriptionTail(prescHead, prn);

        QMessageBox::information(&prescDlg,
            QString::fromUtf8("\u6210\u529F"),
            QString::fromUtf8("\u5904\u65B9\u5F00\u5177\u6210\u529F\uFF01\n\u5904\u65B9ID: %1\n\u603B\u91D1\u989D: \u00A5%2")
                .arg(QString::fromStdString(prn->prescID))
                .arg(total, 0, 'f', 2));
        prescDlg.accept();
    });

    connect(cancelBtn, &QPushButton::clicked, &prescDlg, &QDialog::reject);

    if (prescDlg.exec() == QDialog::Accepted) {
        onOutpatientPatientSelected();
    }
}

void DoctorWidget::onOrderExam() {
    std::string patientID;
    std::string consultID;

    if (m_tabWidget->currentIndex() == 1) {
        patientID = m_outpatientCurrentPatientID;
        consultID = m_outpatientCurrentConsultID;
    }

    if (patientID.empty()) {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u8BF7\u5148\u9009\u62E9\u4E00\u4E2A\u60A3\u8005\u3002"));
        return;
    }

    QDialog examDlg(this);
    examDlg.setWindowTitle(QString::fromUtf8("\u5F00\u5177\u68C0\u67E5"));
    QFormLayout* examForm = new QFormLayout(&examDlg);
    QLineEdit* nameEdit = new QLineEdit();
    QDoubleSpinBox* costSpin = new QDoubleSpinBox();
    costSpin->setRange(0.0, 999999.99);
    costSpin->setDecimals(2);
    costSpin->setPrefix(QString::fromUtf8("\u00A5 "));
    examForm->addRow(QString::fromUtf8("\u68C0\u67E5\u540D\u79F0:"), nameEdit);
    examForm->addRow(QString::fromUtf8("\u8D39\u7528:"), costSpin);
    QDialogButtonBox* examBtns = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    examBtns->button(QDialogButtonBox::Ok)->setText(
        QString::fromUtf8("\u786E\u5B9A"));
    examBtns->button(QDialogButtonBox::Cancel)->setText(
        QString::fromUtf8("\u53D6\u6D88"));
    examForm->addRow(examBtns);
    connect(examBtns, &QDialogButtonBox::accepted, &examDlg, &QDialog::accept);
    connect(examBtns, &QDialogButtonBox::rejected, &examDlg, &QDialog::reject);

    if (examDlg.exec() == QDialog::Accepted) {
        QString itemName = nameEdit->text().trimmed();
        if (itemName.isEmpty()) {
            QMessageBox::warning(this,
                QString::fromUtf8("\u9519\u8BEF"),
                QString::fromUtf8("\u68C0\u67E5\u540D\u79F0\u4E0D\u80FD\u4E3A\u7A7A\u3002"));
            return;
        }

        if (consultID.empty()) {
            consultID = generateConsultID();
            ConsultationNode* cs = new ConsultationNode();
            cs->consultID = consultID;
            cs->regID = "";
            cs->patientID = patientID;
            cs->doctorID = m_doctorID;
            cs->complaint = "(\u95E8\u8BCA\u5DE5\u4F5C\u76F4\u63A5\u5F00\u68C0\u67E5)";
            cs->diagnosis = "";
            cs->consultDay = weekday;
            cs->status = STATUS_VALID;
            insertConsultationTail(consultHead, cs);
            m_outpatientCurrentConsultID = consultID;
        }

        ExaminationNode* exam = new ExaminationNode();
        exam->examID = generateExamID();
        exam->consultID = consultID;
        exam->patientID = patientID;
        exam->itemName = itemName.toStdString();
        exam->cost = round2(costSpin->value());
        exam->examDay = weekday;
        exam->status = STATUS_VALID;
        insertExaminationTail(examHead, exam);

        QMessageBox::information(this,
            QString::fromUtf8("\u6210\u529F"),
            QString::fromUtf8("\u68C0\u67E5\u5F00\u5177\u6210\u529F\uFF01\n\u68C0\u67E5ID: %1\n\u9879\u76EE: %2\n\u8D39\u7528: \u00A5%3")
                .arg(QString::fromStdString(exam->examID))
                .arg(itemName)
                .arg(exam->cost, 0, 'f', 2));
        onOutpatientPatientSelected();
    }
}

void DoctorWidget::onCompleteConsult() {
    QMessageBox::information(this,
        QString::fromUtf8("\u63D0\u793A"),
        QString::fromUtf8("\u770B\u8BCA\u5DF2\u5B8C\u6210\u3002\u5982\u9700\u65B0\u5EFA\u770B\u8BCA\uFF0C\u8BF7\u4F7F\u7528\u201C\u63A5\u8BCA\u201D\u529F\u80FD\u3002"));
}

// ======================================================================
// Tab 3: My Inpatients
// ======================================================================

void DoctorWidget::refreshMyInpatients() {
    inpatientTable->setRowCount(0);
    int row = 0;
    HospitalizationNode* h = hospHead;
    while (h) {
        if (h->status == 0) {
            bool isMyPatient = false;
            for (int i = 0; i < h->doctorCount; i++) {
                if (h->doctorIDs[i] == m_doctorID) {
                    isMyPatient = true;
                    break;
                }
            }
            if (isMyPatient) {
                inpatientTable->insertRow(row);
                inpatientTable->setItem(row, 0, new QTableWidgetItem(
                    QString::fromStdString(h->hospID)));
                inpatientTable->setItem(row, 1, new QTableWidgetItem(
                    QString::fromStdString(h->patientID)));
                PatientNode* p = findPatientByID(patientHead, h->patientID);
                inpatientTable->setItem(row, 2, new QTableWidgetItem(
                    p ? QString::fromStdString(p->name)
                      : QString::fromUtf8("\u672A\u77E5")));
                inpatientTable->setItem(row, 3, new QTableWidgetItem(
                    QString::fromStdString(h->wardID)));
                inpatientTable->setItem(row, 4, new QTableWidgetItem(
                    QString::number(h->bedNo + 1)));
                int daysIn = (int)(weekday - h->admitDay);
                inpatientTable->setItem(row, 5, new QTableWidgetItem(
                    QString::number(daysIn)));
                QTableWidgetItem* depositItem = new QTableWidgetItem(
                    QString::fromUtf8("\u00A5%1").arg(h->deposit, 0, 'f', 2));
                depositItem->setForeground(h->deposit < 0
                    ? QColor("red") : QColor("green"));
                inpatientTable->setItem(row, 6, depositItem);
                row++;
            }
        }
        h = h->next;
    }
    if (row == 0) {
        inpatientTable->insertRow(0);
        QTableWidgetItem* emptyItem = new QTableWidgetItem(
            QString::fromUtf8("\u6682\u65E0\u60A8\u7684\u4F4F\u9662\u60A3\u8005"));
        emptyItem->setTextAlignment(Qt::AlignCenter);
        inpatientTable->setSpan(0, 0, 1, 7);
        inpatientTable->setItem(0, 0, emptyItem);
    }
    inpatientTable->resizeColumnsToContents();
}

void DoctorWidget::onInpatientDoubleClicked(int row, int col) {
    Q_UNUSED(row);
    Q_UNUSED(col);
    onViewInpatient();
}

void DoctorWidget::onViewInpatient() {
    int row = inpatientTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u8BF7\u5148\u9009\u62E9\u4E00\u4F4D\u4F4F\u9662\u60A3\u8005\u3002"));
        return;
    }
    QTableWidgetItem* item = inpatientTable->item(row, 1);
    if (!item) return;
    std::string patientID = item->text().toStdString();

    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
    if (!hosp) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u9519\u8BEF"),
            QString::fromUtf8("\u672A\u627E\u5230\u8BE5\u60A3\u8005\u7684\u4F4F\u9662\u8BB0\u5F55\u3002"));
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, patientID);

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("\u4F4F\u9662\u60A3\u8005\u8BE6\u60C5"));
    dlg.resize(720, 620);
    QVBoxLayout* dlgLayout = new QVBoxLayout(&dlg);

    // Patient info group
    QGroupBox* infoGroup = new QGroupBox(
        QString::fromUtf8("\u60A3\u8005\u57FA\u672C\u4FE1\u606F"));
    QFormLayout* infoForm = new QFormLayout(infoGroup);
    if (patient) {
        infoForm->addRow(QString::fromUtf8("\u59D3\u540D:"),
            new QLabel(QString::fromStdString(patient->name)));
        infoForm->addRow(QString::fromUtf8("\u60A3\u8005ID:"),
            new QLabel(QString::fromStdString(patient->patientID)));
        infoForm->addRow(QString::fromUtf8("\u5E74\u9F84:"),
            new QLabel(QString::number(patient->age)));
        infoForm->addRow(QString::fromUtf8("\u8054\u7CFB\u65B9\u5F0F:"),
            new QLabel(QString::fromStdString(patient->contact)));
    }
    infoForm->addRow(QString::fromUtf8("\u4F4F\u9662ID:"),
        new QLabel(QString::fromStdString(hosp->hospID)));
    infoForm->addRow(QString::fromUtf8("\u75C5\u623F:"),
        new QLabel(QString::fromStdString(hosp->wardID)));
    infoForm->addRow(QString::fromUtf8("\u5E8A\u4F4D:"),
        new QLabel(QString::number(hosp->bedNo + 1)));
    infoForm->addRow(QString::fromUtf8("\u5165\u9662\u65E5\u671F:"),
        new QLabel(QString("Day%1").arg(hosp->admitDay)));
    infoForm->addRow(QString::fromUtf8("\u5DF2\u4F4F\u5929\u6570:"),
        new QLabel(QString::number(weekday - hosp->admitDay)));
    QLabel* depLabel = new QLabel(
        QString::fromUtf8("\u00A5%1").arg(hosp->deposit, 0, 'f', 2));
    depLabel->setStyleSheet(hosp->deposit < 0
        ? "color: red; font-weight: bold;" : "color: green; font-weight: bold;");
    infoForm->addRow(QString::fromUtf8("\u62BC\u91D1\u4F59\u989D:"), depLabel);
    dlgLayout->addWidget(infoGroup);

    // Records in tabs
    QTabWidget* recordTabs = new QTabWidget();

    // Consultations
    QTableWidget* consTable = new QTableWidget();
    consTable->setColumnCount(5);
    consTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u5C31\u8BCAID"),
        QString::fromUtf8("\u4E3B\u8BC9"),
        QString::fromUtf8("\u8BCA\u65AD"),
        QString::fromUtf8("\u65E5\u671F"),
        QString::fromUtf8("\u72B6\u6001")
    });
    consTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    consTable->horizontalHeader()->setStretchLastSection(true);
    int cr = 0;
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->patientID == patientID) {
            consTable->insertRow(cr);
            consTable->setItem(cr, 0, new QTableWidgetItem(
                QString::fromStdString(c->consultID)));
            consTable->setItem(cr, 1, new QTableWidgetItem(
                QString::fromStdString(c->complaint)));
            consTable->setItem(cr, 2, new QTableWidgetItem(
                QString::fromStdString(c->diagnosis)));
            consTable->setItem(cr, 3, new QTableWidgetItem(
                QString("Day%1").arg(c->consultDay)));
            const char* s = (c->status == STATUS_CANCELLED)
                ? "\u5DF2\u64A4\u9500" : "\u6709\u6548";
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(s));
            si->setForeground(c->status == STATUS_CANCELLED
                ? QColor("red") : QColor("green"));
            consTable->setItem(cr, 4, si);
            cr++;
        }
        c = c->next;
    }
    consTable->resizeColumnsToContents();
    recordTabs->addTab(consTable,
        QString::fromUtf8("\u770B\u8BCA\u8BB0\u5F55"));

    // Prescriptions
    QTableWidget* prescTable = new QTableWidget();
    prescTable->setColumnCount(5);
    prescTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u5904\u65B9ID"),
        QString::fromUtf8("\u91D1\u989D"),
        QString::fromUtf8("\u65E5\u671F"),
        QString::fromUtf8("\u72B6\u6001"),
        QString::fromUtf8("\u836F\u54C1")
    });
    prescTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    prescTable->horizontalHeader()->setStretchLastSection(true);
    int pr = 0;
    PrescriptionNode* pn = prescHead;
    while (pn) {
        if (pn->patientID == patientID) {
            prescTable->insertRow(pr);
            prescTable->setItem(pr, 0, new QTableWidgetItem(
                QString::fromStdString(pn->prescID)));
            prescTable->setItem(pr, 1, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(pn->totalAmount, 0, 'f', 2)));
            prescTable->setItem(pr, 2, new QTableWidgetItem(
                QString("Day%1").arg(pn->prescDay)));
            QString pStatus;
            if (pn->status == 0) pStatus = QString::fromUtf8("\u5F85\u7F34\u8D39");
            else if (pn->status == 1) pStatus = QString::fromUtf8("\u5DF2\u64A4\u9500");
            else if (pn->status == 2) pStatus = QString::fromUtf8("\u5DF2\u7F34\u8D39");
            else pStatus = QString::fromUtf8("\u672A\u77E5");
            QTableWidgetItem* psi = new QTableWidgetItem(pStatus);
            psi->setForeground(pn->status == 1 ? QColor("red") : QColor("green"));
            prescTable->setItem(pr, 3, psi);

            // Medicine details
            QString medDetails;
            PrescMedicineNode* pm = prescMedHead;
            while (pm) {
                if (pm->prescID == pn->prescID) {
                    MedicineNode* m = findMedicineByID(medHead, pm->medID);
                    if (!medDetails.isEmpty()) medDetails += "; ";
                    medDetails += QString("%1 x%2")
                        .arg(m ? QString::fromStdString(m->tradeName)
                               : QString::fromStdString(pm->medID))
                        .arg(pm->quantity);
                }
                pm = pm->next;
            }
            prescTable->setItem(pr, 4, new QTableWidgetItem(medDetails));
            pr++;
        }
        pn = pn->next;
    }
    prescTable->resizeColumnsToContents();
    recordTabs->addTab(prescTable,
        QString::fromUtf8("\u5904\u65B9\u8BB0\u5F55"));

    // Examinations
    QTableWidget* examTable = new QTableWidget();
    examTable->setColumnCount(5);
    examTable->setHorizontalHeaderLabels({
        QString::fromUtf8("\u68C0\u67E5ID"),
        QString::fromUtf8("\u9879\u76EE"),
        QString::fromUtf8("\u8D39\u7528"),
        QString::fromUtf8("\u65E5\u671F"),
        QString::fromUtf8("\u72B6\u6001")
    });
    examTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    examTable->horizontalHeader()->setStretchLastSection(true);
    int er = 0;
    ExaminationNode* en = examHead;
    while (en) {
        if (en->patientID == patientID) {
            examTable->insertRow(er);
            examTable->setItem(er, 0, new QTableWidgetItem(
                QString::fromStdString(en->examID)));
            examTable->setItem(er, 1, new QTableWidgetItem(
                QString::fromStdString(en->itemName)));
            examTable->setItem(er, 2, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(en->cost, 0, 'f', 2)));
            examTable->setItem(er, 3, new QTableWidgetItem(
                QString("Day%1").arg(en->examDay)));
            const char* s = (en->status == STATUS_CANCELLED)
                ? "\u5DF2\u64A4\u9500" : "\u6709\u6548";
            QTableWidgetItem* esi = new QTableWidgetItem(
                QString::fromUtf8(s));
            esi->setForeground(en->status == STATUS_CANCELLED
                ? QColor("red") : QColor("green"));
            examTable->setItem(er, 4, esi);
            er++;
        }
        en = en->next;
    }
    examTable->resizeColumnsToContents();
    recordTabs->addTab(examTable,
        QString::fromUtf8("\u68C0\u67E5\u8BB0\u5F55"));

    dlgLayout->addWidget(recordTabs);

    // Action buttons
    QHBoxLayout* actionBtns = new QHBoxLayout();
    QPushButton* btnExam2 = new QPushButton(
        QString::fromUtf8("\u5F00\u68C0\u67E5"));
    QPushButton* btnPresc2 = new QPushButton(
        QString::fromUtf8("\u5F00\u5904\u65B9"));
    QPushButton* btnClose = new QPushButton(
        QString::fromUtf8("\u5173\u95ED"));
    actionBtns->addWidget(btnExam2);
    actionBtns->addWidget(btnPresc2);
    actionBtns->addStretch();
    actionBtns->addWidget(btnClose);
    dlgLayout->addLayout(actionBtns);

    connect(btnExam2, &QPushButton::clicked, [&]() {
        QDialog examDlg(&dlg);
        examDlg.setWindowTitle(
            QString::fromUtf8("\u4E3A\u4F4F\u9662\u60A3\u8005\u5F00\u5177\u68C0\u67E5"));
        QFormLayout* ef = new QFormLayout(&examDlg);
        QLineEdit* ne = new QLineEdit();
        QDoubleSpinBox* cs = new QDoubleSpinBox();
        cs->setRange(0.0, 999999.99);
        cs->setDecimals(2);
        cs->setPrefix(QString::fromUtf8("\u00A5 "));
        ef->addRow(QString::fromUtf8("\u68C0\u67E5\u540D\u79F0:"), ne);
        ef->addRow(QString::fromUtf8("\u8D39\u7528:"), cs);
        QDialogButtonBox* eb = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        eb->button(QDialogButtonBox::Ok)->setText(
            QString::fromUtf8("\u786E\u5B9A"));
        eb->button(QDialogButtonBox::Cancel)->setText(
            QString::fromUtf8("\u53D6\u6D88"));
        ef->addRow(eb);
        connect(eb, &QDialogButtonBox::accepted, &examDlg, &QDialog::accept);
        connect(eb, &QDialogButtonBox::rejected, &examDlg, &QDialog::reject);

        if (examDlg.exec() == QDialog::Accepted) {
            QString in = ne->text().trimmed();
            if (in.isEmpty()) {
                QMessageBox::warning(&examDlg,
                    QString::fromUtf8("\u9519\u8BEF"),
                    QString::fromUtf8("\u68C0\u67E5\u540D\u79F0\u4E0D\u80FD\u4E3A\u7A7A\u3002"));
                return;
            }
            ExaminationNode* exam = new ExaminationNode();
            exam->examID = generateExamID();
            exam->consultID = "";
            exam->patientID = patientID;
            exam->itemName = in.toStdString();
            exam->cost = round2(cs->value());
            exam->examDay = weekday;
            exam->status = STATUS_VALID;
            insertExaminationTail(examHead, exam);
            QMessageBox::information(&dlg,
                QString::fromUtf8("\u6210\u529F"),
                QString::fromUtf8("\u68C0\u67E5\u5F00\u5177\u6210\u529F\uFF01\n\u68C0\u67E5ID: %1")
                    .arg(QString::fromStdString(exam->examID)));
            dlg.accept();
        }
    });

    connect(btnPresc2, &QPushButton::clicked, [&]() {
        QDialog pDlg(&dlg);
        pDlg.setWindowTitle(
            QString::fromUtf8("\u4E3A\u4F4F\u9662\u60A3\u8005\u5F00\u5177\u5904\u65B9"));
        pDlg.resize(750, 550);
        QVBoxLayout* pLayout = new QVBoxLayout(&pDlg);

        pLayout->addWidget(new QLabel(
            QString::fromUtf8("\u60A3\u8005: %1 (%2)")
                .arg(patient ? QString::fromStdString(patient->name)
                             : QString::fromUtf8("\u672A\u77E5"))
                .arg(QString::fromStdString(patientID))));

        QHBoxLayout* sRow = new QHBoxLayout();
        sRow->addWidget(new QLabel(
            QString::fromUtf8("\u641C\u7D22\u836F\u54C1:")));
        QLineEdit* sEdit = new QLineEdit();
        sEdit->setPlaceholderText(
            QString::fromUtf8("\u8F93\u5165\u836F\u54C1\u540D\u79F0\u8FC7\u6EE4..."));
        sRow->addWidget(sEdit);
        pLayout->addLayout(sRow);

        QSplitter* spl = new QSplitter(Qt::Horizontal);
        QWidget* mw = new QWidget();
        QVBoxLayout* ml = new QVBoxLayout(mw);
        ml->addWidget(new QLabel(
            QString::fromUtf8("\u836F\u54C1\u5E93:")));
        QTableWidget* mt = new QTableWidget();
        mt->setColumnCount(3);
        mt->setHorizontalHeaderLabels({
            QString::fromUtf8("\u836F\u54C1ID"),
            QString::fromUtf8("\u5546\u54C1\u540D"),
            QString::fromUtf8("\u5E93\u5B58")
        });
        mt->setSelectionBehavior(QAbstractItemView::SelectRows);
        mt->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mt->horizontalHeader()->setStretchLastSection(true);
        ml->addWidget(mt);

        QWidget* sw = new QWidget();
        QVBoxLayout* sl = new QVBoxLayout(sw);
        sl->addWidget(new QLabel(
            QString::fromUtf8("\u5DF2\u9009\u836F\u54C1:")));
        QTableWidget* st = new QTableWidget();
        st->setColumnCount(4);
        st->setHorizontalHeaderLabels({
            QString::fromUtf8("\u836F\u54C1\u540D"),
            QString::fromUtf8("\u6570\u91CF"),
            QString::fromUtf8("\u5355\u4EF7"),
            QString::fromUtf8("\u5C0F\u8BA1")
        });
        st->setSelectionBehavior(QAbstractItemView::SelectRows);
        st->setEditTriggers(QAbstractItemView::NoEditTriggers);
        st->horizontalHeader()->setStretchLastSection(true);
        sl->addWidget(st);

        spl->addWidget(mw);
        spl->addWidget(sw);
        spl->setStretchFactor(0, 2);
        spl->setStretchFactor(1, 1);
        pLayout->addWidget(spl);

        QHBoxLayout* bRow = new QHBoxLayout();
        QLabel* tLab = new QLabel(
            QString::fromUtf8("\u603B\u91D1\u989D: \u00A50.00"));
        tLab->setStyleSheet("font-size: 14px; font-weight: bold;");
        bRow->addWidget(tLab);
        bRow->addStretch();
        QPushButton* cBtn = new QPushButton(
            QString::fromUtf8("\u786E\u8BA4\u5F00\u5177\u5904\u65B9"));
        cBtn->setMinimumHeight(36);
        bRow->addWidget(cBtn);
        QPushButton* ccBtn = new QPushButton(
            QString::fromUtf8("\u53D6\u6D88"));
        bRow->addWidget(ccBtn);
        pLayout->addLayout(bRow);

        struct SI { std::string medID; int qty; double price; };
        QVector<SI> sel;

        auto pop = [&](const QString& f) {
            mt->setRowCount(0);
            int r = 0;
            MedicineNode* m = medHead;
            while (m) {
                QString tn = QString::fromStdString(m->tradeName);
                if (f.isEmpty() || tn.contains(f, Qt::CaseInsensitive)) {
                    mt->insertRow(r);
                    mt->setItem(r, 0, new QTableWidgetItem(
                        QString::fromStdString(m->medID)));
                    mt->setItem(r, 1, new QTableWidgetItem(tn));
                    mt->setItem(r, 2, new QTableWidgetItem(
                        QString::number(m->stock)));
                    r++;
                }
                m = m->next;
            }
            mt->resizeColumnsToContents();
        };
        pop("");

        auto ref = [&]() {
            st->setRowCount(0);
            double t = 0.0;
            for (int i = 0; i < sel.size(); i++) {
                const SI& si = sel[i];
                MedicineNode* m = findMedicineByID(medHead, si.medID);
                st->insertRow(i);
                st->setItem(i, 0, new QTableWidgetItem(
                    m ? QString::fromStdString(m->tradeName)
                      : QString::fromStdString(si.medID)));
                st->setItem(i, 1, new QTableWidgetItem(
                    QString::number(si.qty)));
                st->setItem(i, 2, new QTableWidgetItem(
                    QString::fromUtf8("\u00A5%1").arg(si.price, 0, 'f', 2)));
                double sub = round2(si.qty * si.price);
                st->setItem(i, 3, new QTableWidgetItem(
                    QString::fromUtf8("\u00A5%1").arg(sub, 0, 'f', 2)));
                t = round2(t + sub);
            }
            tLab->setText(
                QString::fromUtf8("\u603B\u91D1\u989D: \u00A5%1").arg(t, 0, 'f', 2));
        };

        connect(sEdit, &QLineEdit::textChanged, [&](const QString& t) { pop(t); });

        connect(mt, &QTableWidget::cellDoubleClicked, [&](int row, int col) {
            Q_UNUSED(col);
            QTableWidgetItem* idItem = mt->item(row, 0);
            if (!idItem) return;
            std::string medID = idItem->text().toStdString();
            MedicineNode* m = findMedicineByID(medHead, medID);
            if (!m) return;
            bool ok = false;
            int qty = QInputDialog::getInt(&pDlg,
                QString::fromUtf8("\u6570\u91CF"),
                QString::fromUtf8("\u836F\u54C1: %1\n\u5E93\u5B58: %2\n\u8BF7\u8F93\u5165\u6570\u91CF:")
                    .arg(QString::fromStdString(m->tradeName)).arg(m->stock),
                1, 1, 99999, 1, &ok);
            if (!ok) return;
            if (qty > m->stock) {
                QMessageBox::StandardButton reply = QMessageBox::warning(&pDlg,
                    QString::fromUtf8("\u5E93\u5B58\u4E0D\u8DB3"),
                    QString::fromUtf8("\u5E93\u5B58\u4E0D\u8DB3\uFF01\u5F53\u524D\u5E93\u5B58\u4E3A %1\uFF0C\u60A8\u9700\u8981 %2\u3002\n\u662F\u5426\u4ECD\u7136\u6DFB\u52A0\u6B64\u836F\u54C1\uFF1F")
                        .arg(m->stock).arg(qty),
                    QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::No) return;
            }
            double up = 15.0;
            bool pOk = false;
            double prVal = QInputDialog::getDouble(&pDlg,
                QString::fromUtf8("\u5355\u4EF7"),
                QString::fromUtf8("\u836F\u54C1: %1\n\u8BF7\u8F93\u5165\u5355\u4EF7:")
                    .arg(QString::fromStdString(m->tradeName)),
                15.0, 0.01, 99999.99, 2, &pOk);
            if (pOk) up = round2(prVal);
            SI s;
            s.medID = medID;
            s.qty = qty;
            s.price = up;
            sel.append(s);
            ref();
        });

        connect(cBtn, &QPushButton::clicked, [&]() {
            if (sel.isEmpty()) {
                QMessageBox::warning(&pDlg,
                    QString::fromUtf8("\u63D0\u793A"),
                    QString::fromUtf8("\u8BF7\u81F3\u5C11\u6DFB\u52A0\u4E00\u79CD\u836F\u54C1\u3002"));
                return;
            }
            PrescriptionNode* prn = new PrescriptionNode();
            prn->prescID = generatePrescID();
            prn->consultID = "";
            prn->patientID = patientID;
            prn->doctorID = m_doctorID;
            prn->prescDay = weekday;
            prn->status = 0;
            prn->totalAmount = 0.0;
            double total = 0.0;
            for (const SI& si : sel) {
                PrescMedicineNode* pm = new PrescMedicineNode();
                pm->prescID = prn->prescID;
                pm->medID = si.medID;
                pm->quantity = si.qty;
                pm->unitPrice = si.price;
                insertPrescMedicineTail(prescMedHead, pm);
                total = round2(total + round2(si.qty * si.price));
            }
            prn->totalAmount = total;
            insertPrescriptionTail(prescHead, prn);
            QMessageBox::information(&pDlg,
                QString::fromUtf8("\u6210\u529F"),
                QString::fromUtf8("\u5904\u65B9\u5F00\u5177\u6210\u529F\uFF01\n\u5904\u65B9ID: %1")
                    .arg(QString::fromStdString(prn->prescID)));
            pDlg.accept();
            dlg.accept();
        });

        connect(ccBtn, &QPushButton::clicked, &pDlg, &QDialog::reject);
        pDlg.exec();
    });

    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);

    dlg.exec();
    refreshMyInpatients();
}

void DoctorWidget::onPrescribeForInpatient() {
    int row = inpatientTable->currentRow();
    if (row < 0) {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u8BF7\u5148\u9009\u62E9\u4E00\u4F4D\u4F4F\u9662\u60A3\u8005\u3002"));
        return;
    }
    QTableWidgetItem* item = inpatientTable->item(row, 1);
    if (!item) return;
    std::string patientID = item->text().toStdString();

    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, patientID);
    if (!hosp) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u9519\u8BEF"),
            QString::fromUtf8("\u672A\u627E\u5230\u8BE5\u60A3\u8005\u7684\u4F4F\u9662\u8BB0\u5F55\u3002"));
        return;
    }

    bool isMyPatient = false;
    for (int i = 0; i < hosp->doctorCount; i++) {
        if (hosp->doctorIDs[i] == m_doctorID) { isMyPatient = true; break; }
    }
    if (!isMyPatient) {
        QMessageBox::warning(this,
            QString::fromUtf8("\u9519\u8BEF"),
            QString::fromUtf8("\u8BE5\u60A3\u8005\u4E0D\u662F\u60A8\u8D1F\u8D23\u7684\u3002"));
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, patientID);

    QDialog dlg(this);
    dlg.setWindowTitle(
        QString::fromUtf8("\u4E3A\u4F4F\u9662\u60A3\u8005\u5F00\u5904\u65B9/\u68C0\u67E5"));
    QVBoxLayout* dlgLayout = new QVBoxLayout(&dlg);
    dlgLayout->addWidget(new QLabel(
        QString::fromUtf8("\u60A3\u8005: %1  (%2)  \u4F4F\u9662ID: %3")
            .arg(patient ? QString::fromStdString(patient->name)
                         : QString::fromUtf8("\u672A\u77E5"))
            .arg(QString::fromStdString(patientID))
            .arg(QString::fromStdString(hosp->hospID))));

    QPushButton* btnPresc = new QPushButton(
        QString::fromUtf8("\u5F00\u5177\u5904\u65B9"));
    QPushButton* btnExam = new QPushButton(
        QString::fromUtf8("\u5F00\u5177\u68C0\u67E5"));
    QPushButton* btnCancel = new QPushButton(
        QString::fromUtf8("\u53D6\u6D88"));
    btnPresc->setMinimumHeight(40);
    btnExam->setMinimumHeight(40);
    dlgLayout->addWidget(btnPresc);
    dlgLayout->addWidget(btnExam);
    dlgLayout->addWidget(btnCancel);

    connect(btnPresc, &QPushButton::clicked, [&]() {
        dlg.accept();
        onViewInpatient();  // Reuse view dialog which has prescribe capability
    });

    connect(btnExam, &QPushButton::clicked, [&]() {
        dlg.accept();
        onViewInpatient();  // Reuse view dialog
    });

    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);

    dlg.exec();
    refreshAll();
}

// ======================================================================
// Tab 4: Record Search
// ======================================================================

void DoctorWidget::onSearchDoctorRecords() {
    QString query = recordSearchEdit->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u8BF7\u8F93\u5165\u60A3\u8005\u59D3\u540D\u6216ID\u8FDB\u884C\u641C\u7D22\u3002"));
        return;
    }
    std::string queryStr = query.toStdString();

    std::string foundPatientID;
    PatientNode* p = patientHead;
    while (p) {
        if (p->patientID == queryStr || p->name == queryStr) {
            foundPatientID = p->patientID;
            break;
        }
        p = p->next;
    }

    if (foundPatientID.empty()) {
        QMessageBox::information(this,
            QString::fromUtf8("\u63D0\u793A"),
            QString::fromUtf8("\u672A\u627E\u5230\u5339\u914D\u7684\u60A3\u8005\u3002"));
        return;
    }

    refreshRecordSearch(foundPatientID);
}

void DoctorWidget::refreshRecordSearch(const std::string& patientID) {
    recordTable->setRowCount(0);
    int row = 0;
    PatientNode* patient = findPatientByID(patientHead, patientID);

    // Registrations
    RegistrationNode* r = regHead;
    while (r) {
        if (r->patientID == patientID) {
            recordTable->insertRow(row);
            recordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u6302\u53F7")));
            recordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(r->regID)));
            recordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(r->patientID)));
            recordTable->setItem(row, 3, new QTableWidgetItem(
                patient ? QString::fromStdString(patient->name)
                        : QString::fromUtf8("\u672A\u77E5")));
            DepartmentNode* dept = findDepartmentByID(deptHead, r->departmentID);
            DoctorNode* doc = findDoctorByID(doctorHead, r->doctorID);
            recordTable->setItem(row, 4, new QTableWidgetItem(
                QString::fromUtf8("\u79D1\u5BA4: %1  \u533B\u751F: %2")
                    .arg(dept ? QString::fromStdString(dept->name)
                               : QString::fromStdString(r->departmentID))
                    .arg(doc ? QString::fromStdString(doc->name)
                             : QString::fromStdString(r->doctorID))));
            recordTable->setItem(row, 5, new QTableWidgetItem(
                QString("Day%1").arg(r->regDay)));
            recordTable->setItem(row, 6, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(REGISTRATION_FEE, 0, 'f', 2)));
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(getRegStatusStr(r->status)));
            si->setForeground(
                (r->status == STATUS_CANCELLED || r->status == STATUS_EXPIRED)
                    ? QColor("red") : QColor("green"));
            recordTable->setItem(row, 7, si);
            row++;
        }
        r = r->next;
    }

    // Consultations
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->patientID == patientID) {
            recordTable->insertRow(row);
            recordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u770B\u8BCA")));
            recordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(c->consultID)));
            recordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(c->patientID)));
            recordTable->setItem(row, 3, new QTableWidgetItem(
                patient ? QString::fromStdString(patient->name)
                        : QString::fromUtf8("\u672A\u77E5")));
            recordTable->setItem(row, 4, new QTableWidgetItem(
                QString::fromUtf8("\u4E3B\u8BC9: %1  \u8BCA\u65AD: %2")
                    .arg(QString::fromStdString(c->complaint))
                    .arg(QString::fromStdString(c->diagnosis))));
            recordTable->setItem(row, 5, new QTableWidgetItem(
                QString("Day%1").arg(c->consultDay)));
            recordTable->setItem(row, 6, new QTableWidgetItem(""));
            const char* s = (c->status == STATUS_CANCELLED)
                ? "\u5DF2\u64A4\u9500" : "\u6709\u6548";
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(s));
            si->setForeground(c->status == STATUS_CANCELLED
                ? QColor("red") : QColor("green"));
            recordTable->setItem(row, 7, si);
            row++;
        }
        c = c->next;
    }

    // Prescriptions
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->patientID == patientID) {
            recordTable->insertRow(row);
            recordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u5904\u65B9")));
            recordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(pr->prescID)));
            recordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(pr->patientID)));
            recordTable->setItem(row, 3, new QTableWidgetItem(
                patient ? QString::fromStdString(patient->name)
                        : QString::fromUtf8("\u672A\u77E5")));

            QString medSum;
            PrescMedicineNode* pm = prescMedHead;
            while (pm) {
                if (pm->prescID == pr->prescID) {
                    MedicineNode* m = findMedicineByID(medHead, pm->medID);
                    if (!medSum.isEmpty()) medSum += "; ";
                    medSum += QString("%1 x%2")
                        .arg(m ? QString::fromStdString(m->tradeName)
                               : QString::fromStdString(pm->medID))
                        .arg(pm->quantity);
                }
                pm = pm->next;
            }
            recordTable->setItem(row, 4, new QTableWidgetItem(medSum));
            recordTable->setItem(row, 5, new QTableWidgetItem(
                QString("Day%1").arg(pr->prescDay)));
            recordTable->setItem(row, 6, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(pr->totalAmount, 0, 'f', 2)));

            QString pStatus;
            if (pr->status == 0) pStatus = QString::fromUtf8("\u5F85\u7F34\u8D39");
            else if (pr->status == 1) pStatus = QString::fromUtf8("\u5DF2\u64A4\u9500");
            else if (pr->status == 2) pStatus = QString::fromUtf8("\u5DF2\u7F34\u8D39");
            else pStatus = QString::fromUtf8("\u672A\u77E5");
            QTableWidgetItem* si = new QTableWidgetItem(pStatus);
            si->setForeground(pr->status == 1 ? QColor("red")
                : (pr->status == 2 ? QColor("green") : QColor("orange")));
            recordTable->setItem(row, 7, si);
            row++;
        }
        pr = pr->next;
    }

    // Examinations
    ExaminationNode* e = examHead;
    while (e) {
        if (e->patientID == patientID) {
            recordTable->insertRow(row);
            recordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u68C0\u67E5")));
            recordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(e->examID)));
            recordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(e->patientID)));
            recordTable->setItem(row, 3, new QTableWidgetItem(
                patient ? QString::fromStdString(patient->name)
                        : QString::fromUtf8("\u672A\u77E5")));
            recordTable->setItem(row, 4, new QTableWidgetItem(
                QString::fromStdString(e->itemName)));
            recordTable->setItem(row, 5, new QTableWidgetItem(
                QString("Day%1").arg(e->examDay)));
            recordTable->setItem(row, 6, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(e->cost, 0, 'f', 2)));
            const char* s = (e->status == STATUS_CANCELLED)
                ? "\u5DF2\u64A4\u9500" : "\u6709\u6548";
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(s));
            si->setForeground(e->status == STATUS_CANCELLED
                ? QColor("red") : QColor("green"));
            recordTable->setItem(row, 7, si);
            row++;
        }
        e = e->next;
    }

    // Hospitalizations
    HospitalizationNode* h = hospHead;
    while (h) {
        if (h->patientID == patientID) {
            recordTable->insertRow(row);
            recordTable->setItem(row, 0, new QTableWidgetItem(
                QString::fromUtf8("\u4F4F\u9662")));
            recordTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromStdString(h->hospID)));
            recordTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromStdString(h->patientID)));
            recordTable->setItem(row, 3, new QTableWidgetItem(
                patient ? QString::fromStdString(patient->name)
                        : QString::fromUtf8("\u672A\u77E5")));
            recordTable->setItem(row, 4, new QTableWidgetItem(
                QString::fromUtf8("\u75C5\u623F: %1  \u5E8A\u4F4D: %2")
                    .arg(QString::fromStdString(h->wardID))
                    .arg(h->bedNo + 1)));
            recordTable->setItem(row, 5, new QTableWidgetItem(
                QString("Day%1").arg(h->admitDay)));
            recordTable->setItem(row, 6, new QTableWidgetItem(
                QString::fromUtf8("\u00A5%1").arg(h->deposit, 0, 'f', 2)));
            const char* s;
            if (h->status == 0) s = "\u5728\u9662";
            else if (h->status == 1) s = "\u5DF2\u51FA\u9662";
            else s = "\u5DF2\u64A4\u9500";
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(s));
            si->setForeground(h->status == 0 ? QColor("green") : QColor("gray"));
            recordTable->setItem(row, 7, si);
            row++;
        }
        h = h->next;
    }

    recordTable->resizeColumnsToContents();

    if (row == 0) {
        recordTable->insertRow(0);
        QTableWidgetItem* emp = new QTableWidgetItem(
            QString::fromUtf8("\u672A\u627E\u5230\u76F8\u5173\u8BB0\u5F55"));
        emp->setTextAlignment(Qt::AlignCenter);
        recordTable->setSpan(0, 0, 1, 8);
        recordTable->setItem(0, 0, emp);
    }
}

void DoctorWidget::onSearchPatientRecords() {
    onSearchDoctorRecords();
}
