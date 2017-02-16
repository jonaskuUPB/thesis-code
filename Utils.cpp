#include "Utils.h"


namespace Utils {

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
}
