#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include "../entities.h"

class AdminWidget;
class DoctorWidget;
class PatientWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void showLogin();
    void loginAsAdmin();
    void loginAsDoctor();
    void loginAsPatient();
    void showAdminPanel();
    void showDoctorPanel(const std::string& doctorID);
    void showPatientPanel(const std::string& patientID);
    void saveData();
    void loadData();
    void showAbout();

private:
    void setupUI();
    void setupMenuBar();
    void updateStatusBar();

    QStackedWidget* stackedWidget;
    QWidget* loginPage;
    AdminWidget* adminPage;
    DoctorWidget* doctorPage;
    PatientWidget* patientPage;
    QLabel* statusLabel;
};

#endif // MAIN_WINDOW_H
