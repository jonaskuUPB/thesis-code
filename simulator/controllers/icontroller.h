#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <vector>
#include <string>


class IController
{
public:
    virtual std::vector<float> compute(std::vector<float> input, std::vector<float> genome) = 0;
    virtual std::string getInfo() = 0;
    virtual int getNumberOfGenes() = 0;
    virtual int getActionLayerStart() = 0;
    virtual std::string getType() = 0;
    virtual float activation(float x) = 0;
};

#endif // CONTROLLER_H
