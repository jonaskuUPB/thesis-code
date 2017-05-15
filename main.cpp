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

bool gui = true;
bool randomize = true;
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
    if(randomize){
        std::string exp_folder = Utils::createFolderStructure(0);
        std::string genome_folder = exp_folder + "genomes/";
        std::string fname = "gen_" + std::to_string(0);
        std::ofstream file;
        file.open(genome_folder+fname);
        std::mt19937 mt;
        mt.seed(std::stoi("0"));
        std::uniform_real_distribution<float> uniform_rand_float(-0.5,0.5);
        for(unsigned int i = 0; i < 40; i++){
            file << 0;
            for(unsigned int k = 0; k < 40; k++){
                float float_value = uniform_rand_float(mt);
                file << " " << float_value;
            }
            file << "\n";
        }
        file.close();
    } else
        if(gui){
            guiRun(argc, argv);
        }else{

            unsigned concurrentThreadsSupported = std::thread::hardware_concurrency();
            std::cout << "Number of supported threads: " << concurrentThreadsSupported << std::endl;

            //int numberOfExperiments = concurrentThreadsSupported * 4;
            int numberOfExperiments =  4;

            std::queue<ThreadClass*> q;

            std::string controller[] = {"1","2","3","4","5"};
            std::string n_agents[] = {"25","50","75","100"};
            srand(time(NULL));

            for(int i = 0; i < numberOfExperiments; i++){
                int s = rand() % 9000000000 + 1000000000;
                default_settings["seed_int"] = std::to_string(s);
                default_settings["ann_type"] = controller[3];
                default_settings["n_agents_int"] = n_agents[i];
                ThreadClass* t = new ThreadClass();
                t->SetupEnvironment(default_settings);
                q.push(t);
            }

            while(!q.empty()){
                std::vector<ThreadClass*> t_list;

                for(unsigned int i = 0; i < concurrentThreadsSupported; i++){
                    if(!q.empty()){
                        t_list.push_back(q.front());
                        q.pop();
                    }
                }

                for(auto &t : t_list){
                    t->StartEvoProcess();
                }

                for(auto &t : t_list)
                    t->Join();

            }
        }
}
