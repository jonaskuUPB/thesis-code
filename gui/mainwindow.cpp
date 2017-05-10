#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include "Utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), ui->environmentWidget, SLOT(doStep()));
    connect(ui->pushButton_Reset, SIGNAL(clicked()), this, SLOT(resetEnvironment()));
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
        ui->playPushButton->setText("Pause");
    }
    else{
        timer->stop();
        ui->playPushButton->setText("Play");
    }
}

void MainWindow::on_playPushButton_clicked()
{
    ui->environmentWidget->replay = false;
    ui->environmentWidget->evolution = ui->checkBox->isChecked();
    toggle_simulation();
}

void MainWindow::setEnvironment(Environment* env){
    ui->environmentWidget->setEnvironment(env);
    displaySettings(ui->environmentWidget->getEnvironment()->getSettings());
}

void MainWindow::newSeed(){
    int s = rand() % 9000000000 + 1000000000;
    ui->lineEdit_Seed->setText(QString::number(s));
    readAndSetSettingsfromUi();
}

void MainWindow::resetEnvironment(){
    ui->environmentWidget->resetEnvironment();
    ui->environmentWidget->update();
}

void MainWindow::stepsChanged(unsigned long int s){
//    ui->label_Generation->setText(QString("step: ").append(QString::number(s)));
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
        std::string fitness = ui->environmentWidget->readAndSetGenomeFrom(ui->sB_GenomeNumber->value(), results->filePath(index));
        ui->label_Fitness->setText("Fitness: " + QString::fromStdString(fitness));
    }
    if(info.isFile() && info.fileName().startsWith("expId")){
        qDebug() << "Experiment set up with settings from " <<info.fileName() << ".";
        std::map<std::string, std::string> settings = ui->environmentWidget->readSettingsFrom(results->filePath(index));
        displaySettings(settings);
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
    ui->environmentWidget->setEnvironmentSettings(settings);
    ui->environmentWidget->update();
}

void MainWindow::on_pb_LogData_released()
{
    ui->environmentWidget->saveTrace();
}

void MainWindow::on_replayPushButton_clicked()
{
    ui->environmentWidget->replay = true;
    toggle_simulation();
}
