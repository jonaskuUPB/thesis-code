#ifndef C1_H
#define C1_H
#include "icontroller.h"

class C1 : public IController
{
public:
    C1();
    std::vector<float> compute(std::vector<float> input, std::vector<float> genome);
    std::string getInfo();
    float activation(float x);
    int getNumberOfGenes();
    int getActionLayerStart();
    std::string getType();
    float recurrent[4];
};

#endif // C1_H
