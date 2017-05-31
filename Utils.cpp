#include "Utils.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <random>
#include <sys/types.h>


namespace Utils {

    std::string createFolderStructure(int exp_id) {
        std::string base_path = "results/";
        // current folder name (one per start of simulator)
        std::time_t t = std::time(NULL);
        char mbstr[100];
        std::strftime(mbstr, sizeof(mbstr), "%Y_%m_%d-%H_%M_%S/", std::localtime(&t));
        std::string s(mbstr);
        std::string current_folder = base_path.append(s);
        int dir_error = mkdir(current_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_error){
            std::cout << "Error creating directory:" << current_folder << std::endl;
        }
        // experiment folder based on experiment id
        std::string exp_folder = current_folder + std::to_string(exp_id) + "/";
        std::string genome_folder = exp_folder + "genomes/";
        std::string trajectory_folder = exp_folder + "trajectory/";
        std::string actions_folder = exp_folder + "actions/";
        std::string settings_folder = exp_folder;

        dir_error = mkdir(exp_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_error){
            std::cout << "Error creating directory:" << exp_folder << std::endl;
        }
        dir_error = mkdir(genome_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_error){
            std::cout << "Error creating directory:" << genome_folder << std::endl;
        }
        dir_error = mkdir(trajectory_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_error){
            std::cout << "Error creating directory:" << trajectory_folder << std::endl;
        }
        dir_error = mkdir(actions_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_error){
            std::cout << "Error creating directory:" << actions_folder << std::endl;
        }
        return exp_folder;
    }

    std::map<std::string, std::string> readSettingsFrom(QString path){
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


    std::vector<float> readBestGenomeFrom(QString path){
        QFile f(path);
        QStringList s;
        std::vector<float> return_vector;

        if (f.open(QFile::ReadOnly | QFile::Text)){
            QTextStream in(&f);
            QString line = in.readLine();
            s = line.split(" ");
        }
        f.close();
        s.removeAt(0); // remove first fitness entry
        for(auto const& f : s){
            return_vector.push_back(f.toFloat());
        }
        return return_vector;
    }

    std::vector<std::vector<float>> readAllGenomesFrom(QString path){
        QFile f(path);
        QStringList s;
        std::vector<std::vector<float>> return_vector;
        if (f.open(QFile::ReadOnly | QFile::Text)){
            QTextStream in(&f);
            while(!in.atEnd()){
                QString line = in.readLine();
                s = line.split(" ");
                s.removeAt(0); // remove first fitness entry
                std::vector<float> temp_vector;
                for(auto const& f : s){
                    temp_vector.push_back(f.toFloat());
                }
                return_vector.push_back(temp_vector);
            }
        }
        f.close();
        return return_vector;
    }

    int newSeed() {
        int s = rand() % 9000000000 + 1000000000;
        return s;
    }
}
