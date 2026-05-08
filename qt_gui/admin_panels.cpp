#include "admin_panels.h"
#include "../global.h"
#include "../entities.h"
#include "../list_ops.h"
#include "../utils.h"
#include "../persistence.h"
#include "../batch_input.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSplitter>
#include <QGroupBox>
#include <QDateEdit>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QListWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QProgressBar>

// ==================== Extern global linked list heads ====================
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

// ==================== Helper: Build weekday display string ====================
static QString buildWorkDayString(DoctorNode* doc) {
    QString result;
    for (int i = 0; i < doc->workDayCount; i++) {
        if (i > 0) result += ",";
        int dayIndex = doc->workDays[i] - 1;
        if (dayIndex >= 0 && dayIndex < 7)
            result += WEEKDAY_NAMES[dayIndex];
    }
    return result.isEmpty() ? QString::fromUtf8("无") : result;
}

// ==================== Helper: Get today's day-of-week (1=Mon..7=Sun) ====================
static int todayDayOfWeek() {
    return ((weekday - 1) % 7) + 1;
}

// ==================== Constructor ====================
AdminWidget::AdminWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Time advance button and label at top
    QHBoxLayout* timeLayout = new QHBoxLayout();
    QLabel* timeLabel = new QLabel(QString::fromUtf8(formatTime().c_str()));
    timeLabel->setStyleSheet("font-weight: bold;");
    QPushButton* timeBtn = new QPushButton(QString::fromUtf8("时间推进"));
    timeLayout->addWidget(timeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(timeBtn);
    mainLayout->addLayout(timeLayout);

    connect(timeBtn, &QPushButton::clicked, this, &AdminWidget::onTimeAdvance);

    // Main tab widget with 7 tabs
    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);

    setupDeptPanel();
    setupDoctorPanel();
    setupPatientPanel();
    setupRegPanel();
    setupHospPanel();
    setupMedPanel();
    setupReportPanel();
}

void AdminWidget::refreshAll() {
    refreshDeptTable();
    refreshDoctorTable();
    refreshPatientTable();
    refreshRegTable();
    refreshWardGrid();
    refreshHospTable();
    refreshMedTable();
    refreshReportTabs();
}

// ==================== Tab 1: Department Panel ====================
void AdminWidget::setupDeptPanel() {
    QWidget* panel = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(panel);

    // Left: table
    QVBoxLayout* leftLayout = new QVBoxLayout();
    deptTable = new QTableWidget(0, 5);
    deptTable->setHorizontalHeaderLabels({
        QString::fromUtf8("科室ID"),
        QString::fromUtf8("名称"),
        QString::fromUtf8("描述"),
        QString::fromUtf8("关联病房数"),
        QString::fromUtf8("医生数")
    });
    deptTable->horizontalHeader()->setStretchLastSection(true);
    deptTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    deptTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    deptTable->setSortingEnabled(false);
    connect(deptTable, &QTableWidget::cellDoubleClicked, this, &AdminWidget::onDeptDoubleClicked);
    leftLayout->addWidget(deptTable);
    mainLayout->addLayout(leftLayout, 3);

    // Right: buttons
    QVBoxLayout* btnLayout = new QVBoxLayout();
    QPushButton* btnAdd = new QPushButton(QString::fromUtf8("新增"));
    QPushButton* btnMod = new QPushButton(QString::fromUtf8("修改"));
    QPushButton* btnDel = new QPushButton(QString::fromUtf8("删除"));
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnMod);
    btnLayout->addWidget(btnDel);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout, 1);

    connect(btnAdd, &QPushButton::clicked, this, &AdminWidget::onAddDept);
    connect(btnMod, &QPushButton::clicked, this, &AdminWidget::onModifyDept);
    connect(btnDel, &QPushButton::clicked, this, &AdminWidget::onDeleteDept);

    tabWidget->addTab(panel, QString::fromUtf8("科室管理"));
}

void AdminWidget::refreshDeptTable() {
    deptTable->setRowCount(0);
    int row = 0;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        deptTable->insertRow(row);
        deptTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(d->deptID)));
        deptTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(d->name)));
        deptTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(d->description)));

        // Count wards linked to this department
        int wardCount = 0;
        for (WardNode* w = wardHead; w != nullptr; w = w->next) {
            if (w->departmentID == d->deptID) wardCount++;
        }
        deptTable->setItem(row, 3, new QTableWidgetItem(QString::number(wardCount)));

        // Count doctors in this department
        int docCount = 0;
        for (DoctorNode* doc = doctorHead; doc != nullptr; doc = doc->next) {
            if (doc->departmentID == d->deptID) docCount++;
        }
        deptTable->setItem(row, 4, new QTableWidgetItem(QString::number(docCount)));
        row++;
    }
    deptTable->resizeColumnsToContents();
}

void AdminWidget::onAddDept() {
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("新增科室"));
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("科室名称:"), nameEdit);

    QLineEdit* descEdit = new QLineEdit();
    descEdit->setMaxLength(MAX_DESC_LEN);
    form.addRow(QString::fromUtf8("科室描述:"), descEdit);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("科室名称不能为空!"));
            return;
        }
        DepartmentNode* node = new DepartmentNode();
        node->deptID = generateDeptID();
        node->name = name.toStdString();
        node->description = descEdit->text().trimmed().toStdString();
        insertDepartmentHead(deptHead, node);
        refreshDeptTable();
    }
}

void AdminWidget::onModifyDept() {
    int curRow = deptTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一行科室记录!"));
        return;
    }
    QString deptID = deptTable->item(curRow, 0)->text();
    DepartmentNode* dept = findDepartmentByID(deptHead, deptID.toStdString());
    if (!dept) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("科室不存在!"));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("修改科室"));
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(QString::fromStdString(dept->name));
    nameEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("科室名称:"), nameEdit);

    QLineEdit* descEdit = new QLineEdit(QString::fromStdString(dept->description));
    descEdit->setMaxLength(MAX_DESC_LEN);
    form.addRow(QString::fromUtf8("科室描述:"), descEdit);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("科室名称不能为空!"));
            return;
        }
        dept->name = name.toStdString();
        dept->description = descEdit->text().trimmed().toStdString();
        refreshDeptTable();
    }
}

void AdminWidget::onDeleteDept() {
    int curRow = deptTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一行科室记录!"));
        return;
    }
    QString deptID = deptTable->item(curRow, 0)->text();
    DepartmentNode* dept = findDepartmentByID(deptHead, deptID.toStdString());
    if (!dept) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("科室不存在!"));
        return;
    }

    // Check doctors
    int docCount = 0;
    for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
        if (d->departmentID == dept->deptID) docCount++;
    }
    if (docCount > 0) {
        QMessageBox::warning(this, QString::fromUtf8("无法删除"),
            QString::fromUtf8(("科室 " + dept->name + " 下还有 " + std::to_string(docCount) + " 名医生，无法删除!").c_str()));
        return;
    }

    // Check wards
    int wrdCount = 0;
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        if (w->departmentID == dept->deptID) wrdCount++;
    }
    if (wrdCount > 0) {
        QMessageBox::warning(this, QString::fromUtf8("无法删除"),
            QString::fromUtf8(("科室 " + dept->name + " 下还有 " + std::to_string(wrdCount) + " 个病房，无法删除!").c_str()));
        return;
    }

    int ret = QMessageBox::question(this, QString::fromUtf8("确认删除"),
        QString::fromUtf8(("确认删除科室 " + dept->name + "?").c_str()),
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        deleteDepartmentByID(deptHead, dept->deptID);
        refreshDeptTable();
    }
}

void AdminWidget::onDeptDoubleClicked(int row, int /*col*/) {
    if (row < 0 || row >= deptTable->rowCount()) return;
    QString deptID = deptTable->item(row, 0)->text();
    DepartmentNode* dept = findDepartmentByID(deptHead, deptID.toStdString());
    if (!dept) return;

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8(("科室详情 - " + dept->name).c_str()));
    dlg.resize(600, 500);
    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QTabWidget* subTabs = new QTabWidget();
    layout->addWidget(subTabs);

    // Subtask 1: Linked wards
    QWidget* wardTab = new QWidget();
    QVBoxLayout* wardLayout = new QVBoxLayout(wardTab);
    QListWidget* wardList = new QListWidget();
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        QListWidgetItem* item = new QListWidgetItem();
        QString text = QString::fromStdString(
            w->wardID + " (" + getWardTypeName(w->type) + ") - " + std::to_string(w->bedCount) + "床");
        if (w->departmentID == dept->deptID) {
            text += QString::fromUtf8(" [已关联]");
            item->setCheckState(Qt::Checked);
        } else {
            DepartmentNode* otherDept = findDepartmentByID(deptHead, w->departmentID);
            text += QString::fromUtf8(" - 当前科室: ") + (otherDept ? QString::fromStdString(otherDept->name) : QString::fromUtf8("无"));
            item->setCheckState(Qt::Unchecked);
        }
        item->setText(text);
        item->setData(Qt::UserRole, QString::fromStdString(w->wardID));
        wardList->addItem(item);
    }
    wardLayout->addWidget(new QLabel(QString::fromUtf8("勾选以关联病房到此科室（同时会解除原科室关联）：")));
    wardLayout->addWidget(wardList);

    QPushButton* saveWardBtn = new QPushButton(QString::fromUtf8("保存病房关联"));
    wardLayout->addWidget(saveWardBtn);
    subTabs->addTab(wardTab, QString::fromUtf8("关联病房"));

    // Subtask 2: Linked medicines
    QWidget* medTab = new QWidget();
    QVBoxLayout* medLayout = new QVBoxLayout(medTab);
    QListWidget* medList = new QListWidget();
    for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
        QListWidgetItem* item = new QListWidgetItem();
        QString text = QString::fromStdString(m->medID + " - " + m->tradeName + " (" + m->genericName + ")");
        item->setText(text);
        item->setData(Qt::UserRole, QString::fromStdString(m->medID));
        if (findDeptMedicine(deptMedHead, dept->deptID, m->medID)) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
        medList->addItem(item);
    }
    medLayout->addWidget(new QLabel(QString::fromUtf8("勾选以分配药品到本科室：")));
    medLayout->addWidget(medList);

    QPushButton* saveMedBtn = new QPushButton(QString::fromUtf8("保存药品分配"));
    medLayout->addWidget(saveMedBtn);
    subTabs->addTab(medTab, QString::fromUtf8("关联药品"));

    // Button connections
    connect(saveWardBtn, &QPushButton::clicked, [&]() {
        for (int i = 0; i < wardList->count(); i++) {
            QListWidgetItem* item = wardList->item(i);
            QString wID = item->data(Qt::UserRole).toString();
            WardNode* w = findWardByID(wardHead, wID.toStdString());
            if (!w) continue;
            if (item->checkState() == Qt::Checked) {
                w->departmentID = dept->deptID;
            }
        }
        refreshDeptTable();
        QMessageBox::information(&dlg, QString::fromUtf8("成功"), QString::fromUtf8("病房关联已更新!"));
    });
    connect(saveMedBtn, &QPushButton::clicked, [&]() {
        for (int i = 0; i < medList->count(); i++) {
            QListWidgetItem* item = medList->item(i);
            QString mID = item->data(Qt::UserRole).toString();
            bool currentlyLinked = findDeptMedicine(deptMedHead, dept->deptID, mID.toStdString());
            if (item->checkState() == Qt::Checked && !currentlyLinked) {
                DeptMedicineNode* dm = new DeptMedicineNode();
                dm->deptID = dept->deptID;
                dm->medID = mID.toStdString();
                insertDeptMedicineHead(deptMedHead, dm);
            } else if (item->checkState() == Qt::Unchecked && currentlyLinked) {
                deleteDeptMedicine(deptMedHead, dept->deptID, mID.toStdString());
            }
        }
        QMessageBox::information(&dlg, QString::fromUtf8("成功"), QString::fromUtf8("药品分配已更新!"));
    });

    dlg.exec();
}

// ==================== Tab 2: Doctor Panel ====================
void AdminWidget::setupDoctorPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);

    // Filter bar
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel(QString::fromUtf8("按科室筛选:")));
    doctorDeptFilter = new QComboBox();
    doctorDeptFilter->addItem(QString::fromUtf8("全部"), QVariant(QString()));
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        doctorDeptFilter->addItem(QString::fromStdString(d->name), QVariant(QString::fromStdString(d->deptID)));
    }
    filterLayout->addWidget(doctorDeptFilter);
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);

    // Table
    doctorTable = new QTableWidget(0, 5);
    doctorTable->setHorizontalHeaderLabels({
        QString::fromUtf8("医生ID"),
        QString::fromUtf8("姓名"),
        QString::fromUtf8("级别"),
        QString::fromUtf8("所属科室"),
        QString::fromUtf8("出诊星期")
    });
    doctorTable->horizontalHeader()->setStretchLastSection(true);
    doctorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    doctorTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(doctorTable);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnAdd = new QPushButton(QString::fromUtf8("新增"));
    QPushButton* btnMod = new QPushButton(QString::fromUtf8("修改"));
    QPushButton* btnDel = new QPushButton(QString::fromUtf8("删除"));
    QPushButton* btnLeave = new QPushButton(QString::fromUtf8("请假/离职标记"));
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnMod);
    btnLayout->addWidget(btnDel);
    btnLayout->addWidget(btnLeave);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(doctorDeptFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, [this]() {
            QString deptID = doctorDeptFilter->currentData().toString();
            onFilterDoctorByDept(deptID);
        });
    connect(btnAdd, &QPushButton::clicked, this, &AdminWidget::onAddDoctor);
    connect(btnMod, &QPushButton::clicked, this, &AdminWidget::onModifyDoctor);
    connect(btnDel, &QPushButton::clicked, this, &AdminWidget::onDeleteDoctor);
    connect(btnLeave, &QPushButton::clicked, [this]() {
        int curRow = doctorTable->currentRow();
        if (curRow < 0) {
            QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一名医生!"));
            return;
        }
        QString docID = doctorTable->item(curRow, 0)->text();
        DoctorNode* doc = findDoctorByID(doctorHead, docID.toStdString());
        if (!doc) return;
        QStringList items;
        items << QString::fromUtf8("设置请假（清除出诊日）")
              << QString::fromUtf8("恢复出诊（需手动重设出诊日）")
              << QString::fromUtf8("取消");
        bool ok;
        QString choice = QInputDialog::getItem(this, QString::fromUtf8("请假/离职标记"),
            QString::fromUtf8(("医生: " + doc->name).c_str()), items, 0, false, &ok);
        if (!ok || choice == QString::fromUtf8("取消")) return;
        if (choice == QString::fromUtf8("设置请假（清除出诊日）")) {
            doc->workDayCount = 0;
            for (int i = 0; i < MAX_WORKDAYS; i++) doc->workDays[i] = 0;
        }
        refreshDoctorTable();
    });

    tabWidget->addTab(panel, QString::fromUtf8("医生管理"));
}

void AdminWidget::refreshDoctorTable() {
    QString filterDept = doctorDeptFilter->currentData().toString();
    doctorTable->setRowCount(0);
    int row = 0;
    for (DoctorNode* doc = doctorHead; doc != nullptr; doc = doc->next) {
        if (!filterDept.isEmpty() && doc->departmentID != filterDept.toStdString()) continue;
        doctorTable->insertRow(row);
        doctorTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(doc->doctorID)));
        doctorTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(doc->name)));
        doctorTable->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(getLevelName(doc->level))));
        DepartmentNode* dept = findDepartmentByID(deptHead, doc->departmentID);
        doctorTable->setItem(row, 3, new QTableWidgetItem(
            dept ? QString::fromStdString(dept->name) : QString::fromUtf8("未知")));
        doctorTable->setItem(row, 4, new QTableWidgetItem(buildWorkDayString(doc)));
        row++;
    }
    doctorTable->resizeColumnsToContents();
}

void AdminWidget::onFilterDoctorByDept(const QString& /*deptID*/) {
    refreshDoctorTable();
}

void AdminWidget::onAddDoctor() {
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("新增医生"));
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("姓名:"), nameEdit);

    QComboBox* levelCombo = new QComboBox();
    for (int i = 0; i < MAX_DOCTOR_LEVELS; i++) {
        levelCombo->addItem(QString::fromUtf8(LEVEL_NAMES[i]));
    }
    form.addRow(QString::fromUtf8("级别:"), levelCombo);

    QComboBox* deptCombo = new QComboBox();
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        deptCombo->addItem(QString::fromStdString(d->name), QVariant(QString::fromStdString(d->deptID)));
    }
    form.addRow(QString::fromUtf8("所属科室:"), deptCombo);

    // Work days - checkboxes Mon-Sun
    QGroupBox* workDayGroup = new QGroupBox(QString::fromUtf8("出诊星期"));
    QHBoxLayout* workDayLayout = new QHBoxLayout(workDayGroup);
    QCheckBox* dayChecks[7];
    for (int i = 0; i < 7; i++) {
        dayChecks[i] = new QCheckBox(QString::fromUtf8(WEEKDAY_NAMES[i]));
        workDayLayout->addWidget(dayChecks[i]);
    }
    form.addRow(workDayGroup);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("医生姓名不能为空!"));
            return;
        }
        if (deptCombo->currentData().isNull()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请选择所属科室!"));
            return;
        }

        DoctorNode* node = new DoctorNode();
        node->doctorID = generateDoctorID();
        node->name = name.toStdString();
        node->level = levelCombo->currentIndex();
        node->departmentID = deptCombo->currentData().toString().toStdString();

        int dayCount = 0;
        for (int i = 0; i < 7; i++) {
            if (dayChecks[i]->isChecked() && dayCount < MAX_WORKDAYS) {
                node->workDays[dayCount++] = i + 1;
            }
        }
        node->workDayCount = dayCount;

        insertDoctorHead(doctorHead, node);
        refreshDoctorTable();
    }
}

void AdminWidget::onModifyDoctor() {
    int curRow = doctorTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一名医生!"));
        return;
    }
    QString docID = doctorTable->item(curRow, 0)->text();
    DoctorNode* doc = findDoctorByID(doctorHead, docID.toStdString());
    if (!doc) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("医生不存在!"));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("修改医生"));
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(QString::fromStdString(doc->name));
    nameEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("姓名:"), nameEdit);

    QComboBox* levelCombo = new QComboBox();
    for (int i = 0; i < MAX_DOCTOR_LEVELS; i++) {
        levelCombo->addItem(QString::fromUtf8(LEVEL_NAMES[i]));
    }
    levelCombo->setCurrentIndex(doc->level);
    form.addRow(QString::fromUtf8("级别:"), levelCombo);

    QComboBox* deptCombo = new QComboBox();
    int deptIndex = -1;
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        deptCombo->addItem(QString::fromStdString(d->name), QVariant(QString::fromStdString(d->deptID)));
        if (d->deptID == doc->departmentID) deptIndex = deptCombo->count() - 1;
    }
    if (deptIndex >= 0) deptCombo->setCurrentIndex(deptIndex);
    form.addRow(QString::fromUtf8("所属科室:"), deptCombo);

    QGroupBox* workDayGroup = new QGroupBox(QString::fromUtf8("出诊星期"));
    QHBoxLayout* workDayLayout = new QHBoxLayout(workDayGroup);
    QCheckBox* dayChecks[7];
    for (int i = 0; i < 7; i++) {
        dayChecks[i] = new QCheckBox(QString::fromUtf8(WEEKDAY_NAMES[i]));
        for (int j = 0; j < doc->workDayCount; j++) {
            if (doc->workDays[j] == i + 1) { dayChecks[i]->setChecked(true); break; }
        }
        workDayLayout->addWidget(dayChecks[i]);
    }
    form.addRow(workDayGroup);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("医生姓名不能为空!"));
            return;
        }
        doc->name = name.toStdString();
        doc->level = levelCombo->currentIndex();
        if (!deptCombo->currentData().isNull())
            doc->departmentID = deptCombo->currentData().toString().toStdString();

        int dayCount = 0;
        for (int i = 0; i < 7; i++) {
            if (dayChecks[i]->isChecked() && dayCount < MAX_WORKDAYS) {
                doc->workDays[dayCount++] = i + 1;
            }
        }
        doc->workDayCount = dayCount;
        refreshDoctorTable();
    }
}

void AdminWidget::onDeleteDoctor() {
    int curRow = doctorTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一名医生!"));
        return;
    }
    QString docID = doctorTable->item(curRow, 0)->text();
    DoctorNode* doc = findDoctorByID(doctorHead, docID.toStdString());
    if (!doc) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("医生不存在!"));
        return;
    }

    int pendingRegs = 0;
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->doctorID == doc->doctorID && r->status == STATUS_PENDING) pendingRegs++;
    }
    QString msg = QString::fromUtf8(("确认删除医生 " + doc->name + "?").c_str());
    if (pendingRegs > 0) {
        msg += QString::fromUtf8(("\n警告: 该医生还有 " + std::to_string(pendingRegs) + " 条待处理挂号记录!").c_str());
    }
    int ret = QMessageBox::question(this, QString::fromUtf8("确认删除"), msg,
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        deleteDoctorByID(doctorHead, doc->doctorID);
        refreshDoctorTable();
    }
}

// ==================== Tab 3: Patient Panel ====================
void AdminWidget::setupPatientPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);

    // Search bar
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel(QString::fromUtf8("搜索(姓名/ID):")));
    patientSearch = new QLineEdit();
    patientSearch->setPlaceholderText(QString::fromUtf8("输入患者姓名或ID搜索..."));
    searchLayout->addWidget(patientSearch);
    QPushButton* searchBtn = new QPushButton(QString::fromUtf8("搜索"));
    searchLayout->addWidget(searchBtn);
    QPushButton* clearBtn = new QPushButton(QString::fromUtf8("清除"));
    searchLayout->addWidget(clearBtn);
    mainLayout->addLayout(searchLayout);

    // Table
    patientTable = new QTableWidget(0, 5);
    patientTable->setHorizontalHeaderLabels({
        QString::fromUtf8("患者ID"),
        QString::fromUtf8("姓名"),
        QString::fromUtf8("年龄"),
        QString::fromUtf8("类型"),
        QString::fromUtf8("联系方式")
    });
    patientTable->horizontalHeader()->setStretchLastSection(true);
    patientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    patientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(patientTable, &QTableWidget::cellDoubleClicked, this, &AdminWidget::onPatientDoubleClicked);
    mainLayout->addWidget(patientTable);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnAdd = new QPushButton(QString::fromUtf8("新增"));
    QPushButton* btnMod = new QPushButton(QString::fromUtf8("修改"));
    QPushButton* btnDel = new QPushButton(QString::fromUtf8("删除"));
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnMod);
    btnLayout->addWidget(btnDel);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(searchBtn, &QPushButton::clicked, this, &AdminWidget::onSearchPatient);
    connect(clearBtn, &QPushButton::clicked, [this]() {
        patientSearch->clear();
        refreshPatientTable();
    });
    connect(btnAdd, &QPushButton::clicked, this, &AdminWidget::onAddPatient);
    connect(btnMod, &QPushButton::clicked, this, &AdminWidget::onModifyPatient);
    connect(btnDel, &QPushButton::clicked, this, &AdminWidget::onDeletePatient);

    tabWidget->addTab(panel, QString::fromUtf8("患者管理"));
}

void AdminWidget::refreshPatientTable(const QString& filter) {
    patientTable->setRowCount(0);
    int row = 0;
    for (PatientNode* p = patientHead; p != nullptr; p = p->next) {
        if (!filter.isEmpty()) {
            QString id = QString::fromStdString(p->patientID);
            QString name = QString::fromStdString(p->name);
            if (!id.contains(filter, Qt::CaseInsensitive) && !name.contains(filter, Qt::CaseInsensitive))
                continue;
        }
        patientTable->insertRow(row);
        patientTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p->patientID)));
        patientTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p->name)));
        patientTable->setItem(row, 2, new QTableWidgetItem(QString::number(p->age)));
        patientTable->setItem(row, 3, new QTableWidgetItem(
            p->type == PATIENT_OUTPATIENT ? QString::fromUtf8("门诊") : QString::fromUtf8("住院")));
        patientTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(p->contact)));
        row++;
    }
    patientTable->resizeColumnsToContents();
}

void AdminWidget::onSearchPatient() {
    refreshPatientTable(patientSearch->text().trimmed());
}

void AdminWidget::onAddPatient() {
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("新增患者"));
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit();
    nameEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("姓名:"), nameEdit);

    QSpinBox* ageSpin = new QSpinBox();
    ageSpin->setRange(0, 150);
    ageSpin->setValue(30);
    form.addRow(QString::fromUtf8("年龄:"), ageSpin);

    QLineEdit* contactEdit = new QLineEdit();
    contactEdit->setMaxLength(MAX_CONTACT_LEN);
    form.addRow(QString::fromUtf8("联系方式:"), contactEdit);

    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItem(QString::fromUtf8("门诊患者"), PATIENT_OUTPATIENT);
    typeCombo->addItem(QString::fromUtf8("住院患者"), PATIENT_INPATIENT);
    form.addRow(QString::fromUtf8("患者类型:"), typeCombo);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("患者姓名不能为空!"));
            return;
        }
        QString contact = contactEdit->text().trimmed();
        if (contact.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("联系方式不能为空!"));
            return;
        }

        PatientNode* node = new PatientNode();
        node->patientID = generatePatientID();
        node->name = name.toStdString();
        node->age = ageSpin->value();
        node->contact = contact.toStdString();
        node->type = typeCombo->currentData().toInt();
        insertPatientHead(patientHead, node);
        refreshPatientTable();
    }
}

void AdminWidget::onModifyPatient() {
    int curRow = patientTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一名患者!"));
        return;
    }
    QString patID = patientTable->item(curRow, 0)->text();
    PatientNode* p = findPatientByID(patientHead, patID.toStdString());
    if (!p) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("患者不存在!"));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("修改患者"));
    QFormLayout form(&dlg);

    QLineEdit* nameEdit = new QLineEdit(QString::fromStdString(p->name));
    nameEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("姓名:"), nameEdit);

    QSpinBox* ageSpin = new QSpinBox();
    ageSpin->setRange(0, 150);
    ageSpin->setValue(p->age);
    form.addRow(QString::fromUtf8("年龄:"), ageSpin);

    QLineEdit* contactEdit = new QLineEdit(QString::fromStdString(p->contact));
    contactEdit->setMaxLength(MAX_CONTACT_LEN);
    form.addRow(QString::fromUtf8("联系方式:"), contactEdit);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("患者姓名不能为空!"));
            return;
        }
        QString contact = contactEdit->text().trimmed();
        if (contact.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("联系方式不能为空!"));
            return;
        }
        p->name = name.toStdString();
        p->age = ageSpin->value();
        p->contact = contact.toStdString();
        refreshPatientTable();
    }
}

void AdminWidget::onDeletePatient() {
    int curRow = patientTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一名患者!"));
        return;
    }
    QString patID = patientTable->item(curRow, 0)->text();
    PatientNode* p = findPatientByID(patientHead, patID.toStdString());
    if (!p) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("患者不存在!"));
        return;
    }

    HospitalizationNode* activeHosp = findActiveHospitalizationByPatient(hospHead, p->patientID);
    if (activeHosp) {
        QMessageBox::warning(this, QString::fromUtf8("无法删除"),
            QString::fromUtf8(("患者 " + p->name + " 目前正在住院 (住院ID: " + activeHosp->hospID + ")，无法删除!").c_str()));
        return;
    }

    int ret = QMessageBox::question(this, QString::fromUtf8("确认删除"),
        QString::fromUtf8(("确认逻辑删除患者 " + p->name + "?").c_str()),
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        deletePatientByID(patientHead, p->patientID);
        refreshPatientTable();
    }
}

void AdminWidget::onPatientDoubleClicked(int row, int /*col*/) {
    if (row < 0 || row >= patientTable->rowCount()) return;
    QString patID = patientTable->item(row, 0)->text();
    PatientNode* p = findPatientByID(patientHead, patID.toStdString());
    if (!p) return;

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8(("患者详情 - " + p->name + " (" + p->patientID + ")").c_str()));
    dlg.resize(750, 550);
    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QLabel* infoLabel = new QLabel(QString::fromUtf8(
        ("姓名: " + p->name + "  年龄: " + std::to_string(p->age)
         + "  联系方式: " + p->contact
         + "  类型: " + (p->type == PATIENT_OUTPATIENT ? "门诊" : "住院")).c_str()));
    infoLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
    layout->addWidget(infoLabel);

    QTabWidget* detailTabs = new QTabWidget();
    layout->addWidget(detailTabs);

    // Tab: Registrations
    {
        QTableWidget* tab = new QTableWidget(0, 5);
        tab->setHorizontalHeaderLabels({
            QString::fromUtf8("挂号ID"), QString::fromUtf8("科室"), QString::fromUtf8("医生"),
            QString::fromUtf8("日期"), QString::fromUtf8("状态")
        });
        tab->horizontalHeader()->setStretchLastSection(true);
        tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
        int rr = 0;
        for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
            if (r->patientID != p->patientID) continue;
            tab->insertRow(rr);
            tab->setItem(rr, 0, new QTableWidgetItem(QString::fromStdString(r->regID)));
            DepartmentNode* dept = findDepartmentByID(deptHead, r->departmentID);
            tab->setItem(rr, 1, new QTableWidgetItem(
                dept ? QString::fromStdString(dept->name) : QString::fromStdString(r->departmentID)));
            DoctorNode* doc = findDoctorByID(doctorHead, r->doctorID);
            tab->setItem(rr, 2, new QTableWidgetItem(
                doc ? QString::fromStdString(doc->name) : QString::fromStdString(r->doctorID)));
            tab->setItem(rr, 3, new QTableWidgetItem(QString("Day%1").arg(r->regDay)));
            tab->setItem(rr, 4, new QTableWidgetItem(QString::fromUtf8(getRegStatusStr(r->status))));
            rr++;
        }
        tab->resizeColumnsToContents();
        detailTabs->addTab(tab, QString::fromUtf8("挂号记录"));
    }

    // Tab: Consultations
    {
        QTableWidget* tab = new QTableWidget(0, 5);
        tab->setHorizontalHeaderLabels({
            QString::fromUtf8("接诊ID"), QString::fromUtf8("医生"), QString::fromUtf8("主诉"),
            QString::fromUtf8("诊断"), QString::fromUtf8("日期")
        });
        tab->horizontalHeader()->setStretchLastSection(true);
        tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
        int cr = 0;
        for (ConsultationNode* c = consultHead; c != nullptr; c = c->next) {
            if (c->patientID != p->patientID) continue;
            tab->insertRow(cr);
            tab->setItem(cr, 0, new QTableWidgetItem(QString::fromStdString(c->consultID)));
            DoctorNode* doc = findDoctorByID(doctorHead, c->doctorID);
            tab->setItem(cr, 1, new QTableWidgetItem(
                doc ? QString::fromStdString(doc->name) : QString::fromStdString(c->doctorID)));
            tab->setItem(cr, 2, new QTableWidgetItem(QString::fromStdString(c->complaint)));
            tab->setItem(cr, 3, new QTableWidgetItem(QString::fromStdString(c->diagnosis)));
            tab->setItem(cr, 4, new QTableWidgetItem(QString("Day%1").arg(c->consultDay)));
            cr++;
        }
        tab->resizeColumnsToContents();
        detailTabs->addTab(tab, QString::fromUtf8("接诊记录"));
    }

    // Tab: Prescriptions
    {
        QTableWidget* tab = new QTableWidget(0, 4);
        tab->setHorizontalHeaderLabels({
            QString::fromUtf8("处方ID"), QString::fromUtf8("金额"), QString::fromUtf8("日期"), QString::fromUtf8("状态")
        });
        tab->horizontalHeader()->setStretchLastSection(true);
        tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
        int pr = 0;
        for (PrescriptionNode* pn = prescHead; pn != nullptr; pn = pn->next) {
            if (pn->patientID != p->patientID) continue;
            tab->insertRow(pr);
            tab->setItem(pr, 0, new QTableWidgetItem(QString::fromStdString(pn->prescID)));
            tab->setItem(pr, 1, new QTableWidgetItem(QString::number(pn->totalAmount, 'f', 2)));
            tab->setItem(pr, 2, new QTableWidgetItem(QString("Day%1").arg(pn->prescDay)));
            tab->setItem(pr, 3, new QTableWidgetItem(QString::fromUtf8(getPrescStatusStr(pn->status))));
            pr++;
        }
        tab->resizeColumnsToContents();
        detailTabs->addTab(tab, QString::fromUtf8("处方记录"));
    }

    // Tab: Examinations
    {
        QTableWidget* tab = new QTableWidget(0, 4);
        tab->setHorizontalHeaderLabels({
            QString::fromUtf8("检查ID"), QString::fromUtf8("检查项目"), QString::fromUtf8("费用"), QString::fromUtf8("日期")
        });
        tab->horizontalHeader()->setStretchLastSection(true);
        tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
        int er = 0;
        for (ExaminationNode* e = examHead; e != nullptr; e = e->next) {
            if (e->patientID != p->patientID) continue;
            tab->insertRow(er);
            tab->setItem(er, 0, new QTableWidgetItem(QString::fromStdString(e->examID)));
            tab->setItem(er, 1, new QTableWidgetItem(QString::fromStdString(e->itemName)));
            tab->setItem(er, 2, new QTableWidgetItem(QString::number(e->cost, 'f', 2)));
            tab->setItem(er, 3, new QTableWidgetItem(QString("Day%1").arg(e->examDay)));
            er++;
        }
        tab->resizeColumnsToContents();
        detailTabs->addTab(tab, QString::fromUtf8("检查记录"));
    }

    // Tab: Hospitalization
    {
        QTableWidget* tab = new QTableWidget(0, 6);
        tab->setHorizontalHeaderLabels({
            QString::fromUtf8("住院ID"), QString::fromUtf8("病房"), QString::fromUtf8("床位"),
            QString::fromUtf8("入院日"), QString::fromUtf8("押金"), QString::fromUtf8("状态")
        });
        tab->horizontalHeader()->setStretchLastSection(true);
        tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
        int hr = 0;
        for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
            if (h->patientID != p->patientID) continue;
            tab->insertRow(hr);
            tab->setItem(hr, 0, new QTableWidgetItem(QString::fromStdString(h->hospID)));
            tab->setItem(hr, 1, new QTableWidgetItem(QString::fromStdString(h->wardID)));
            tab->setItem(hr, 2, new QTableWidgetItem(QString::number(h->bedNo + 1)));
            tab->setItem(hr, 3, new QTableWidgetItem(QString("Day%1").arg(h->admitDay)));
            tab->setItem(hr, 4, new QTableWidgetItem(QString::number(h->deposit, 'f', 2)));
            tab->setItem(hr, 5, new QTableWidgetItem(QString::fromUtf8(getHospStatusStr(h->status))));
            hr++;
        }
        tab->resizeColumnsToContents();
        detailTabs->addTab(tab, QString::fromUtf8("住院记录"));
    }

    dlg.exec();
}

// ==================== Tab 4: Registration Panel ====================
void AdminWidget::setupRegPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);

    // Top: selection row
    QHBoxLayout* selLayout = new QHBoxLayout();
    selLayout->addWidget(new QLabel(QString::fromUtf8("科室:")));
    regDeptCombo = new QComboBox();
    regDeptCombo->addItem(QString::fromUtf8("-- 选择科室 --"), QVariant(QString()));
    for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
        regDeptCombo->addItem(QString::fromStdString(d->name), QVariant(QString::fromStdString(d->deptID)));
    }
    selLayout->addWidget(regDeptCombo);

    selLayout->addWidget(new QLabel(QString::fromUtf8("医生:")));
    regDoctorCombo = new QComboBox();
    regDoctorCombo->addItem(QString::fromUtf8("-- 先选科室 --"), QVariant(QString()));
    selLayout->addWidget(regDoctorCombo);

    selLayout->addWidget(new QLabel(QString::fromUtf8("患者ID:")));
    regPatientSearch = new QLineEdit();
    regPatientSearch->setPlaceholderText(QString::fromUtf8("输入患者ID..."));
    selLayout->addWidget(regPatientSearch);

    QPushButton* regBtn = new QPushButton(QString::fromUtf8("挂号"));
    selLayout->addWidget(regBtn);
    mainLayout->addLayout(selLayout);

    // Info label
    regDoctorLoad = new QLabel(QString::fromUtf8("请选择科室和医生"));
    regDoctorLoad->setStyleSheet("color: #555;");
    mainLayout->addWidget(regDoctorLoad);

    // Today's registration table
    regTable = new QTableWidget(0, 6);
    regTable->setHorizontalHeaderLabels({
        QString::fromUtf8("挂号ID"),
        QString::fromUtf8("患者"),
        QString::fromUtf8("科室"),
        QString::fromUtf8("医生"),
        QString::fromUtf8("日期"),
        QString::fromUtf8("状态")
    });
    regTable->horizontalHeader()->setStretchLastSection(true);
    regTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    regTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(regTable);

    // Action buttons for registrations
    QHBoxLayout* regActionLayout = new QHBoxLayout();
    QPushButton* btnSeen = new QPushButton(QString::fromUtf8("标记已就诊"));
    QPushButton* btnCancel = new QPushButton(QString::fromUtf8("作废"));
    regActionLayout->addWidget(btnSeen);
    regActionLayout->addWidget(btnCancel);
    regActionLayout->addStretch();
    mainLayout->addLayout(regActionLayout);

    connect(regDeptCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &AdminWidget::onDeptSelectedForReg);
    connect(regDoctorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &AdminWidget::onDoctorSelectedForReg);
    connect(regBtn, &QPushButton::clicked, this, &AdminWidget::onRegister);

    connect(btnSeen, &QPushButton::clicked, [this]() {
        int curRow = regTable->currentRow();
        if (curRow < 0) {
            QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一条挂号记录!"));
            return;
        }
        QString regID = regTable->item(curRow, 0)->text();
        RegistrationNode* r = findRegistrationByID(regHead, regID.toStdString());
        if (!r) return;
        if (r->status != STATUS_PENDING) {
            QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("只能将待就诊记录标记为已就诊!"));
            return;
        }
        r->status = STATUS_SEEN;
        refreshRegTable();
    });
    connect(btnCancel, &QPushButton::clicked, [this]() {
        int curRow = regTable->currentRow();
        if (curRow < 0) {
            QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一条挂号记录!"));
            return;
        }
        QString regID = regTable->item(curRow, 0)->text();
        RegistrationNode* r = findRegistrationByID(regHead, regID.toStdString());
        if (!r) return;
        if (r->status != STATUS_PENDING) {
            QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("只能作废待就诊的记录!"));
            return;
        }
        r->status = 3;  // cancelled
        refreshRegTable();
    });

    tabWidget->addTab(panel, QString::fromUtf8("挂号管理"));
}

void AdminWidget::onDeptSelectedForReg() {
    QString deptID = regDeptCombo->currentData().toString();
    regDoctorCombo->clear();
    regDoctorCombo->addItem(QString::fromUtf8("-- 选择医生 --"), QVariant(QString()));

    if (deptID.isEmpty()) {
        regDoctorLoad->setText(QString::fromUtf8("请选择科室和医生"));
        return;
    }

    int day = todayDayOfWeek();
    int count = 0;
    for (DoctorNode* doc = doctorHead; doc != nullptr; doc = doc->next) {
        if (doc->departmentID == deptID.toStdString() && doctorWorksOnDay(doc, day)) {
            regDoctorCombo->addItem(
                QString::fromStdString(doc->name + " (" + getLevelName(doc->level) + ")"),
                QVariant(QString::fromStdString(doc->doctorID)));
            count++;
        }
    }

    regDoctorLoad->setText(QString::fromUtf8(
        ("今日(" + std::string(WEEKDAY_NAMES[day - 1]) + ")该科室有 " + std::to_string(count) + " 名医生出诊").c_str()));
}

void AdminWidget::onDoctorSelectedForReg() {
    QString doctorID = regDoctorCombo->currentData().toString();
    if (doctorID.isEmpty()) {
        regDoctorLoad->setText(QString::fromUtf8("请选择科室和医生"));
        return;
    }

    // Count today's registrations for this doctor
    int todayCount = 0;
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->doctorID == doctorID.toStdString() && r->regDay == weekday && r->status == STATUS_PENDING) {
            todayCount++;
        }
    }

    regDoctorLoad->setText(QString::fromUtf8(
        ("当前医生今日已挂 " + std::to_string(todayCount) + " 人 (限号20)").c_str()));
}

void AdminWidget::onRegister() {
    QString deptID = regDeptCombo->currentData().toString();
    QString doctorID = regDoctorCombo->currentData().toString();
    QString patientID = regPatientSearch->text().trimmed();

    if (deptID.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请选择科室!"));
        return;
    }
    if (doctorID.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请选择医生!"));
        return;
    }
    if (patientID.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入患者ID!"));
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, patientID.toStdString());
    if (!patient) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("患者不存在!"));
        return;
    }

    DoctorNode* doctor = findDoctorByID(doctorHead, doctorID.toStdString());
    if (!doctor) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("医生不存在!"));
        return;
    }

    int day = todayDayOfWeek();
    if (doctor->departmentID != deptID.toStdString()) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("该医生不属于所选科室!"));
        return;
    }
    if (!doctorWorksOnDay(doctor, day)) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("该医生今日不出诊!"));
        return;
    }

    // Check duplicate registration
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->patientID == patientID.toStdString()
            && r->departmentID == deptID.toStdString()
            && r->regDay == weekday && r->status == STATUS_PENDING) {
            QMessageBox::warning(this, QString::fromUtf8("重复挂号"),
                QString::fromUtf8("该患者今天已在该科室挂号，不可重复挂号!"));
            return;
        }
    }

    // Check doctor load limit (20)
    int todayCount = 0;
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->doctorID == doctorID.toStdString() && r->regDay == weekday && r->status == STATUS_PENDING) {
            todayCount++;
        }
    }
    if (todayCount >= 20) {
        QMessageBox::warning(this, QString::fromUtf8("限号"), QString::fromUtf8("该医生今日已满号(20)人，无法继续挂号!"));
        return;
    }

    RegistrationNode* node = new RegistrationNode();
    node->regID = generateRegID(weekday);
    node->patientID = patientID.toStdString();
    node->departmentID = deptID.toStdString();
    node->doctorID = doctorID.toStdString();
    node->regDay = weekday;
    node->status = STATUS_PENDING;
    insertRegistrationTail(regHead, node);

    QMessageBox::information(this, QString::fromUtf8("挂号成功"),
        QString::fromUtf8(("挂号ID: " + node->regID + "\n患者: " + patient->name + "\n医生: " + doctor->name).c_str()));

    refreshRegTable();
    onDoctorSelectedForReg();
}

void AdminWidget::refreshRegTable() {
    regTable->setRowCount(0);
    int row = 0;
    unsigned int today = weekday;
    for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
        if (r->regDay != today) continue;
        regTable->insertRow(row);
        regTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r->regID)));
        PatientNode* p = findPatientByID(patientHead, r->patientID);
        regTable->setItem(row, 1, new QTableWidgetItem(
            p ? QString::fromStdString(p->name) : QString::fromStdString(r->patientID)));
        DepartmentNode* dept = findDepartmentByID(deptHead, r->departmentID);
        regTable->setItem(row, 2, new QTableWidgetItem(
            dept ? QString::fromStdString(dept->name) : QString::fromStdString(r->departmentID)));
        DoctorNode* doc = findDoctorByID(doctorHead, r->doctorID);
        regTable->setItem(row, 3, new QTableWidgetItem(
            doc ? QString::fromStdString(doc->name) : QString::fromStdString(r->doctorID)));
        regTable->setItem(row, 4, new QTableWidgetItem(QString("Day%1").arg(r->regDay)));
        regTable->setItem(row, 5, new QTableWidgetItem(QString::fromUtf8(getRegStatusStr(r->status))));
        row++;
    }
    regTable->resizeColumnsToContents();
}

// ==================== Tab 5: Hospitalization Panel ====================
void AdminWidget::setupHospPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);

    // Left side: Ward-Bed grid
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel(QString::fromUtf8("病房-床位状态 (绿色=空闲, 红色=占用):")));
    wardGridTable = new QTableWidget();
    wardGridTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    wardGridTable->setSelectionBehavior(QAbstractItemView::SelectItems);
    leftLayout->addWidget(wardGridTable);
    splitter->addWidget(leftWidget);

    // Right side: Admission controls
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);

    QGroupBox* admitGroup = new QGroupBox(QString::fromUtf8("办理入院"));
    QFormLayout* admitForm = new QFormLayout(admitGroup);

    hospWardCombo = new QComboBox();
    hospWardCombo->addItem(QString::fromUtf8("-- 选择病房 --"), QVariant(QString()));
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        int freeBeds = getFreeBedCount(w);
        if (freeBeds > 0) {
            hospWardCombo->addItem(
                QString::fromStdString(w->wardID + " (" + getWardTypeName(w->type) + ", 空闲" + std::to_string(freeBeds) + "床)"),
                QVariant(QString::fromStdString(w->wardID)));
        }
    }
    admitForm->addRow(QString::fromUtf8("病房:"), hospWardCombo);

    hospBedCombo = new QComboBox();
    hospBedCombo->addItem(QString::fromUtf8("-- 先选病房 --"), QVariant(-1));
    admitForm->addRow(QString::fromUtf8("床位:"), hospBedCombo);

    hospPatientSearch = new QLineEdit();
    hospPatientSearch->setPlaceholderText(QString::fromUtf8("输入门诊患者ID..."));
    admitForm->addRow(QString::fromUtf8("患者ID:"), hospPatientSearch);

    admitForm->addRow(new QLabel(QString::fromUtf8("主管医生 (可多选):")));
    hospDoctorList = new QTableWidget(0, 3);
    hospDoctorList->setHorizontalHeaderLabels({
        QString::fromUtf8("选择"), QString::fromUtf8("医生ID"), QString::fromUtf8("姓名")
    });
    hospDoctorList->horizontalHeader()->setStretchLastSection(true);
    hospDoctorList->setSelectionMode(QAbstractItemView::NoSelection);
    admitForm->addRow(hospDoctorList);

    QPushButton* admitBtn = new QPushButton(QString::fromUtf8("办理住院"));
    admitForm->addRow(admitBtn);

    rightLayout->addWidget(admitGroup);
    rightLayout->addStretch();
    splitter->addWidget(rightWidget);

    mainLayout->addWidget(splitter);

    // Below: Inpatients table
    QLabel* hospLabel = new QLabel(QString::fromUtf8("当前在院患者:"));
    hospLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(hospLabel);

    hospTable = new QTableWidget(0, 8);
    hospTable->setHorizontalHeaderLabels({
        QString::fromUtf8("住院ID"), QString::fromUtf8("患者ID"), QString::fromUtf8("姓名"),
        QString::fromUtf8("病房"), QString::fromUtf8("床位"), QString::fromUtf8("入院日"),
        QString::fromUtf8("已住天数"), QString::fromUtf8("押金")
    });
    hospTable->horizontalHeader()->setStretchLastSection(true);
    hospTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    hospTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(hospTable, &QTableWidget::cellDoubleClicked, this, &AdminWidget::onHospPatientDoubleClicked);
    mainLayout->addWidget(hospTable);

    QPushButton* dischargeBtn = new QPushButton(QString::fromUtf8("办理出院"));
    mainLayout->addWidget(dischargeBtn);

    // Connections
    connect(hospWardCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]() {
        QString wardID = hospWardCombo->currentData().toString();
        hospBedCombo->clear();
        if (wardID.isEmpty()) {
            hospBedCombo->addItem(QString::fromUtf8("-- 先选病房 --"), QVariant(-1));
        } else {
            WardNode* w = findWardByID(wardHead, wardID.toStdString());
            if (w) {
                for (int i = 0; i < w->bedCount; i++) {
                    if (w->bedStatus[i].empty()) {
                        hospBedCombo->addItem(
                            QString::fromUtf8((std::to_string(i + 1) + "号床 (空闲)").c_str()),
                            QVariant(i));
                    }
                }
                if (hospBedCombo->count() == 0) {
                    hospBedCombo->addItem(QString::fromUtf8("无空闲床位"), QVariant(-1));
                }
            }
        }
        // Refresh doctor list filtered by ward's department
        hospDoctorList->setRowCount(0);
        if (!wardID.isEmpty()) {
            WardNode* w = findWardByID(wardHead, wardID.toStdString());
            if (w) {
                int dr = 0;
                for (DoctorNode* doc = doctorHead; doc != nullptr; doc = doc->next) {
                    if (doc->departmentID == w->departmentID) {
                        hospDoctorList->insertRow(dr);
                        QTableWidgetItem* chkItem = new QTableWidgetItem();
                        chkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                        chkItem->setCheckState(Qt::Unchecked);
                        hospDoctorList->setItem(dr, 0, chkItem);
                        hospDoctorList->setItem(dr, 1, new QTableWidgetItem(
                            QString::fromStdString(doc->doctorID)));
                        hospDoctorList->setItem(dr, 2, new QTableWidgetItem(
                            QString::fromStdString(doc->name)));
                        dr++;
                    }
                }
                hospDoctorList->resizeColumnsToContents();
            }
        }
    });

    connect(admitBtn, &QPushButton::clicked, this, &AdminWidget::onAdmitPatient);
    connect(dischargeBtn, &QPushButton::clicked, this, &AdminWidget::onDischargePatient);

    tabWidget->addTab(panel, QString::fromUtf8("住院管理"));
}

void AdminWidget::refreshWardGrid() {
    int totalWards = 0;
    int maxBeds = 0;
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        totalWards++;
        if (w->bedCount > maxBeds) maxBeds = w->bedCount;
    }
    if (maxBeds == 0) maxBeds = 1;

    wardGridTable->setRowCount(totalWards);
    wardGridTable->setColumnCount(1 + maxBeds);
    QStringList headers;
    headers << QString::fromUtf8("病房");
    for (int i = 0; i < maxBeds; i++) {
        headers << QString::fromUtf8((std::to_string(i + 1) + "号").c_str());
    }
    wardGridTable->setHorizontalHeaderLabels(headers);

    int row = 0;
    for (WardNode* w = wardHead; w != nullptr; w = w->next) {
        DepartmentNode* dept = findDepartmentByID(deptHead, w->departmentID);
        QString wardLabel = QString::fromStdString(
            w->wardID + "\n" + (dept ? dept->name : "?") + "\n" + getWardTypeName(w->type));
        wardGridTable->setItem(row, 0, new QTableWidgetItem(wardLabel));

        for (int i = 0; i < maxBeds; i++) {
            QTableWidgetItem* item = new QTableWidgetItem();
            if (i < w->bedCount) {
                if (w->bedStatus[i].empty()) {
                    item->setText(QString::fromUtf8("空闲"));
                    item->setBackground(QColor(144, 238, 144));
                } else {
                    PatientNode* p = findPatientByID(patientHead, w->bedStatus[i]);
                    item->setText(p ? QString::fromStdString(p->name) : QString::fromStdString(w->bedStatus[i]));
                    item->setBackground(QColor(255, 140, 140));
                }
            } else {
                item->setBackground(QColor(200, 200, 200));
                item->setFlags(Qt::NoItemFlags);
            }
            wardGridTable->setItem(row, i + 1, item);
        }
        row++;
    }
    wardGridTable->resizeColumnsToContents();
    wardGridTable->resizeRowsToContents();
}

void AdminWidget::onAdmitPatient() {
    QString wardID = hospWardCombo->currentData().toString();
    int bedIdx = hospBedCombo->currentData().toInt();
    QString patientID = hospPatientSearch->text().trimmed();

    if (wardID.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请选择病房!"));
        return;
    }
    if (bedIdx < 0) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请选择床位!"));
        return;
    }
    if (patientID.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("请输入患者ID!"));
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, patientID.toStdString());
    if (!patient) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("患者不存在!"));
        return;
    }
    if (patient->type != PATIENT_OUTPATIENT) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("该患者不是门诊患者，无法办理入院!"));
        return;
    }

    if (findActiveHospitalizationByPatient(hospHead, patientID.toStdString())) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("该患者已住院!"));
        return;
    }

    WardNode* ward = findWardByID(wardHead, wardID.toStdString());
    if (!ward) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("病房不存在!"));
        return;
    }
    if (bedIdx >= ward->bedCount || !ward->bedStatus[bedIdx].empty()) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("床位已被占用!"));
        return;
    }

    // Ask for deposit via input dialog
    bool ok;
    double deposit = QInputDialog::getDouble(this, QString::fromUtf8("入院押金"),
        QString::fromUtf8("请输入押金金额:"), 5000.00, 0, 999999.99, 2, &ok);
    if (!ok) return;

    // Collect doctor IDs from the doctor list checkboxes
    std::string docIDs[10];
    int docCount = 0;
    for (int i = 0; i < hospDoctorList->rowCount() && docCount < 10; i++) {
        QTableWidgetItem* chkItem = hospDoctorList->item(i, 0);
        if (chkItem && chkItem->checkState() == Qt::Checked) {
            docIDs[docCount++] = hospDoctorList->item(i, 1)->text().toStdString();
        }
    }
    if (docCount == 0) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("请至少选择一名主管医生!"));
        return;
    }

    // Allocate bed
    int allocResult = getAllocateBed(ward, patientID.toStdString());
    if (allocResult < 0) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("床位分配失败!"));
        return;
    }

    // Update patient
    patient->type = PATIENT_INPATIENT;
    patient->deposit = deposit;
    patient->admitDays = 0;
    patient->bedID = wardID.toStdString() + "-" + std::to_string(allocResult + 1);

    // Create hospitalization record
    HospitalizationNode* hosp = new HospitalizationNode();
    hosp->hospID = generateHospID(weekday);
    hosp->patientID = patientID.toStdString();
    hosp->wardID = wardID.toStdString();
    hosp->bedNo = allocResult;
    hosp->doctorCount = docCount;
    for (int i = 0; i < docCount; i++) hosp->doctorIDs[i] = docIDs[i];
    hosp->admitDay = weekday;
    hosp->dischargeDay = 0;
    hosp->deposit = deposit;
    hosp->status = 0;
    insertHospitalizationTail(hospHead, hosp);

    QMessageBox::information(this, QString::fromUtf8("入院成功"),
        QString::fromUtf8(("住院ID: " + hosp->hospID + "\n患者: " + patient->name
                           + "\n病房: " + wardID.toStdString() + " " + std::to_string(allocResult + 1) + "号床"
                           + "\n押金: " + std::to_string(deposit) + " 元").c_str()));

    refreshWardGrid();
    refreshHospTable();
}

void AdminWidget::onDischargePatient() {
    int curRow = hospTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一名在院患者!"));
        return;
    }
    QString hospID = hospTable->item(curRow, 0)->text();
    HospitalizationNode* hosp = findHospitalizationByID(hospHead, hospID.toStdString());
    if (!hosp || hosp->status != 0) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("未找到有效的住院记录!"));
        return;
    }

    PatientNode* patient = findPatientByID(patientHead, hosp->patientID);
    WardNode* ward = findWardByID(wardHead, hosp->wardID);

    int stayDays = weekday - hosp->admitDay;
    if (stayDays < 1) stayDays = 1;
    double totalCharge = HOSPITAL_DAILY_FEE * stayDays;
    double balance = hosp->deposit - totalCharge;

    QString info = QString::fromUtf8(
        ("住院ID: " + hosp->hospID + "\n"
         + "患者: " + (patient ? patient->name : "?") + "\n"
         + "病房: " + hosp->wardID + " " + std::to_string(hosp->bedNo + 1) + "号床\n"
         + "入院Day: " + std::to_string(hosp->admitDay) + "\n"
         + "住院天数: " + std::to_string(stayDays) + " 天\n"
         + "每日费用: " + std::to_string(HOSPITAL_DAILY_FEE) + " 元\n"
         + "总费用: " + std::to_string(totalCharge) + " 元\n"
         + "押金: " + std::to_string(hosp->deposit) + " 元\n"
         + (balance >= 0 ? "应退: " + std::to_string(balance) + " 元" : "应补: " + std::to_string(-balance) + " 元")
         + "\n\n确认办理出院?").c_str());

    int ret = QMessageBox::question(this, QString::fromUtf8("出院费用结算"), info,
        QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) return;

    // Release bed
    if (ward) {
        releaseBed(ward, hosp->bedNo);
    }

    // Update patient
    if (patient) {
        patient->type = PATIENT_OUTPATIENT;
        patient->bedID = "";
    }

    // Update hosp record
    hosp->dischargeDay = weekday;
    hosp->status = 1;

    // Update hospital funds
    extern double money;
    money += totalCharge;

    QMessageBox::information(this, QString::fromUtf8("出院成功"), QString::fromUtf8("出院办理成功!"));

    refreshWardGrid();
    refreshHospTable();
}

void AdminWidget::onHospPatientDoubleClicked(int row, int /*col*/) {
    if (row < 0 || row >= hospTable->rowCount()) return;
    QString hospID = hospTable->item(row, 0)->text();
    HospitalizationNode* hosp = findHospitalizationByID(hospHead, hospID.toStdString());
    if (!hosp) return;

    PatientNode* patient = findPatientByID(patientHead, hosp->patientID);
    WardNode* ward = findWardByID(wardHead, hosp->wardID);

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8(("住院详情 - " + (patient ? patient->name : "?")).c_str()));
    dlg.resize(500, 400);
    QVBoxLayout* layout = new QVBoxLayout(&dlg);

    QString info = QString::fromUtf8(
        ("住院ID: " + hosp->hospID + "\n"
         + "患者: " + (patient ? patient->name : "?") + " (" + hosp->patientID + ")\n"
         + "病房: " + hosp->wardID + " " + std::to_string(hosp->bedNo + 1) + "号床\n"
         + "入院日: Day" + std::to_string(hosp->admitDay) + "\n"
         + "已住天数: " + std::to_string(weekday - hosp->admitDay) + " 天\n"
         + "押金: " + std::to_string(hosp->deposit) + " 元\n"
         + "状态: " + getHospStatusStr(hosp->status)).c_str());

    QLabel* infoLabel = new QLabel(info);
    infoLabel->setStyleSheet("font-size: 13px;");
    layout->addWidget(infoLabel);

    // List responsible doctors
    layout->addWidget(new QLabel(QString::fromUtf8("\n主管医生:")));
    for (int i = 0; i < hosp->doctorCount; i++) {
        DoctorNode* doc = findDoctorByID(doctorHead, hosp->doctorIDs[i]);
        layout->addWidget(new QLabel(QString::fromUtf8(
            ("  " + hosp->doctorIDs[i] + " - " + (doc ? doc->name : "未知")).c_str())));
    }

    QDialogButtonBox buttons(QDialogButtonBox::Ok);
    layout->addWidget(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    dlg.exec();
}

void AdminWidget::refreshHospTable() {
    hospTable->setRowCount(0);
    int row = 0;
    for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
        if (h->status != 0) continue;
        hospTable->insertRow(row);
        hospTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(h->hospID)));
        hospTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(h->patientID)));
        PatientNode* p = findPatientByID(patientHead, h->patientID);
        hospTable->setItem(row, 2, new QTableWidgetItem(
            p ? QString::fromStdString(p->name) : QString::fromUtf8("未知")));
        hospTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(h->wardID)));
        hospTable->setItem(row, 4, new QTableWidgetItem(QString::number(h->bedNo + 1)));
        hospTable->setItem(row, 5, new QTableWidgetItem(QString("Day%1").arg(h->admitDay)));
        int stayDays = weekday - h->admitDay;
        if (stayDays < 0) stayDays = 0;
        hospTable->setItem(row, 6, new QTableWidgetItem(QString::number(stayDays)));
        hospTable->setItem(row, 7, new QTableWidgetItem(
            QString::number(p ? p->deposit : h->deposit, 'f', 2)));
        row++;
    }
    hospTable->resizeColumnsToContents();
}

// ==================== Tab 6: Medicine Panel ====================
void AdminWidget::setupMedPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);

    medTable = new QTableWidget(0, 5);
    medTable->setHorizontalHeaderLabels({
        QString::fromUtf8("药品ID"),
        QString::fromUtf8("商品名"),
        QString::fromUtf8("通用名"),
        QString::fromUtf8("库存量"),
        QString::fromUtf8("消耗量")
    });
    medTable->horizontalHeader()->setStretchLastSection(true);
    medTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    medTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(medTable);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnAdd = new QPushButton(QString::fromUtf8("新增"));
    QPushButton* btnMod = new QPushButton(QString::fromUtf8("修改"));
    QPushButton* btnDel = new QPushButton(QString::fromUtf8("删除"));
    QPushButton* btnIn  = new QPushButton(QString::fromUtf8("入库"));
    QPushButton* btnOut = new QPushButton(QString::fromUtf8("出库"));
    btnLayout->addWidget(btnAdd);
    btnLayout->addWidget(btnMod);
    btnLayout->addWidget(btnDel);
    btnLayout->addWidget(btnIn);
    btnLayout->addWidget(btnOut);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(btnAdd, &QPushButton::clicked, this, &AdminWidget::onAddMedicine);
    connect(btnMod, &QPushButton::clicked, this, &AdminWidget::onModifyMedicine);
    connect(btnDel, &QPushButton::clicked, this, &AdminWidget::onDeleteMedicine);
    connect(btnIn,  &QPushButton::clicked, this, &AdminWidget::onStockIn);
    connect(btnOut, &QPushButton::clicked, this, &AdminWidget::onStockOut);

    tabWidget->addTab(panel, QString::fromUtf8("药品管理"));
}

void AdminWidget::refreshMedTable() {
    medTable->setRowCount(0);
    int row = 0;
    for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
        medTable->insertRow(row);
        medTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m->medID)));
        medTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(m->tradeName)));
        medTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(m->genericName)));
        medTable->setItem(row, 3, new QTableWidgetItem(QString::number(m->stock)));
        medTable->setItem(row, 4, new QTableWidgetItem(QString::number(m->consumed)));
        row++;
    }
    medTable->resizeColumnsToContents();
}

void AdminWidget::onAddMedicine() {
    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("新增药品"));
    QFormLayout form(&dlg);

    QLineEdit* tradeEdit = new QLineEdit();
    tradeEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("商品名:"), tradeEdit);

    QLineEdit* genericEdit = new QLineEdit();
    genericEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("通用名:"), genericEdit);

    QLineEdit* aliasEdit = new QLineEdit();
    aliasEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("别名:"), aliasEdit);

    QLineEdit* specEdit = new QLineEdit();
    specEdit->setMaxLength(MAX_ITEM_LEN);
    form.addRow(QString::fromUtf8("规格:"), specEdit);

    QSpinBox* stockSpin = new QSpinBox();
    stockSpin->setRange(0, 99999);
    stockSpin->setValue(0);
    form.addRow(QString::fromUtf8("初始库存:"), stockSpin);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString tradeName = tradeEdit->text().trimmed();
        QString genericName = genericEdit->text().trimmed();
        if (tradeName.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("商品名不能为空!"));
            return;
        }
        if (genericName.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("通用名不能为空!"));
            return;
        }

        MedicineNode* node = new MedicineNode();
        node->medID = generateMedID();
        node->tradeName = tradeName.toStdString();
        node->genericName = genericName.toStdString();
        node->alias = aliasEdit->text().trimmed().toStdString();
        node->spec = specEdit->text().trimmed().toStdString();
        node->stock = stockSpin->value();
        node->consumed = 0;
        insertMedicineHead(medHead, node);
        refreshMedTable();
    }
}

void AdminWidget::onModifyMedicine() {
    int curRow = medTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一种药品!"));
        return;
    }
    QString medID = medTable->item(curRow, 0)->text();
    MedicineNode* med = findMedicineByID(medHead, medID.toStdString());
    if (!med) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("药品不存在!"));
        return;
    }

    QDialog dlg(this);
    dlg.setWindowTitle(QString::fromUtf8("修改药品"));
    QFormLayout form(&dlg);

    QLineEdit* tradeEdit = new QLineEdit(QString::fromStdString(med->tradeName));
    tradeEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("商品名:"), tradeEdit);

    QLineEdit* genericEdit = new QLineEdit(QString::fromStdString(med->genericName));
    genericEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("通用名:"), genericEdit);

    QLineEdit* aliasEdit = new QLineEdit(QString::fromStdString(med->alias));
    aliasEdit->setMaxLength(MAX_NAME_LEN);
    form.addRow(QString::fromUtf8("别名:"), aliasEdit);

    QLineEdit* specEdit = new QLineEdit(QString::fromStdString(med->spec));
    specEdit->setMaxLength(MAX_ITEM_LEN);
    form.addRow(QString::fromUtf8("规格:"), specEdit);

    QSpinBox* stockSpin = new QSpinBox();
    stockSpin->setRange(0, 99999);
    stockSpin->setValue(med->stock);
    form.addRow(QString::fromUtf8("库存:"), stockSpin);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        QString tradeName = tradeEdit->text().trimmed();
        QString genericName = genericEdit->text().trimmed();
        if (tradeName.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("商品名不能为空!"));
            return;
        }
        if (genericName.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("输入错误"), QString::fromUtf8("通用名不能为空!"));
            return;
        }
        med->tradeName = tradeName.toStdString();
        med->genericName = genericName.toStdString();
        med->alias = aliasEdit->text().trimmed().toStdString();
        med->spec = specEdit->text().trimmed().toStdString();
        med->stock = stockSpin->value();
        refreshMedTable();
    }
}

void AdminWidget::onDeleteMedicine() {
    int curRow = medTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一种药品!"));
        return;
    }
    QString medID = medTable->item(curRow, 0)->text();
    MedicineNode* med = findMedicineByID(medHead, medID.toStdString());
    if (!med) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("药品不存在!"));
        return;
    }

    int ret = QMessageBox::question(this, QString::fromUtf8("确认删除"),
        QString::fromUtf8(("确认删除药品 " + med->tradeName + "?").c_str()),
        QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        deleteMedicineByID(medHead, med->medID);
        refreshMedTable();
    }
}

void AdminWidget::onStockIn() {
    int curRow = medTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一种药品!"));
        return;
    }
    QString medID = medTable->item(curRow, 0)->text();
    MedicineNode* med = findMedicineByID(medHead, medID.toStdString());
    if (!med) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("药品不存在!"));
        return;
    }

    bool ok;
    int qty = QInputDialog::getInt(this, QString::fromUtf8("药品入库"),
        QString::fromUtf8(("药品: " + med->tradeName + "  当前库存: " + std::to_string(med->stock) + "\n入库数量:").c_str()),
        1, 1, 99999, 1, &ok);
    if (!ok) return;

    med->stock += qty;
    refreshMedTable();
}

void AdminWidget::onStockOut() {
    int curRow = medTable->currentRow();
    if (curRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("提示"), QString::fromUtf8("请先选择一种药品!"));
        return;
    }
    QString medID = medTable->item(curRow, 0)->text();
    MedicineNode* med = findMedicineByID(medHead, medID.toStdString());
    if (!med) {
        QMessageBox::warning(this, QString::fromUtf8("错误"), QString::fromUtf8("药品不存在!"));
        return;
    }

    bool ok;
    int qty = QInputDialog::getInt(this, QString::fromUtf8("药品出库"),
        QString::fromUtf8(("药品: " + med->tradeName + "  当前库存: " + std::to_string(med->stock) + "\n出库数量:").c_str()),
        1, 1, 99999, 1, &ok);
    if (!ok) return;

    if (qty > med->stock) {
        QMessageBox::warning(this, QString::fromUtf8("库存不足"),
            QString::fromUtf8(("库存不足! 当前库存仅 " + std::to_string(med->stock) + ", 无法出库 " + std::to_string(qty) + ".").c_str()));
        return;
    }

    med->stock -= qty;
    med->consumed += qty;
    refreshMedTable();
}

// ==================== Tab 7: Report Panel ====================
void AdminWidget::setupReportPanel() {
    QWidget* panel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);

    reportTabs = new QTabWidget();
    mainLayout->addWidget(reportTabs);

    // ---------- Subtask 1: Medicine Report ----------
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout(tab);

        QHBoxLayout* filterLay = new QHBoxLayout();
        filterLay->addWidget(new QLabel(QString::fromUtf8("科室筛选:")));
        QComboBox* deptFilter = new QComboBox();
        deptFilter->addItem(QString::fromUtf8("全部"), QVariant(QString()));
        for (DepartmentNode* d = deptHead; d != nullptr; d = d->next) {
            deptFilter->addItem(QString::fromStdString(d->name), QVariant(QString::fromStdString(d->deptID)));
        }
        filterLay->addWidget(deptFilter);
        filterLay->addWidget(new QLabel(QString::fromUtf8("搜索:")));
        QLineEdit* searchBox = new QLineEdit();
        searchBox->setPlaceholderText(QString::fromUtf8("输入药品名..."));
        filterLay->addWidget(searchBox);
        filterLay->addStretch();
        lay->addLayout(filterLay);

        medReportTable = new QTableWidget(0, 5);
        medReportTable->setHorizontalHeaderLabels({
            QString::fromUtf8("药品ID"), QString::fromUtf8("商品名"),
            QString::fromUtf8("通用名"), QString::fromUtf8("库存量"), QString::fromUtf8("消耗量")
        });
        medReportTable->horizontalHeader()->setStretchLastSection(true);
        medReportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        medReportTable->setSortingEnabled(true);
        lay->addWidget(medReportTable);

        QPushButton* refreshBtn = new QPushButton(QString::fromUtf8("刷新"));
        lay->addWidget(refreshBtn);

        connect(refreshBtn, &QPushButton::clicked, [this, deptFilter, searchBox]() {
            refreshReportTabs();
            // Apply filters
            QString filterDept = deptFilter->currentData().toString();
            QString search = searchBox->text().trimmed();
            medReportTable->setRowCount(0);
            int row = 0;
            for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
                if (!filterDept.isEmpty()
                    && !findDeptMedicine(deptMedHead, filterDept.toStdString(), m->medID)) continue;
                if (!search.isEmpty()) {
                    QString trade = QString::fromStdString(m->tradeName);
                    QString generic = QString::fromStdString(m->genericName);
                    if (!trade.contains(search, Qt::CaseInsensitive)
                        && !generic.contains(search, Qt::CaseInsensitive)) continue;
                }
                medReportTable->insertRow(row);
                medReportTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m->medID)));
                medReportTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(m->tradeName)));
                medReportTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(m->genericName)));
                medReportTable->setItem(row, 3, new QTableWidgetItem(QString::number(m->stock)));
                medReportTable->setItem(row, 4, new QTableWidgetItem(QString::number(m->consumed)));
                row++;
            }
            medReportTable->resizeColumnsToContents();
        });

        reportTabs->addTab(tab, QString::fromUtf8("药品报表"));
    }

    // ---------- Subtask 2: Doctor Report ----------
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout(tab);

        QHBoxLayout* timeLay = new QHBoxLayout();
        timeLay->addWidget(new QLabel(QString::fromUtf8("起始日期:")));
        QDateEdit* startDate = new QDateEdit(QDate::currentDate());
        startDate->setCalendarPopup(true);
        timeLay->addWidget(startDate);
        timeLay->addWidget(new QLabel(QString::fromUtf8("结束日期:")));
        QDateEdit* endDate = new QDateEdit(QDate::currentDate());
        endDate->setCalendarPopup(true);
        timeLay->addWidget(endDate);
        QPushButton* queryBtn = new QPushButton(QString::fromUtf8("查询"));
        timeLay->addWidget(queryBtn);
        timeLay->addStretch();
        lay->addLayout(timeLay);

        doctorReportTable = new QTableWidget(0, 5);
        doctorReportTable->setHorizontalHeaderLabels({
            QString::fromUtf8("医生ID"), QString::fromUtf8("姓名"),
            QString::fromUtf8("级别"), QString::fromUtf8("接诊数"), QString::fromUtf8("处方数")
        });
        doctorReportTable->horizontalHeader()->setStretchLastSection(true);
        doctorReportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        doctorReportTable->setSortingEnabled(true);
        lay->addWidget(doctorReportTable);

        connect(queryBtn, &QPushButton::clicked, [this, startDate, endDate]() {
            doctorReportTable->setRowCount(0);
            QDate refDate(2024, 1, 1);
            unsigned int sDay = (unsigned int)refDate.daysTo(startDate->date()) + 1;
            unsigned int eDay = (unsigned int)refDate.daysTo(endDate->date()) + 1;
            if (sDay < 1) sDay = 1;
            if (eDay < sDay) eDay = sDay;

            int row = 0;
            for (DoctorNode* doc = doctorHead; doc != nullptr; doc = doc->next) {
                int consultCount = 0;
                for (ConsultationNode* c = consultHead; c != nullptr; c = c->next) {
                    if (c->doctorID == doc->doctorID && c->status == STATUS_VALID
                        && c->consultDay >= sDay && c->consultDay <= eDay) consultCount++;
                }
                int prescCount = 0;
                for (PrescriptionNode* pn = prescHead; pn != nullptr; pn = pn->next) {
                    if (pn->doctorID == doc->doctorID && pn->status != 1
                        && pn->prescDay >= sDay && pn->prescDay <= eDay) prescCount++;
                }
                doctorReportTable->insertRow(row);
                doctorReportTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(doc->doctorID)));
                doctorReportTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(doc->name)));
                doctorReportTable->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(getLevelName(doc->level))));
                doctorReportTable->setItem(row, 3, new QTableWidgetItem(QString::number(consultCount)));
                doctorReportTable->setItem(row, 4, new QTableWidgetItem(QString::number(prescCount)));
                row++;
            }
            doctorReportTable->resizeColumnsToContents();
        });

        reportTabs->addTab(tab, QString::fromUtf8("医生报表"));
    }

    // ---------- Subtask 3: Bed Report ----------
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout(tab);

        QTableWidget* bedRepTable = new QTableWidget();
        bedRepTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        lay->addWidget(bedRepTable);

        QPushButton* refreshBtn = new QPushButton(QString::fromUtf8("刷新床位报表"));
        lay->addWidget(refreshBtn);

        connect(refreshBtn, &QPushButton::clicked, [this, bedRepTable]() {
            int totalWards = 0, maxBeds = 0;
            for (WardNode* w = wardHead; w != nullptr; w = w->next) {
                totalWards++;
                if (w->bedCount > maxBeds) maxBeds = w->bedCount;
            }
            if (maxBeds == 0) maxBeds = 1;

            bedRepTable->setRowCount(totalWards);
            bedRepTable->setColumnCount(3 + maxBeds);
            QStringList headers;
            headers << QString::fromUtf8("病房ID") << QString::fromUtf8("病房类型") << QString::fromUtf8("所属科室");
            for (int i = 0; i < maxBeds; i++) {
                headers << QString::fromUtf8((std::to_string(i + 1) + "号床").c_str());
            }
            bedRepTable->setHorizontalHeaderLabels(headers);

            int row = 0;
            for (WardNode* w = wardHead; w != nullptr; w = w->next) {
                bedRepTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(w->wardID)));
                bedRepTable->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(getWardTypeName(w->type))));
                DepartmentNode* dept = findDepartmentByID(deptHead, w->departmentID);
                bedRepTable->setItem(row, 2, new QTableWidgetItem(
                    dept ? QString::fromStdString(dept->name) : QString::fromUtf8("无")));

                for (int i = 0; i < maxBeds; i++) {
                    QTableWidgetItem* item = new QTableWidgetItem();
                    if (i < w->bedCount) {
                        if (w->bedStatus[i].empty()) {
                            item->setText(QString::fromUtf8("空闲"));
                            item->setBackground(QColor(144, 238, 144));
                        } else {
                            item->setText(QString::fromUtf8("占用"));
                            item->setBackground(QColor(255, 140, 140));
                        }
                    } else {
                        item->setText("-");
                        item->setBackground(QColor(200, 200, 200));
                        item->setFlags(Qt::NoItemFlags);
                    }
                    bedRepTable->setItem(row, 3 + i, item);
                }
                row++;
            }
            bedRepTable->resizeColumnsToContents();
        });

        reportTabs->addTab(tab, QString::fromUtf8("床位报表"));
    }

    // ---------- Subtask 4: Revenue Report ----------
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout(tab);

        QHBoxLayout* timeLay = new QHBoxLayout();
        timeLay->addWidget(new QLabel(QString::fromUtf8("起始日期:")));
        QDateEdit* startDate = new QDateEdit(QDate::currentDate());
        startDate->setCalendarPopup(true);
        timeLay->addWidget(startDate);
        timeLay->addWidget(new QLabel(QString::fromUtf8("结束日期:")));
        QDateEdit* endDate = new QDateEdit(QDate::currentDate());
        endDate->setCalendarPopup(true);
        timeLay->addWidget(endDate);
        QPushButton* queryBtn = new QPushButton(QString::fromUtf8("查询"));
        timeLay->addWidget(queryBtn);
        timeLay->addStretch();
        lay->addLayout(timeLay);

        revenueReportTable = new QTableWidget(0, 4);
        revenueReportTable->setHorizontalHeaderLabels({
            QString::fromUtf8("收入类别"), QString::fromUtf8("数量/明细"),
            QString::fromUtf8("金额(元)"), QString::fromUtf8("占比")
        });
        revenueReportTable->horizontalHeader()->setStretchLastSection(true);
        revenueReportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        lay->addWidget(revenueReportTable);

        connect(queryBtn, &QPushButton::clicked, [this, startDate, endDate]() {
            revenueReportTable->setRowCount(0);
            QDate refDate(2024, 1, 1);
            unsigned int sDay = (unsigned int)refDate.daysTo(startDate->date()) + 1;
            unsigned int eDay = (unsigned int)refDate.daysTo(endDate->date()) + 1;
            if (sDay < 1) sDay = 1;
            if (eDay < sDay) eDay = sDay;

            // Registration income
            int regCount = 0;
            for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
                if (r->regDay >= sDay && r->regDay <= eDay) regCount++;
            }
            double regIncome = regCount * REGISTRATION_FEE;

            // Prescription income
            double prescIncome = 0.0;
            int paidPrescCount = 0;
            for (PrescriptionNode* pn = prescHead; pn != nullptr; pn = pn->next) {
                if (pn->status == 2 && pn->prescDay >= sDay && pn->prescDay <= eDay) {
                    prescIncome += pn->totalAmount;
                    paidPrescCount++;
                }
            }

            // Hospitalization income
            double hospIncome = 0.0;
            int hospCount = 0;
            for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
                if (h->status == 1 && h->dischargeDay >= sDay && h->dischargeDay <= eDay) {
                    int stayDays = h->dischargeDay - h->admitDay;
                    if (stayDays < 1) stayDays = 1;
                    hospIncome += HOSPITAL_DAILY_FEE * stayDays;
                    hospCount++;
                }
            }

            double totalIncome = regIncome + prescIncome + hospIncome;
            if (totalIncome <= 0) totalIncome = 1.0;

            auto addRow = [&](const QString& cat, const QString& detail, double amount) {
                int r = revenueReportTable->rowCount();
                revenueReportTable->insertRow(r);
                revenueReportTable->setItem(r, 0, new QTableWidgetItem(cat));
                revenueReportTable->setItem(r, 1, new QTableWidgetItem(detail));
                revenueReportTable->setItem(r, 2, new QTableWidgetItem(QString::number(amount, 'f', 2)));
                revenueReportTable->setItem(r, 3, new QTableWidgetItem(
                    QString::number(amount / totalIncome * 100.0, 'f', 1) + "%"));
            };

            addRow(QString::fromUtf8("挂号费收入"),
                   QString::fromUtf8(("共 " + std::to_string(regCount) + " 人次").c_str()), regIncome);
            addRow(QString::fromUtf8("药费收入"),
                   QString::fromUtf8(("共 " + std::to_string(paidPrescCount) + " 张处方").c_str()), prescIncome);
            addRow(QString::fromUtf8("住院费收入"),
                   QString::fromUtf8(("共 " + std::to_string(hospCount) + " 人次").c_str()), hospIncome);

            int r = revenueReportTable->rowCount();
            revenueReportTable->insertRow(r);
            QTableWidgetItem* totalItem = new QTableWidgetItem(QString::fromUtf8("总计"));
            totalItem->setBackground(QColor(220, 220, 255));
            revenueReportTable->setItem(r, 0, totalItem);
            revenueReportTable->setItem(r, 1, new QTableWidgetItem("-"));
            QTableWidgetItem* totalVal = new QTableWidgetItem(QString::number(totalIncome, 'f', 2));
            totalVal->setBackground(QColor(220, 220, 255));
            revenueReportTable->setItem(r, 2, totalVal);
            revenueReportTable->setItem(r, 3, new QTableWidgetItem("100.0%"));

            revenueReportTable->resizeColumnsToContents();
        });

        reportTabs->addTab(tab, QString::fromUtf8("营业额报表"));
    }

    // ---------- Subtask 5: Doctor Workload ----------
    {
        QWidget* tab = new QWidget();
        QVBoxLayout* lay = new QVBoxLayout(tab);

        QTableWidget* workTable = new QTableWidget(0, 5);
        workTable->setHorizontalHeaderLabels({
            QString::fromUtf8("医生ID"), QString::fromUtf8("姓名"),
            QString::fromUtf8("接诊数"), QString::fromUtf8("处方数"), QString::fromUtf8("负责住院患者数")
        });
        workTable->horizontalHeader()->setStretchLastSection(true);
        workTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        workTable->setSortingEnabled(true);
        lay->addWidget(workTable);

        QPushButton* refreshBtn = new QPushButton(QString::fromUtf8("刷新医生繁忙度"));
        lay->addWidget(refreshBtn);

        connect(refreshBtn, &QPushButton::clicked, [this, workTable]() {
            workTable->setRowCount(0);
            int row = 0;
            for (DoctorNode* doc = doctorHead; doc != nullptr; doc = doc->next) {
                int consultCount = 0;
                for (ConsultationNode* c = consultHead; c != nullptr; c = c->next) {
                    if (c->doctorID == doc->doctorID && c->status == STATUS_VALID) consultCount++;
                }
                int prescCount = 0;
                for (PrescriptionNode* pn = prescHead; pn != nullptr; pn = pn->next) {
                    if (pn->doctorID == doc->doctorID && pn->status != 1) prescCount++;
                }
                int hospPatCount = 0;
                for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
                    if (h->status != 0) continue;
                    for (int i = 0; i < h->doctorCount; i++) {
                        if (h->doctorIDs[i] == doc->doctorID) { hospPatCount++; break; }
                    }
                }
                workTable->insertRow(row);
                workTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(doc->doctorID)));
                workTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(doc->name)));
                workTable->setItem(row, 2, new QTableWidgetItem(QString::number(consultCount)));
                workTable->setItem(row, 3, new QTableWidgetItem(QString::number(prescCount)));
                workTable->setItem(row, 4, new QTableWidgetItem(QString::number(hospPatCount)));
                row++;
            }
            workTable->resizeColumnsToContents();
        });

        reportTabs->addTab(tab, QString::fromUtf8("医生繁忙度"));
    }

    tabWidget->addTab(panel, QString::fromUtf8("报表"));
}

void AdminWidget::refreshReportTabs() {
    medReportTable->setRowCount(0);
    int row = 0;
    for (MedicineNode* m = medHead; m != nullptr; m = m->next) {
        medReportTable->insertRow(row);
        medReportTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m->medID)));
        medReportTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(m->tradeName)));
        medReportTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(m->genericName)));
        medReportTable->setItem(row, 3, new QTableWidgetItem(QString::number(m->stock)));
        medReportTable->setItem(row, 4, new QTableWidgetItem(QString::number(m->consumed)));
        row++;
    }
    medReportTable->resizeColumnsToContents();
}

// ==================== Time Advance ====================
void AdminWidget::onTimeAdvance() {
    bool ok;
    int days = QInputDialog::getInt(this, QString::fromUtf8("时间推进"),
        QString::fromUtf8("请输入推进天数 (>0):"), 1, 1, 9999, 1, &ok);
    if (!ok || days <= 0) return;

    extern unsigned long long globalTime;
    extern double money;
    double totalDeducted = 0.0;
    int expiredCount = 0;

    for (int d = 0; d < days; d++) {
        setTime(globalTime + 86400);

        // Auto-deduct daily fee from inpatients
        for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
            if (h->status != 0) continue;
            h->deposit -= HOSPITAL_DAILY_FEE;
            money += HOSPITAL_DAILY_FEE;
            totalDeducted += HOSPITAL_DAILY_FEE;

            PatientNode* p = findPatientByID(patientHead, h->patientID);
            if (p) {
                p->deposit = h->deposit;
                p->admitDays++;
            }
        }

        // Expire pending registrations from past days
        for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
            if (r->status == STATUS_PENDING && r->regDay < weekday) {
                r->status = STATUS_EXPIRED;
                expiredCount++;
            }
        }
    }

    QString msg = QString::fromUtf8(
        ("时间推进 " + std::to_string(days) + " 天完成!\n"
         + "当前日期: Day" + std::to_string(weekday) + " ("
         + WEEKDAY_NAMES[((weekday - 1) % 7)] + ")\n"
         + "扣除住院费用总计: " + std::to_string(totalDeducted) + " 元\n"
         + "过期挂号数: " + std::to_string(expiredCount)).c_str());

    int negativeCount = 0;
    QString negativeList;
    for (PatientNode* p = patientHead; p != nullptr; p = p->next) {
        if (p->type == PATIENT_INPATIENT && p->deposit < 0) {
            if (negativeCount == 0)
                negativeList += QString::fromUtf8("\n\n以下患者押金不足:\n");
            negativeList += QString::fromUtf8(
                ("  " + p->patientID + " " + p->name + " 余额: " + std::to_string(p->deposit) + " 元\n").c_str());
            negativeCount++;
        }
    }
    msg += negativeList;

    QMessageBox::information(this, QString::fromUtf8("时间推进完成"), msg);
    refreshAll();
}
