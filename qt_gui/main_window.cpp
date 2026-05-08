#include "main_window.h"
#include "admin_panels.h"
#include "doctor_panels.h"
#include "patient_panels.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QCloseEvent>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFont>
#include <QGroupBox>
#include <QSpacerItem>
#include <QKeySequence>

#include "../global.h"
#include "../list_ops.h"
#include "../persistence.h"

// External linked list heads (defined in main.cpp)
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

// ==================== Admin Login Dialog ====================
class AdminLoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdminLoginDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("管理员登录");
        setFixedSize(380, 220);
        setModal(true);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(15);

        QLabel* titleLabel = new QLabel("管理员登录");
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        QFormLayout* formLayout = new QFormLayout();
        formLayout->setSpacing(10);

        adminIDEdit = new QLineEdit();
        adminIDEdit->setPlaceholderText("请输入管理员ID");
        adminIDEdit->setMinimumHeight(30);
        formLayout->addRow("管理员ID:", adminIDEdit);

        passwordEdit = new QLineEdit();
        passwordEdit->setPlaceholderText("请输入密码");
        passwordEdit->setEchoMode(QLineEdit::Password);
        passwordEdit->setMinimumHeight(30);
        formLayout->addRow("密码:", passwordEdit);

        mainLayout->addLayout(formLayout);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();

        QPushButton* cancelBtn = new QPushButton("取消");
        cancelBtn->setMinimumWidth(80);
        cancelBtn->setMinimumHeight(32);
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
        buttonLayout->addWidget(cancelBtn);

        QPushButton* okBtn = new QPushButton("登录");
        okBtn->setMinimumWidth(80);
        okBtn->setMinimumHeight(32);
        okBtn->setDefault(true);
        connect(okBtn, &QPushButton::clicked, this, &AdminLoginDialog::onLogin);
        buttonLayout->addWidget(okBtn);

        mainLayout->addLayout(buttonLayout);
    }

    std::string getAdminID() const { return adminID.toStdString(); }

private slots:
    void onLogin() {
        adminID = adminIDEdit->text().trimmed();
        QString pass = passwordEdit->text().trimmed();

        if (adminID.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入管理员ID！");
            adminIDEdit->setFocus();
            return;
        }
        if (pass.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入密码！");
            passwordEdit->setFocus();
            return;
        }

        AdminNode* admin = findAdminByID(adminHead, adminID.toStdString());
        if (!admin) {
            QMessageBox::warning(this, "登录失败", "管理员ID不存在！");
            adminIDEdit->setFocus();
            return;
        }
        if (admin->password != pass.toStdString()) {
            QMessageBox::warning(this, "登录失败", "密码错误！");
            passwordEdit->setFocus();
            return;
        }

        accept();
    }

private:
    QLineEdit* adminIDEdit;
    QLineEdit* passwordEdit;
    QString adminID;
};

// ==================== About Dialog ====================
class AboutDialog : public QDialog {
    Q_OBJECT
public:
    explicit AboutDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("关于");
        setFixedSize(420, 280);
        setModal(true);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setSpacing(12);

        QLabel* title = new QLabel("小型医院医疗管理系统");
        QFont titleFont = title->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        title->setFont(titleFont);
        title->setAlignment(Qt::AlignCenter);
        layout->addWidget(title);

        QLabel* version = new QLabel("版本 1.0");
        version->setAlignment(Qt::AlignCenter);
        layout->addWidget(version);

        QLabel* desc = new QLabel(
            "本系统用于模拟小型医院的日常运营管理，\n"
            "包括挂号、诊疗、检查、处方、住院、药品管理等功能。\n\n"
            "基于Qt5框架开发，使用链表数据结构管理数据。\n"
            "支持数据持久化保存与加载。"
        );
        desc->setAlignment(Qt::AlignCenter);
        desc->setWordWrap(true);
        layout->addWidget(desc);

        layout->addStretch();

        QPushButton* closeBtn = new QPushButton("确定");
        closeBtn->setMinimumWidth(80);
        closeBtn->setMinimumHeight(32);
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->addStretch();
        btnLayout->addWidget(closeBtn);
        btnLayout->addStretch();
        layout->addLayout(btnLayout);
    }
};

// ==================== MainWindow Implementation ====================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      stackedWidget(nullptr),
      loginPage(nullptr),
      adminPage(nullptr),
      doctorPage(nullptr),
      patientPage(nullptr),
      statusLabel(nullptr)
{
    setupUI();
    setupMenuBar();
    updateStatusBar();
}

void MainWindow::setupUI() {
    setWindowTitle("小型医院医疗管理系统");
    resize(1280, 800);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // ---- Login Page ----
    loginPage = new QWidget();
    QVBoxLayout* loginLayout = new QVBoxLayout(loginPage);
    loginLayout->setAlignment(Qt::AlignCenter);
    loginLayout->setSpacing(20);

    // Title
    QLabel* titleLabel = new QLabel("小型医院医疗管理系统");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    loginLayout->addWidget(titleLabel);

    // Subtitle
    QLabel* subtitleLabel = new QLabel("Hospital Management System");
    QFont subFont = subtitleLabel->font();
    subFont.setPointSize(12);
    subtitleLabel->setFont(subFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("color: #888;");
    loginLayout->addWidget(subtitleLabel);

    loginLayout->addSpacing(30);

    // Status info group
    QGroupBox* statusGroup = new QGroupBox("系统状态");
    statusGroup->setFixedWidth(500);
    QVBoxLayout* statusGroupLayout = new QVBoxLayout(statusGroup);
    statusGroupLayout->setSpacing(8);

    QLabel* dayLabel = new QLabel();
    dayLabel->setObjectName("dayLabel");
    QFont statusFont = dayLabel->font();
    statusFont.setPointSize(14);
    dayLabel->setFont(statusFont);
    dayLabel->setAlignment(Qt::AlignCenter);
    statusGroupLayout->addWidget(dayLabel);

    QLabel* moneyLabel = new QLabel();
    moneyLabel->setObjectName("moneyLabel");
    moneyLabel->setFont(statusFont);
    moneyLabel->setAlignment(Qt::AlignCenter);
    statusGroupLayout->addWidget(moneyLabel);

    QHBoxLayout* statusCenterLayout = new QHBoxLayout();
    statusCenterLayout->addStretch();
    statusCenterLayout->addWidget(statusGroup);
    statusCenterLayout->addStretch();
    loginLayout->addLayout(statusCenterLayout);

    loginLayout->addSpacing(30);

    // Login buttons in a horizontal layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(30);
    buttonLayout->addStretch();

    // Admin login button
    QPushButton* adminBtn = new QPushButton("管理员登录");
    adminBtn->setMinimumSize(200, 80);
    QFont btnFont = adminBtn->font();
    btnFont.setPointSize(15);
    btnFont.setBold(true);
    adminBtn->setFont(btnFont);
    adminBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e74c3c;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #a93226;"
        "}"
    );
    connect(adminBtn, &QPushButton::clicked, this, &MainWindow::loginAsAdmin);
    buttonLayout->addWidget(adminBtn);

    // Doctor login button
    QPushButton* doctorBtn = new QPushButton("医生登录");
    doctorBtn->setMinimumSize(200, 80);
    doctorBtn->setFont(btnFont);
    doctorBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #2ecc71;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #27ae60;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #1e8449;"
        "}"
    );
    connect(doctorBtn, &QPushButton::clicked, this, &MainWindow::loginAsDoctor);
    buttonLayout->addWidget(doctorBtn);

    // Patient login button
    QPushButton* patientBtn = new QPushButton("患者登录");
    patientBtn->setMinimumSize(200, 80);
    patientBtn->setFont(btnFont);
    patientBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3498db;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #1a5276;"
        "}"
    );
    connect(patientBtn, &QPushButton::clicked, this, &MainWindow::loginAsPatient);
    buttonLayout->addWidget(patientBtn);

    buttonLayout->addStretch();
    loginLayout->addLayout(buttonLayout);

    loginLayout->addSpacing(30);

    // Footer hint
    QLabel* hintLabel = new QLabel("请选择角色进行登录，或使用菜单栏加载已有数据");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("color: #aaa;");
    loginLayout->addWidget(hintLabel);

    stackedWidget->addWidget(loginPage);

    // ---- Admin Page (placeholder) ----
    adminPage = new AdminWidget();
    stackedWidget->addWidget(adminPage);

    // ---- Doctor Page (placeholder) ----
    doctorPage = new DoctorWidget();
    stackedWidget->addWidget(doctorPage);

    // ---- Patient Page (placeholder) ----
    patientPage = new PatientWidget();
    stackedWidget->addWidget(patientPage);

    // Start on login page
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = this->menuBar();

    // ---- File Menu ----
    QMenu* fileMenu = menuBar->addMenu("文件(&F)");

    QAction* saveAction = fileMenu->addAction("保存到文件(&S)");
    saveAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveData);

    QAction* loadAction = fileMenu->addAction("从文件加载(&L)");
    loadAction->setShortcut(QKeySequence("Ctrl+L"));
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadData);

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    // ---- Help Menu ----
    QMenu* helpMenu = menuBar->addMenu("帮助(&H)");

    QAction* aboutAction = helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    // ---- Status Bar ----
    statusLabel = new QLabel();
    statusBar()->addPermanentWidget(statusLabel);
}

void MainWindow::updateStatusBar() {
    QString timeText = QString::fromStdString(formatTime());
    char moneyBuf[32];
    snprintf(moneyBuf, sizeof(moneyBuf), "%.2f", money);
    statusLabel->setText(
        QString("当前时间: %1  |  资金: ¥%2 元")
            .arg(timeText)
            .arg(moneyBuf)
    );

    // Also update the status group on login page if visible
    if (loginPage) {
        QLabel* dayLabel = loginPage->findChild<QLabel*>("dayLabel");
        if (dayLabel) {
            dayLabel->setText(QString("当前时间: %1").arg(timeText));
        }
        QLabel* moneyLabel = loginPage->findChild<QLabel*>("moneyLabel");
        if (moneyLabel) {
            moneyLabel->setText(QString("医院资金: ¥%1 元").arg(moneyBuf));
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    // Save data before exiting
    bool saved = saveAllData(deptHead, doctorHead, patientHead,
                             regHead, consultHead, examHead,
                             prescHead, medHead, prescMedHead,
                             deptMedHead, wardHead, hospHead, adminHead);

    if (saved) {
        QMessageBox::information(this, "提示", "数据已保存，感谢使用！");
    } else {
        QMessageBox::warning(this, "警告", "数据保存失败！系统仍将退出。");
    }

    event->accept();
}

// ==================== Login Slots ====================

void MainWindow::loginAsAdmin() {
    AdminLoginDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Successful login
        QMessageBox::information(this, "登录成功", "管理员登录成功！");
        showAdminPanel();
        stackedWidget->setCurrentWidget(adminPage);
        updateStatusBar();
    }
}

void MainWindow::loginAsDoctor() {
    bool ok = false;
    QString doctorID = QInputDialog::getText(
        this,
        "医生登录",
        "请输入医生ID:",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok || doctorID.trimmed().isEmpty()) {
        return;  // User cancelled or entered empty
    }

    std::string id = doctorID.trimmed().toStdString();
    DoctorNode* doctor = findDoctorByID(doctorHead, id);

    if (!doctor) {
        QMessageBox::warning(this, "登录失败",
            QString("医生ID '%1' 不存在！").arg(doctorID.trimmed()));
        return;
    }

    QMessageBox::information(this, "登录成功",
        QString("欢迎，%1 医生！").arg(QString::fromStdString(doctor->name)));

    showDoctorPanel(id);
    stackedWidget->setCurrentWidget(doctorPage);
    updateStatusBar();
}

void MainWindow::loginAsPatient() {
    bool ok = false;
    QString patientID = QInputDialog::getText(
        this,
        "患者登录",
        "请输入患者ID:",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok || patientID.trimmed().isEmpty()) {
        return;  // User cancelled or entered empty
    }

    std::string id = patientID.trimmed().toStdString();
    PatientNode* patient = findPatientByID(patientHead, id);

    if (!patient) {
        QMessageBox::warning(this, "登录失败",
            QString("患者ID '%1' 不存在！").arg(patientID.trimmed()));
        return;
    }

    QMessageBox::information(this, "登录成功",
        QString("欢迎，%1 ！").arg(QString::fromStdString(patient->name)));

    showPatientPanel(id);
    stackedWidget->setCurrentWidget(patientPage);
    updateStatusBar();
}

// ==================== Panel Navigation ====================

void MainWindow::showLogin() {
    stackedWidget->setCurrentWidget(loginPage);
    updateStatusBar();
}

void MainWindow::showAdminPanel() {
    // The admin panel will be shown but no special init needed here
    // AdminWidget will handle its own initialization
}

void MainWindow::showDoctorPanel(const std::string& doctorID) {
    // Tell the doctor page which doctor is logged in
    DoctorNode* doc = findDoctorByID(doctorHead, doctorID);
    if (doc) {
        // For the initial stub, we just switch pages
        // Future: call doctorPage->setDoctor(doc) with full implementation
    }
}

void MainWindow::showPatientPanel(const std::string& patientID) {
    // Tell the patient page which patient is logged in
    PatientNode* pat = findPatientByID(patientHead, patientID);
    if (pat) {
        // For the initial stub, we just switch pages
        // Future: call patientPage->setPatient(pat) with full implementation
    }
}

// ==================== File Operations ====================

void MainWindow::saveData() {
    bool saved = saveAllData(deptHead, doctorHead, patientHead,
                             regHead, consultHead, examHead,
                             prescHead, medHead, prescMedHead,
                             deptMedHead, wardHead, hospHead, adminHead);

    if (saved) {
        QMessageBox::information(this, "保存成功", "所有数据已保存到文件。");
    } else {
        QMessageBox::warning(this, "保存失败", "数据保存到文件时发生错误，请检查文件权限。");
    }

    updateStatusBar();
}

void MainWindow::loadData() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认加载",
        "加载数据将覆盖当前所有数据，是否继续？",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply != QMessageBox::Yes) {
        return;
    }

    // Free current data first
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

    // Reset head pointers
    deptHead = nullptr;
    doctorHead = nullptr;
    patientHead = nullptr;
    regHead = nullptr;
    consultHead = nullptr;
    examHead = nullptr;
    prescHead = nullptr;
    medHead = nullptr;
    prescMedHead = nullptr;
    deptMedHead = nullptr;
    wardHead = nullptr;
    hospHead = nullptr;
    adminHead = nullptr;

    bool loaded = loadAllData(deptHead, doctorHead, patientHead,
                              regHead, consultHead, examHead,
                              prescHead, medHead, prescMedHead,
                              deptMedHead, wardHead, hospHead, adminHead);

    if (loaded) {
        QMessageBox::information(this, "加载成功", "数据已从文件加载。");
    } else {
        // Load failed, re-init defaults
        QMessageBox::warning(this, "加载失败",
            "无法加载数据文件，将使用默认初始数据。");
        setTime(0);
        money = 10000.00;
        initDefaultData(deptHead, doctorHead, patientHead,
                       regHead, consultHead, examHead,
                       prescHead, medHead, prescMedHead,
                       deptMedHead, wardHead, hospHead, adminHead);
    }

    updateStatusBar();
}

void MainWindow::showAbout() {
    AboutDialog dialog(this);
    dialog.exec();
}
