#ifndef UTILS_H
#define UTILS_H
#include <map>
#include <QtGui>

namespace Utils {
    std::string createFolderStructure(int exp_id);
    std::map<std::string, std::string> readSettingsFrom(QString path);
    std::vector<float> readBestGenomeFrom(QString path);
    std::vector<std::vector<float>> readAllGenomesFrom(QString path);
    int newSeed();
}

#endif // UTILS_H
