#include "environmentwidget.h"
#include "mainwindow.h"

EnvironmentWidget::EnvironmentWidget(QWidget *parent) :
    QWidget(parent)
{
    _transform.translate(10.0f, 10.0f);
}

void EnvironmentWidget::setEnvironment(Environment* env){
    this->env = env;
}

Environment* EnvironmentWidget::getEnvironment(){
    return env;
}

void EnvironmentWidget::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setTransform(_transform);
    p.fillRect(0, 0, WIDTH, HEIGHT, Qt::white);
    p.drawRect(0, 0, WIDTH, HEIGHT);
    //env->wall->draw(&p);
    for(auto const a : env->getAgents()){
        if(a->selected)
            a->drawTrace(&p);
    }
    for(auto const a : env->getAgents()){
        a->draw(&p);
    }
}

void EnvironmentWidget::mousePressEvent(QMouseEvent *e){
    int px = e->x()-10;
    int py = e->y()-10;

    for(auto const a : env->getAgents()){
        if(fabs(a->getBody()->GetPosition().x - px) < 20.0 && fabs(a->getBody()->GetPosition().y - py) < 20.0)
            a->clicked();
    }
    update();
}

void EnvironmentWidget::doStep(){
    /*if(evolution){
        env->updateEnvironment();
    }else{
        env->updateWithoutEvoProcess();
    }*/
    env->updateEnvironment();
    update();
}

void EnvironmentWidget::saveTrace()
{
    env->saveCoveredDistance();
}

std::string EnvironmentWidget::readAndSetMOGenomeFrom(int value, QString path){
    QFile f(path);
    QStringList s;

    // currently only reading the first line => best fitness, since the file is sorted by decreasing fitness
    int counter = 0;
    if (f.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&f);
        QString line = in.readLine();
        line = in.readLine();
        line = in.readLine();
        while(counter < value){
            line = in.readLine();
            counter++;
        }
        s =  line.split("\t");
    }
    f.close();
    std::string fitness = s[0].toStdString();  // store fitness for gui
    s.removeAt(0);
    s.removeFirst(); //remove second objective
    s.removeLast(); //remove crowding distance
    s.removeLast(); //remove rank
    s.removeLast(); //remove constraint violation
    std::vector<float> temp_vector;
    for(auto const& f : s){
        qDebug() << f;
        temp_vector.push_back(f.toFloat());
    }
    emit initEnvWithSettingsFromUi();
    env->setGenomeForAllAgents(temp_vector);
    update();
    return fitness;
}

std::string EnvironmentWidget::readAndSetGenomeFrom(int value, QString path){
    QFile f(path);
    QStringList s;

    // currently only reading the first line => best fitness, since the file is sorted by decreasing fitness
    int counter = 0;
    if (f.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&f);
        QString line = in.readLine();
        while(counter < value){
            line = in.readLine();
            counter++;
        }
        s =  line.split(" ");
    }
    f.close();
    std::string fitness = s[0].toStdString();  // store fitness for gui
    s.removeAt(0);
    std::vector<float> temp_vector;
    for(auto const& f : s){
        temp_vector.push_back(f.toFloat());
    }
    emit initEnvWithSettingsFromUi();
    env->setGenomeForAllAgents(temp_vector);
    update();
    return fitness;
}

std::map<std::string, std::string> EnvironmentWidget::readSettingsFrom(QString path){
    QFile f(path);
    QStringList s;

    std::map<std::string, std::string> temp;

    if (f.open(QFile::ReadOnly | QFile::Text)){
        QTextStream in(&f);
        while(!in.atEnd()){
            QString first_line = in.readLine();
            s = first_line.split("=");
            temp[s[0].toStdString()] = s[1].toStdString();
        }
    }
    f.close();

    return temp;
}

void EnvironmentWidget::setEnvironmentSettings(std::map<std::string, std::string> s)
{
    env->setSettings(s);
    update();
}

void EnvironmentWidget::resetEnvironment(){
    env->reset();
}

void EnvironmentWidget::setLogging(){
    env->logging = true;
}

void EnvironmentWidget::setEnvironmentMode(int mode){
    switch (mode) {
    case 0:
        env->initGenomeReplay();
        break;
    case 1:
        env->initEvolution();
        break;
    case 2:
        env->initExperimentReplay();
        break;
    case 3:
        env->initMultiObjectiveOptimization();
        break;
    default:
        break;
    }
}

void EnvironmentWidget::setExpFolder(std::string path) {
    env->run_folder = path;
    env->genome_folder = path + "genomes/";
    env->trajectory_folder = path + "trajectory/";
    env->stats_folder = path + "stats/";
    env->settings_folder = path;
}
