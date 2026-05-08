#include "patient_panels.h"
#include "../global.h"
#include "../list_ops.h"
#include "../utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QSplitter>
#include <QGroupBox>
#include <QScrollArea>

extern DepartmentNode* deptHead;
extern DoctorNode* doctorHead;
extern PatientNode* patientHead;
extern RegistrationNode* regHead;
extern ConsultationNode* consultHead;
extern ExaminationNode* examHead;
extern PrescriptionNode* prescHead;
extern PrescMedicineNode* prescMedHead;
extern MedicineNode* medHead;
extern WardNode* wardHead;
extern HospitalizationNode* hospHead;
extern double money;

PatientWidget::PatientWidget(const std::string& patientID, QWidget* parent)
    : QWidget(parent), m_patientID(patientID), m_isInpatient(false), m_patient(nullptr),
      m_tabWidget(nullptr), recordsTable(nullptr), paymentTable(nullptr),
      totalUnpaidLabel(nullptr), payBtn(nullptr), inpatientTab(nullptr),
      wardLabel(nullptr), bedLabel(nullptr), doctorsLabel(nullptr),
      daysLabel(nullptr), depositLabel(nullptr), feeDetailTable(nullptr), topUpBtn(nullptr)
{
    m_patient = findPatientByID(patientHead, m_patientID);
    if (m_patient) {
        m_isInpatient = (m_patient->type == PATIENT_INPATIENT);
    }
    setupUI();
    refreshAll();
}

void PatientWidget::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel(QString::fromUtf8("患者服务面板"));
    QFont tf = title->font(); tf.setPointSize(18); tf.setBold(true);
    title->setFont(tf); title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QLabel* info = new QLabel(QString::fromUtf8(
        ("患者: " + (m_patient ? m_patient->name : "未知") + "  ID: " + m_patientID
         + "  类型: " + (m_isInpatient ? "住院" : "门诊")).c_str()));
    info->setAlignment(Qt::AlignCenter);
    layout->addWidget(info);

    m_tabWidget = new QTabWidget();
    layout->addWidget(m_tabWidget);

    // Tab 1: My Records
    QWidget* recTab = new QWidget();
    QVBoxLayout* recLay = new QVBoxLayout(recTab);
    recordsTable = new QTableWidget(0, 7);
    recordsTable->setHorizontalHeaderLabels({
        QString::fromUtf8("类型"), QString::fromUtf8("ID"),
        QString::fromUtf8("详情"), QString::fromUtf8("费用"),
        QString::fromUtf8("日期"), QString::fromUtf8("状态"),
        QString::fromUtf8("医生/科室")
    });
    recordsTable->horizontalHeader()->setStretchLastSection(true);
    recordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recLay->addWidget(recordsTable);
    m_tabWidget->addTab(recTab, QString::fromUtf8("我的记录"));

    // Tab 2: Payment
    QWidget* payTab = new QWidget();
    QVBoxLayout* payLay = new QVBoxLayout(payTab);
    totalUnpaidLabel = new QLabel(QString::fromUtf8("待支付总额: ¥0.00"));
    QFont pf = totalUnpaidLabel->font(); pf.setPointSize(14); totalUnpaidLabel->setFont(pf);
    payLay->addWidget(totalUnpaidLabel);

    paymentTable = new QTableWidget(0, 5);
    paymentTable->setHorizontalHeaderLabels({
        QString::fromUtf8(""), QString::fromUtf8("项目"),
        QString::fromUtf8("金额"), QString::fromUtf8("状态"),
        QString::fromUtf8("ID")
    });
    paymentTable->horizontalHeader()->setStretchLastSection(true);
    paymentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    payLay->addWidget(paymentTable);

    payBtn = new QPushButton(QString::fromUtf8("支付选中项目"));
    payBtn->setMinimumHeight(40);
    payLay->addWidget(payBtn);
    connect(payBtn, &QPushButton::clicked, this, &PatientWidget::onPayFees);

    m_tabWidget->addTab(payTab, QString::fromUtf8("费用支付"));

    // Tab 3: Inpatient Info (only if inpatient)
    if (m_isInpatient) {
        inpatientTab = new QWidget();
        QVBoxLayout* inpLay = new QVBoxLayout(inpatientTab);

        QGroupBox* infoBox = new QGroupBox(QString::fromUtf8("住院信息"));
        QVBoxLayout* infoLay = new QVBoxLayout(infoBox);
        wardLabel = new QLabel(); infoLay->addWidget(wardLabel);
        bedLabel = new QLabel(); infoLay->addWidget(bedLabel);
        doctorsLabel = new QLabel(); infoLay->addWidget(doctorsLabel);
        daysLabel = new QLabel(); infoLay->addWidget(daysLabel);
        depositLabel = new QLabel(); infoLay->addWidget(depositLabel);
        inpLay->addWidget(infoBox);

        feeDetailTable = new QTableWidget(0, 3);
        feeDetailTable->setHorizontalHeaderLabels({
            QString::fromUtf8("日期"), QString::fromUtf8("费用(元)"), QString::fromUtf8("余额")
        });
        inpLay->addWidget(new QLabel(QString::fromUtf8("每日费用明细:")));
        inpLay->addWidget(feeDetailTable);

        topUpBtn = new QPushButton(QString::fromUtf8("补缴押金"));
        topUpBtn->setMinimumHeight(40);
        inpLay->addWidget(topUpBtn);
        connect(topUpBtn, &QPushButton::clicked, this, &PatientWidget::onTopUpDeposit);

        m_tabWidget->addTab(inpatientTab, QString::fromUtf8("住院信息"));
    }

    QPushButton* refreshBtn = new QPushButton(QString::fromUtf8("刷新"));
    connect(refreshBtn, &QPushButton::clicked, this, &PatientWidget::onRefresh);
    layout->addWidget(refreshBtn);
}

void PatientWidget::refreshAll() {
    refreshMyRecords();
    refreshPaymentList();
    if (m_isInpatient) refreshInpatientInfo();
}

void PatientWidget::onRefresh() { refreshAll(); }

void PatientWidget::refreshMyRecords() {
    recordsTable->setRowCount(0);
    int row = 0;

    // Registrations
    RegistrationNode* r = regHead;
    while (r) {
        if (r->patientID == m_patientID) {
            recordsTable->insertRow(row);
            recordsTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8("挂号")));
            recordsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r->regID)));
            DepartmentNode* d = findDepartmentByID(deptHead, r->departmentID);
            DoctorNode* doc = findDoctorByID(doctorHead, r->doctorID);
            recordsTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromUtf8("科室: %1  医生: %2")
                    .arg(d ? QString::fromStdString(d->name) : QString::fromStdString(r->departmentID))
                    .arg(doc ? QString::fromStdString(doc->name) : QString::fromStdString(r->doctorID))));
            recordsTable->setItem(row, 3, new QTableWidgetItem(
                QString::fromUtf8("¥%1").arg(REGISTRATION_FEE, 0, 'f', 2)));
            recordsTable->setItem(row, 4, new QTableWidgetItem(
                QString("Day%1").arg(r->regDay)));
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(getRegStatusStr(r->status)));
            si->setForeground(r->status == STATUS_CANCELLED || r->status == STATUS_EXPIRED
                ? QColor("red") : QColor("green"));
            recordsTable->setItem(row, 5, si);
            recordsTable->setItem(row, 6, new QTableWidgetItem(
                doc ? QString::fromStdString(doc->name) : ""));
            row++;
        }
        r = r->next;
    }

    // Consultations
    ConsultationNode* c = consultHead;
    while (c) {
        if (c->patientID == m_patientID) {
            recordsTable->insertRow(row);
            recordsTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8("看诊")));
            recordsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c->consultID)));
            recordsTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromUtf8("主诉: %1  诊断: %2").arg(
                    QString::fromStdString(c->complaint),
                    QString::fromStdString(c->diagnosis))));
            recordsTable->setItem(row, 3, new QTableWidgetItem("-"));
            recordsTable->setItem(row, 4, new QTableWidgetItem(QString("Day%1").arg(c->consultDay)));
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(c->status == STATUS_CANCELLED ? "已撤销" : "有效"));
            si->setForeground(c->status == STATUS_CANCELLED ? QColor("red") : QColor("green"));
            recordsTable->setItem(row, 5, si);
            recordsTable->setItem(row, 6, new QTableWidgetItem(""));
            row++;
        }
        c = c->next;
    }

    // Examinations
    ExaminationNode* e = examHead;
    while (e) {
        if (e->patientID == m_patientID) {
            recordsTable->insertRow(row);
            recordsTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8("检查")));
            recordsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(e->examID)));
            recordsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(e->itemName)));
            recordsTable->setItem(row, 3, new QTableWidgetItem(
                QString::fromUtf8("¥%1").arg(e->cost, 0, 'f', 2)));
            recordsTable->setItem(row, 4, new QTableWidgetItem(QString("Day%1").arg(e->examDay)));
            QTableWidgetItem* si = new QTableWidgetItem(
                QString::fromUtf8(e->status == STATUS_CANCELLED ? "已撤销" : "有效"));
            si->setForeground(e->status == STATUS_CANCELLED ? QColor("red") : QColor("green"));
            recordsTable->setItem(row, 5, si);
            recordsTable->setItem(row, 6, new QTableWidgetItem(""));
            row++;
        }
        e = e->next;
    }

    // Prescriptions
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->patientID == m_patientID) {
            recordsTable->insertRow(row);
            recordsTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8("处方")));
            recordsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(pr->prescID)));

            QString meds;
            PrescMedicineNode* pm = prescMedHead;
            while (pm) {
                if (pm->prescID == pr->prescID) {
                    MedicineNode* m = findMedicineByID(medHead, pm->medID);
                    if (!meds.isEmpty()) meds += "; ";
                    meds += QString("%1 x%2").arg(
                        m ? QString::fromStdString(m->tradeName) : QString::fromStdString(pm->medID))
                        .arg(pm->quantity);
                }
                pm = pm->next;
            }
            recordsTable->setItem(row, 2, new QTableWidgetItem(meds));
            recordsTable->setItem(row, 3, new QTableWidgetItem(
                QString::fromUtf8("¥%1").arg(pr->totalAmount, 0, 'f', 2)));
            recordsTable->setItem(row, 4, new QTableWidgetItem(QString("Day%1").arg(pr->prescDay)));

            const char* ps;
            if (pr->status == 0) ps = "待缴费";
            else if (pr->status == 1) ps = "已撤销";
            else if (pr->status == 2) ps = "已缴费";
            else ps = "未知";
            QTableWidgetItem* si = new QTableWidgetItem(QString::fromUtf8(ps));
            si->setForeground(pr->status == 1 ? QColor("red") :
                (pr->status == 2 ? QColor("green") : QColor("orange")));
            recordsTable->setItem(row, 5, si);
            recordsTable->setItem(row, 6, new QTableWidgetItem(""));
            row++;
        }
        pr = pr->next;
    }

    // Hospitalizations
    HospitalizationNode* h = hospHead;
    while (h) {
        if (h->patientID == m_patientID) {
            recordsTable->insertRow(row);
            recordsTable->setItem(row, 0, new QTableWidgetItem(QString::fromUtf8("住院")));
            recordsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(h->hospID)));
            recordsTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromUtf8("病房: %1 床位: %2").arg(
                    QString::fromStdString(h->wardID)).arg(h->bedNo + 1)));
            recordsTable->setItem(row, 3, new QTableWidgetItem(
                QString::fromUtf8("押金: ¥%1").arg(h->deposit, 0, 'f', 2)));
            recordsTable->setItem(row, 4, new QTableWidgetItem(
                QString("Day%1~Day%2").arg(h->admitDay).arg(
                    h->dischargeDay > 0 ? h->dischargeDay : weekday)));
            const char* hs = h->status == 0 ? "在院" : (h->status == 1 ? "已出院" : "已撤销");
            QTableWidgetItem* si = new QTableWidgetItem(QString::fromUtf8(hs));
            si->setForeground(h->status == 0 ? QColor("green") : QColor("gray"));
            recordsTable->setItem(row, 5, si);
            recordsTable->setItem(row, 6, new QTableWidgetItem(""));
            row++;
        }
        h = h->next;
    }

    recordsTable->resizeColumnsToContents();

    if (row == 0) {
        recordsTable->insertRow(0);
        QTableWidgetItem* emp = new QTableWidgetItem(QString::fromUtf8("暂无记录"));
        emp->setTextAlignment(Qt::AlignCenter);
        recordsTable->setSpan(0, 0, 1, 7);
        recordsTable->setItem(0, 0, emp);
    }
}

void PatientWidget::refreshPaymentList() {
    paymentTable->setRowCount(0);
    double totalUnpaid = 0.0;
    int row = 0;

    // Unpaid registrations
    RegistrationNode* r = regHead;
    while (r) {
        if (r->patientID == m_patientID && r->status == 0) {
            paymentTable->insertRow(row);
            QTableWidgetItem* ck = new QTableWidgetItem();
            ck->setCheckState(Qt::Unchecked);
            ck->setData(Qt::UserRole, QString("REG|%1|%2").arg(
                QString::fromStdString(r->regID)).arg(REGISTRATION_FEE, 0, 'f', 2));
            paymentTable->setItem(row, 0, ck);
            DoctorNode* doc = findDoctorByID(doctorHead, r->doctorID);
            paymentTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8("挂号费 - %1").arg(
                doc ? QString::fromStdString(doc->name) : "")));
            paymentTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromUtf8("¥%1").arg(REGISTRATION_FEE, 0, 'f', 2)));
            paymentTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("待支付")));
            paymentTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(r->regID)));
            totalUnpaid += REGISTRATION_FEE;
            row++;
        }
        r = r->next;
    }

    // Unpaid prescriptions
    PrescriptionNode* pr = prescHead;
    while (pr) {
        if (pr->patientID == m_patientID && pr->status == 0) {
            paymentTable->insertRow(row);
            QTableWidgetItem* ck = new QTableWidgetItem();
            ck->setCheckState(Qt::Unchecked);
            ck->setData(Qt::UserRole, QString("PRESC|%1|%2").arg(
                QString::fromStdString(pr->prescID)).arg(pr->totalAmount, 0, 'f', 2));
            paymentTable->setItem(row, 0, ck);

            QString medInfo;
            PrescMedicineNode* pm = prescMedHead;
            while (pm) {
                if (pm->prescID == pr->prescID) {
                    MedicineNode* m = findMedicineByID(medHead, pm->medID);
                    if (!medInfo.isEmpty()) medInfo += ", ";
                    medInfo += (m ? QString::fromStdString(m->tradeName) : QString::fromStdString(pm->medID));
                }
                pm = pm->next;
            }
            paymentTable->setItem(row, 1, new QTableWidgetItem(
                QString::fromUtf8("处方药费 - %1").arg(medInfo)));
            paymentTable->setItem(row, 2, new QTableWidgetItem(
                QString::fromUtf8("¥%1").arg(pr->totalAmount, 0, 'f', 2)));
            paymentTable->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8("待缴费")));
            paymentTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(pr->prescID)));
            totalUnpaid += pr->totalAmount;
            row++;
        }
        pr = pr->next;
    }

    totalUnpaidLabel->setText(QString::fromUtf8("待支付总额: ¥%1").arg(totalUnpaid, 0, 'f', 2));
    paymentTable->resizeColumnsToContents();
}

void PatientWidget::onPayFees() {
    double amountToPay = 0.0;
    std::vector<std::pair<std::string, std::string>> toPay; // type, id

    for (int i = 0; i < paymentTable->rowCount(); i++) {
        QTableWidgetItem* ck = paymentTable->item(i, 0);
        if (ck && ck->checkState() == Qt::Checked) {
            QString data = ck->data(Qt::UserRole).toString();
            QStringList parts = data.split("|");
            if (parts.size() >= 3) {
                toPay.push_back({parts[0].toStdString(), parts[1].toStdString()});
                amountToPay += parts[2].toDouble();
            }
        }
    }

    if (toPay.empty()) {
        QMessageBox::information(this, QString::fromUtf8("提示"),
            QString::fromUtf8("请至少勾选一个待支付项目。"));
        return;
    }

    // For outpatient: check if they can pay
    if (!m_isInpatient) {
        // Outpatient pays directly to hospital
        money = round2(money + amountToPay);
    } else {
        // Inpatient pays from deposit
        HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, m_patientID);
        if (!hosp) {
            QMessageBox::warning(this, QString::fromUtf8("错误"),
                QString::fromUtf8("未找到在院记录。"));
            return;
        }
        hosp->deposit = round2(hosp->deposit - amountToPay);
        money = round2(money + amountToPay);
        if (m_patient) m_patient->deposit = hosp->deposit;
    }

    // Update statuses
    for (auto& tp : toPay) {
        if (tp.first == "REG") {
            RegistrationNode* r = findRegistrationByID(regHead, tp.second);
            if (r && r->status == 0) r->status = STATUS_SEEN;
        } else if (tp.first == "PRESC") {
            PrescriptionNode* pr = findPrescriptionByID(prescHead, tp.second);
            if (pr && pr->status == 0) {
                pr->status = 2; // paid
                // Deduct medicine stock
                PrescMedicineNode* pm = prescMedHead;
                while (pm) {
                    if (pm->prescID == pr->prescID) {
                        MedicineNode* med = findMedicineByID(medHead, pm->medID);
                        if (med) {
                            med->stock -= pm->quantity;
                            med->consumed += pm->quantity;
                            if (med->stock < 0) med->stock = 0;
                        }
                    }
                    pm = pm->next;
                }
            }
        }
    }

    QMessageBox::information(this, QString::fromUtf8("支付成功"),
        QString::fromUtf8("已支付 %1 元。\n%2")
            .arg(amountToPay, 0, 'f', 2)
            .arg(m_isInpatient ?
                QString::fromUtf8("已从住院押金扣除。") :
                QString::fromUtf8("医院资金已更新。")));

    refreshAll();
}

void PatientWidget::refreshInpatientInfo() {
    if (!m_isInpatient) return;

    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, m_patientID);
    if (!hosp) {
        wardLabel->setText(QString::fromUtf8("您当前未在住院。"));
        return;
    }

    WardNode* ward = findWardByID(wardHead, hosp->wardID);
    wardLabel->setText(QString::fromUtf8("病房: %1 (%2)")
        .arg(QString::fromStdString(hosp->wardID))
        .arg(ward ? QString::fromUtf8(getWardTypeName(ward->type)) : "?"));
    bedLabel->setText(QString::fromUtf8("床位号: %1").arg(hosp->bedNo + 1));

    QString docNames;
    for (int i = 0; i < hosp->doctorCount; i++) {
        DoctorNode* doc = findDoctorByID(doctorHead, hosp->doctorIDs[i]);
        if (i > 0) docNames += ", ";
        docNames += doc ? QString::fromStdString(doc->name) : QString::fromStdString(hosp->doctorIDs[i]);
    }
    doctorsLabel->setText(QString::fromUtf8("责任医生: %1").arg(docNames));

    unsigned int daysStayed = weekday - hosp->admitDay;
    if (daysStayed < 1) daysStayed = 1;
    daysLabel->setText(QString::fromUtf8("入院天数: %1 天 (Day%2 ~ Day%3)")
        .arg(daysStayed).arg(hosp->admitDay).arg(weekday));

    depositLabel->setText(QString::fromUtf8(
        "<b>押金余额: ¥%1</b> %2")
        .arg(hosp->deposit, 0, 'f', 2)
        .arg(hosp->deposit < 0 ?
            QString::fromUtf8("<span style='color:red'>[不足!]</span>") :
            QString::fromUtf8("<span style='color:green'>[正常]</span>")));

    // Daily fee detail
    feeDetailTable->setRowCount(0);
    double balance = hosp->deposit + HOSPITAL_DAILY_FEE * daysStayed; // starting balance
    for (unsigned int d = 0; d < daysStayed; d++) {
        balance = round2(balance - HOSPITAL_DAILY_FEE);
        feeDetailTable->insertRow(d);
        feeDetailTable->setItem(d, 0, new QTableWidgetItem(
            QString("Day%1").arg(hosp->admitDay + d)));
        feeDetailTable->setItem(d, 1, new QTableWidgetItem(
            QString::fromUtf8("¥%1").arg(HOSPITAL_DAILY_FEE, 0, 'f', 2)));
        feeDetailTable->setItem(d, 2, new QTableWidgetItem(
            QString::fromUtf8("¥%1").arg(balance, 0, 'f', 2)));
    }
    feeDetailTable->resizeColumnsToContents();
}

void PatientWidget::onTopUpDeposit() {
    HospitalizationNode* hosp = findActiveHospitalizationByPatient(hospHead, m_patientID);
    if (!hosp) {
        QMessageBox::warning(this, QString::fromUtf8("错误"),
            QString::fromUtf8("未找到在院记录。"));
        return;
    }

    bool ok;
    double amount = QInputDialog::getDouble(this, QString::fromUtf8("补缴押金"),
        QString::fromUtf8("当前押金: ¥%1\n请输入补缴金额:").arg(hosp->deposit, 0, 'f', 2),
        100.00, 0.01, 999999.99, 2, &ok);

    if (!ok || amount <= 0) return;

    hosp->deposit = round2(hosp->deposit + amount);
    money = round2(money + amount);
    if (m_patient) m_patient->deposit = hosp->deposit;

    QMessageBox::information(this, QString::fromUtf8("补缴成功"),
        QString::fromUtf8("已补缴 ¥%1。\n当前押金余额: ¥%2")
            .arg(amount, 0, 'f', 2)
            .arg(hosp->deposit, 0, 'f', 2));

    refreshInpatientInfo();
}
