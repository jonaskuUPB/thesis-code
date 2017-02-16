#ifndef C5_H
#define C5_H
#include "icontroller.h"

class C5 : public IController
{
public:
    C5();
    std::vector<float> compute(std::vector<float> input, std::vector<float> genome);
    std::string getInfo();
    float activation(float x);
    int getNumberOfGenes();
    int getActionLayerStart();
    std::string getType();
    float recurrent[4];
};

#endif // C5_H
