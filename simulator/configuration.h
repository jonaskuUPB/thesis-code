#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <string>
#include <map>

class Configuration
{
public:
    static Configuration& getInstance();
    void set(std::string s, std::string t);
    std::string get(std::string s);
private:
    Configuration();
    Configuration(const Configuration&);
    Configuration& operator=(const Configuration&);
    std::map<std::string,std::string> settings;
};

#endif // CONFIGURATION_H
