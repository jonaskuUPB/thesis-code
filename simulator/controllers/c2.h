#ifndef C2_H
#define C2_H
#include "icontroller.h"

class C2 : public IController
{
public:
    C2();
    std::vector<float> compute(std::vector<float> input, std::vector<float> genome);
    std::string getInfo();
    float activation(float x);
    int getNumberOfGenes();
    int getActionLayerStart();
    std::string getType();
    float recurrent[4];
};

#endif // C2_H
