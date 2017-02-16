#ifndef C3_H
#define C3_H
#include "icontroller.h"

class C3 : public IController
{
public:
    C3();
    std::vector<float> compute(std::vector<float> input, std::vector<float> genome);
    std::string getInfo();
    float activation(float x);
    int getNumberOfGenes();
    int getActionLayerStart();
    std::string getType();
    float recurrent[6];
};

#endif // C3_H
