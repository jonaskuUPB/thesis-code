#ifndef THREADCLASS_H
#define THREADCLASS_H

#include <thread>
#include "simulator/environment.h"
#include "nsga2.h"

class ThreadClass
{
public:
    ThreadClass();
    void SetupEnvironment(std::string expName, std::map<std::string, std::string> s);
    void StartEvoProcess();
    void StartReplayProcess();
    void StartNSGA2Process(NSGA2Type nsga2Params, void *inp, void *out);
    void Join();
    bool IsJoinable();
    void ThreadEvoMain();
    void ThreadNSGA2Main(NSGA2Type nsga2Params, void *inp, void *out);
    void printResults();

private:
    std::thread t;
    Environment* env;
    bool post_evaluation = false;
};

#endif // THREADCLASS_H
