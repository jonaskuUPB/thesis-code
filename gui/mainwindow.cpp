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

    //set signals for buttons and timeouts
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
        //TODO: Load experiment settings from here as well?!
        std::string fitness = ui->environmentWidget->readAndSetGenomeFrom(ui->sB_GenomeNumber->value(), results->filePath(index));
        ui->label_Fitness->setText("Fitness: " + QString::fromStdString(fitness));
    }
    if(info.isFile() && info.fileName().startsWith("expId")){
        qDebug() << "Experiment set up with settings from " <<info.fileName() << ".";
        std::map<std::string, std::string> settings = ui->environmentWidget->readSettingsFrom(results->filePath(index));
        displaySettings(settings);
        readAndSetSettingsfromUi();
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
    std::vector<std::vector<float>> temp_genomes = Utils::readAllGenomesFrom(path);
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
    startSimulation(1);
}

void MainWindow::on_pushButtonGenerateRandomANNs_clicked()
{
    //std::string exp_folder = Utils::createFolderStructure(0);
    std::string exp_folder = ui->environmentWidget->getEnvironment()->exp_folder;
    std::map<std::string, std::string> settings = ui->environmentWidget->getEnvironment()->settings;
    std::string genome_folder = exp_folder + "genomes/";
    std::mt19937 mt;
    mt.seed(std::stoi(settings["seed_int"]));
    for(unsigned int gen = 0; gen < std::stoi(settings["n_generations_int"]); gen++){
        std::string fname = "gen_" + std::to_string(gen);
        std::ofstream file;
        file.open(genome_folder+fname);
        std::uniform_real_distribution<float> uniform_rand_float(-0.5,0.5);
        for(unsigned int i = 0; i < std::stoi(settings["n_genomes_int"]); i++){
            file << 0; //dummy fitness
            for(unsigned int k = 0; k < 60; k++){
                float float_value = uniform_rand_float(mt);
                file << " " << float_value;
            }
            file << "\n";
        }
        file.close();
    }
}
