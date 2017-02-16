#include "configuration.h"

Configuration::Configuration()
{
}

Configuration &Configuration::getInstance()
{
    static Configuration instance;
    return instance;
}

void Configuration::set(std::string s, std::string t)
{
    settings[s] = t;
}

std::string Configuration::get(std::string s)
{
    return settings[s];
}
