#ifndef THREADCLASS_H
#define THREADCLASS_H

#include <thread>
#include "simulator/environment.h"

class ThreadClass
{
public:
    ThreadClass();
    void SetupEnvironment(std::string expName, std::map<std::string, std::string> s);
    void StartEvoProcess();
    void StartReplayProcess();
    void Join();
    bool IsJoinable();
    void ThreadEvoMain();
    void printResults();

private:
    std::thread t;
    Environment* env;
    bool post_evaluation = false;
};

#endif // THREADCLASS_H
