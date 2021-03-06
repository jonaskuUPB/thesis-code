#include <QtGui>
#include <QtOpenGL>
#include <stdio.h>
#include <iostream>
#include <queue>
#include "gui/mainwindow.h"
#include "threadclass.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <random>
#include <sys/types.h>
#include "nsga2.h"

bool gui = false; //if true the GUI will be shown, otherwise all unsupervised runs are started
bool nsga = false; //starts one unsupervised run with NSGA-2
bool evo = false; //starts one unsupervised run with Evolution
bool randANN = false; //starts one unsupervised run with random ANNs
std::map<std::string, std::string> default_settings;

int guiRun(int argc, char** argv){
    QApplication app(argc, argv);

    // create environment with default settings
    Environment* env = new Environment();
    env->setupExperiment(default_settings);

    // and pass it to the ui
    MainWindow *w = new MainWindow(0);
    w->setWindowTitle("Simulation Environment");
    w->setEnvironment(env);
    w->show();
    return app.exec();
}

int main(int argc, char** argv)
{
    // read default settings from file
    default_settings = Utils::readSettingsFrom("results/default_settings");
    srand(time(NULL));
    default_settings["seed_int"] = std::to_string(Utils::newSeed());

    if(argc > 0) {
        for(int i = 0; i < argc; i++) {
            if(strcmp(argv[i],"-g")==0) {
                gui = true;
            }
            if(strcmp(argv[i],"-n")==0) {
                nsga = true;
            }
            if(strcmp(argv[i],"-e")==0) {
                evo = true;
            }
            if(strcmp(argv[i],"-c")==0) {
                randANN = true;
            }
        }
    }

    if(gui){
        guiRun(argc, argv);
    }else{
        ThreadClass* t = new ThreadClass();
        t->SetupEnvironment("replay", default_settings);
        t->StartReplayProcess("results/control-2017_06_24-10_08_21/run_0/", "settings_run_0_1260797451");
        t->Join();
        /*
        if(nsga){
            //Run an evolution
            ThreadClass* t = new ThreadClass();
            t->SetupEnvironment("mo", default_settings);
            t->StartMOProcess();
            t->Join();
        }
        if(evo){
            //Run an evolution
            ThreadClass* t = new ThreadClass();
            t->SetupEnvironment("evolution", default_settings);
            t->StartEvoProcess();
            t->Join();
        }
        if(randANN){
            //Run a replay
            ThreadClass* t2 = new ThreadClass();
            t2->SetupEnvironment("control", default_settings);
            t2->StartControlExperimentProcess();
            t2->Join();
        }
        //finally turn of the computer
        //system("shutdown -P now");
        */
    }
}
