#ifndef C4_H
#define C4_H
#include "icontroller.h"

class C4 : public IController
{
public:
    C4();
    std::vector<float> compute(std::vector<float> input, std::vector<float> genome);
    std::string getInfo();
    float activation(float x);
    int getNumberOfGenes();
    int getActionLayerStart();
    std::string getType();
    float recurrent[4];
};

#endif // C4_H
