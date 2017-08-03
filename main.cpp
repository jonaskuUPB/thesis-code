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

bool gui = true;
bool nsga = true;
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

    if(nsga){// create environment with default settings
        Environment* env = new Environment();
        env->setupExperiment(default_settings);

        NSGA2Type nsga2Params;
        void *inp = env;
        void *out = NULL;

        double *minReal = (double *)malloc(2*sizeof(double));
        minReal[0] = 0;
        minReal[1] = 0;
        double *maxReal = (double *)malloc(2*sizeof(double));
        maxReal[0] = 5;
        maxReal[1] = 3;

        int *numBits = (int *)malloc(1*sizeof(int));
        double *minBin = (double *)malloc(1*sizeof(double));
        double *maxBin = (double *)malloc(1*sizeof(double));

        nsga2Params = SetParameters(0.5, 100, 150, 2, 2, 2, minReal, maxReal, 0.9, 0.5, 10, 20, 0, numBits, minBin, maxBin, 0, 0, 1, 1, 2, 0, 0, 0, 0);
        InitNSGA2(&nsga2Params, inp, out, env->setNSGA2Genome);
        NSGA2(&nsga2Params, inp, out, env->setNSGA2Genome);
        exit(0);
    }
    if(gui){
        guiRun(argc, argv);
    }else{
        //Run an evolution
        ThreadClass* t = new ThreadClass();
        t->SetupEnvironment("evolution", default_settings);
        t->StartEvoProcess();
        t->Join();
        //Run a replay
        ThreadClass* t2 = new ThreadClass();
        t2->SetupEnvironment("control", default_settings);
        t2->StartReplayProcess();
        t2->Join();
        //finally turn of the computer
        //system("shutdown -P now");
    }
}
