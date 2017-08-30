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

    /*
     * Reads all genomes from a list of " " (space) seperated values.
     * Specify how many entries before and after the genome are in that file (per line). They are omitted.
     */
    std::vector<std::vector<float>> readAllGenomesFromFile(QString path, int entriesBefore, int entriesAfter) {
        QFile f(path);
        QStringList s;
        std::vector<std::vector<float>> return_vector;
        if (f.open(QFile::ReadOnly | QFile::Text)){
            QTextStream in(&f);
            while(!in.atEnd()){
                QString line = in.readLine();
                s = line.split(" ");
                for (int i = 0; i < entriesBefore; i++){
                    s.removeFirst();
                }
                for (int i = 0; i < entriesAfter; i++){
                    s.removeLast();
                }
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

    std::vector<float> readBestGenomeFromEvolutionFile(QString path){
        /*QFile f(path);
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
        return return_vector;*/
        return readAllGenomesFromEvolutionFile(path)[0];
    }

    std::vector<std::vector<float>> readAllGenomesFromEvolutionFile(QString path){
        return readAllGenomesFromFile(path, 1,0);
    }

    int newSeed() {
        int s = rand() % 9000000000 + 1000000000;
        return s;
    }
}
