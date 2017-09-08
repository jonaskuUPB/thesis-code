#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include "Utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //QLocale::setDefault(QLocale::c());
    //setlocale(LC_NUMERIC,"C");
    ui->setupUi(this);

    timer = new QTimer(this);

    //set signals for buttons and timeouts
    connect(timer, SIGNAL(timeout()), ui->environmentWidget, SLOT(doStep()));
    connect(ui->pushButton_SetupExperiment, SIGNAL(clicked()), this, SLOT(readAndSetSettingsfromUi()));
    connect(ui->environmentWidget, SIGNAL(initEnvWithSettingsFromUi()), this, SLOT(readAndSetSettingsfromUi()));
    connect(ui->pushButton_NewSeed, SIGNAL(clicked()), this, SLOT(newSeed()));


    results = new QFileSystemModel();
    results->setRootPath(QDir::currentPath().append("/results"));
    ui->treeView->setModel(results);
    ui->treeView->setRootIndex(results->index(QDir::currentPath().append("/results")));
    ui->treeView->hideColumn(1);
    ui->treeView->hideColumn(2);
    ui->treeView->hideColumn(3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggle_simulation() {
    start_pushed = !start_pushed;
    if(start_pushed){
        timer->start();
    }
    else{
        timer->stop();
    }
}

void MainWindow::setEnvironment(Environment* env){
    ui->environmentWidget->setEnvironment(env);
    displaySettings(ui->environmentWidget->getEnvironment()->getSettings());
    connect(env, SIGNAL(stepFinished(unsigned long int)), this, SLOT(stepsChanged(unsigned long int)));
}

void MainWindow::newSeed(){
    int s = Utils::newSeed();
    ui->lineEdit_Seed->setText(QString::number(s));
    readAndSetSettingsfromUi();
}

void MainWindow::resetEnvironment(){
    ui->environmentWidget->resetEnvironment();
    ui->environmentWidget->update();
}

void MainWindow::stepsChanged(unsigned long int s){
    ui->label_StepDisplay->setText(QString("Steps: ").append(QString::number(s)));
    //TODO: Other two stats
    ui->label_GenomeCount->setText(QString("Generation: %1, Genome: %2").arg(ui->environmentWidget->getEnvironment()->getGenerationCount()).arg(ui->environmentWidget->getEnvironment()->getGenomeCount()));
    ui->label_ActionValueDisplay->setText(QString("Action Value: ").append(QString::number(ui->environmentWidget->getEnvironment()->getLastActionValue())));
    ui->label_KDistanceDisplay->setText(QString("K Distance: ").append(QString::number(ui->environmentWidget->getEnvironment()->getLastKDistance())));
    //ui->label_MaxClusterDisplay->setText(QString("Fitness: ").append(QString::number(ui->environmentWidget->getEnvironment()->getLastFitness())));
    ui->label_MaxClusterDisplay->setText(QString("Speed: ").append(QString::number(ui->environmentWidget->getEnvironment()->getLastSpeed())));
}

void MainWindow::avgFitnessChanged(float f){
//    ui->label_avgFitness->setText(QString("avg. fitness: ").append(QString::number(f)));
}


void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QFileInfo info(results->filePath(index));
    qDebug() << "Clicked on " << info.fileName();
    if(info.isFile() && info.path().endsWith("genomes")){
        qDebug() << "Reading genome "<< ui->sB_GenomeNumber->value() <<" from " <<info.fileName() << ".";
        //TODO: Load experiment settings from here as well?!
        std::string fitness = ui->environmentWidget->readAndSetGenomeFrom(ui->sB_GenomeNumber->value(), results->filePath(index));
        ui->label_Fitness->setText("Fitness: " + QString::fromStdString(fitness));
    }
    if(info.isFile() && info.fileName().startsWith("settings")){
        qDebug() << "Experiment set up with settings from " << info.fileName() << ".";
        std::map<std::string, std::string> settings = ui->environmentWidget->readSettingsFrom(results->filePath(index));
        displaySettings(settings);
        readAndSetSettingsfromUi();
    }
    if(info.isFile() && info.fileName().endsWith("pop.out")){
        qDebug() << "Reading genome "<< ui->sB_GenomeNumber->value() <<" from " <<info.fileName() << ".";
        std::string fitness = ui->environmentWidget->readAndSetMOGenomeFrom(ui->sB_GenomeNumber->value(), results->filePath(index));
        ui->label_Fitness->setText("Fitness: " + QString::fromStdString(fitness));
    }
}

void MainWindow::displaySettings(std::map<std::string, std::string> settings){
   for(auto const& s : settings){
       if(s.first == "elitism_int")
           ui->sb_elitism->setValue(std::stoi(s.second));
       if(s.first == "mutations_rate_float")
           ui->sb_mutation_rate->setValue(std::stof(s.second));
       if(s.first == "n_agents_int")
           ui->sb_n_agents->setValue(std::stoi(s.second));
       if(s.first == "n_generations_int")
           ui->sb_n_generations->setValue(std::stoi(s.second));
       if(s.first == "n_genomes_int")
           ui->sb_n_genomes->setValue(std::stoi(s.second));
       if(s.first == "n_steps_per_genome_int")
           ui->sb_n_steps_per_genome->setValue(std::stoi(s.second));
       if(s.first == "seed_int")
           ui->lineEdit_Seed->setText(QString::fromStdString(s.second));
       if(s.first == "ann_type")
           ui->sb_ann->setValue(std::stoi(s.second));
   }
}

void MainWindow::readAndSetSettingsfromUi(){
    std::cout << "Reading settings from UI" << std::endl;

    std::map<std::string, std::string> settings;
    settings["n_agents_int"] = std::to_string((int)ui->sb_n_agents->value());
    settings["n_genomes_int"] = std::to_string((int)ui->sb_n_genomes->value());
    settings["n_steps_per_genome_int"] = std::to_string((int)ui->sb_n_steps_per_genome->value());
    settings["n_generations_int"] = std::to_string((int)ui->sb_n_generations->value());
    settings["ann_type"] = std::to_string((int)ui->sb_ann->value());
    settings["n_runs_int"] = "1";
    settings["elitism_int"] = std::to_string((int)ui->sb_elitism->value());
    settings["type_evo_int"] = "1";
    settings["mutation_rate_float"] = QString::number(ui->sb_mutation_rate->value()).toStdString();
    settings["ann_type"] = std::to_string((int)ui->sb_ann->value());;
    settings["seed_int"] = ui->lineEdit_Seed->text().toStdString();
    displaySettings(settings);
    ui->sB_GenomeNumber->setMaximum(std::stoi(settings["n_generations_int"])-1);
    ui->environmentWidget->setEnvironmentSettings(settings);
    ui->environmentWidget->update();
}

void MainWindow::on_pb_LogData_released()
{
    ui->environmentWidget->saveTrace();
}

void MainWindow::on_pushButtonStop_clicked()
{
    //stop timer
    timer->stop();
    ui->pushButtonPauseResume->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonResetRun->setEnabled(false);
    //reset experiment
    resetEnvironment();
}

void MainWindow::on_pushButtonPauseResume_clicked()
{
    if(running) {
        timer->stop();
        running = false;
        ui->pushButtonPauseResume->setText("Resume");
    } else {
        timer->start();
        running = true;
        ui->pushButtonPauseResume->setText("Pause");
    }
}

void MainWindow::on_pushButtonResetRun_clicked()
{
    //reset experiment
    resetEnvironment();
}

void MainWindow::startSimulation(int mode) {
    running = true;
    ui->environmentWidget->setEnvironmentMode(mode);
    timer->start();
    ui->pushButtonPauseResume->setEnabled(true);
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonResetRun->setEnabled(true);
}

void MainWindow::on_ReplayExperimentButton_clicked()
{
    //load first generation from file
    QString path = QString();
    path.append(QString::fromStdString(ui->environmentWidget->getEnvironment()->genome_folder));
    path.append(QString::fromStdString("gen_0"));
    std::vector<std::vector<float>> temp_genomes = Utils::readAllGenomesFromEvolutionFile(path);
    ui->environmentWidget->getEnvironment()->setPopulation(temp_genomes);
    startSimulation(2);
}

void MainWindow::on_ReplayGenomeButton_clicked()
{
    startSimulation(0);
}

void MainWindow::on_pushButtonLoadExperiment_clicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    QFileInfo info(results->filePath(index));
    qDebug() << "Load experiment " << info.fileName();
    std::map<std::string, std::string> settings = Utils::readSettingsFrom(info.filePath());
    displaySettings(settings);
    readAndSetSettingsfromUi();
    //update experiment folder
    qDebug() << "New exp folder is " << info.path();
    ui->environmentWidget->setExpFolder(info.path().toStdString());
}

void MainWindow::on_RunExperimentButton_clicked()
{
    if(ui->radioButton_Evolution->isChecked())
        startSimulation(1);
    else if (ui->radioButton_NSGA2->isChecked())
        startSimulation(3);
}

void MainWindow::on_pushButtonGenerateRandomANNs_clicked()
{
    ui->environmentWidget->getEnvironment()->generateRandomANNs();
}

void MainWindow::on_pushButton_NewRun_clicked()
{
    //TODO: Make new run
    //ui->environmentWidget->getEnvironment()->finished_run();
}
