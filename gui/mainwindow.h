#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <environment.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void startEnvironment();
    void updateListView();
    void displaySettings(std::map<std::string, std::string> settings);
    void setEnvironment(Environment* env);
    std::vector<std::vector<float>> readAllGenomesFrom(QString path);
    std::vector<float> readBestGenomeFrom(QString path);
public slots:
    void resetEnvironment();
    void readAndSetSettingsfromUi();
    void newSeed();
    void stepsChanged(unsigned long int s);
    void avgFitnessChanged(float f);

private slots:
    void on_playPushButton_clicked();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_pb_LogData_released();

    void on_pushButtonPauseResume_clicked();
    void on_pushButtonResetRun_clicked();
    void on_pushButtonStop_clicked();
    void on_ReplayExperimentButton_clicked();
    void on_ReplayGenomeButton_clicked();

    void on_pushButtonLoadExperiment_clicked();

    void on_RunExperimentButton_clicked();

    void on_pushButtonGenerateRandomANNs_clicked();

private:
    Ui::MainWindow *ui;
    std::map<int, QString> intList;
    QTimer* timer;
    bool start_pushed = false;
    QFileSystemModel* results;

    void startSimulation(int mode);
    void toggle_simulation();


    //states of the simulation
    bool running = false;

};

#endif // MAINWINDOW_H
