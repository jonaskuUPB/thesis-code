#include "c5.h"
#include <math.h>

C5::C5()
{
    recurrent[0] = 1.0;
    recurrent[1] = 1.0;
    recurrent[2] = 1.0;
    recurrent[3] = 1.0;
}

std::string C5::getInfo(){
    return "no info";
}

int C5::getNumberOfGenes(){
    return 52;
}

int C5::getActionLayerStart()
{
    return -1;
}

std::string C5::getType(){
    return "5";
}

float C5::activation(float x){
    // sigmoid
    return 2.0/(1.0+exp(-2.0*x))-1.0;
}

std::vector<float> C5::compute(std::vector<float> input, std::vector<float> genome){
    std::vector<float> output;

    int n_first_outputs = 2;
    float prediction_first_output[n_first_outputs];
    prediction_first_output[0] = activation(genome[0] * input[0]);
    prediction_first_output[1] = activation(genome[1] * input[1]);

    int n_second_outputs = 4;
    int n_second_neurons = 4;
    float prediction_second_output[n_second_outputs];
    int second_layer_start = 2;
    for(int i = 0; i < n_second_neurons; i++){
        for(int j = 0; j < n_first_outputs; j++){
           prediction_second_output[i] += genome[(second_layer_start+i*(n_first_outputs+1)) + j] * prediction_first_output[j];
        }
        prediction_second_output[i] = activation(prediction_second_output[i] + genome[second_layer_start+i*(n_first_outputs+1) + 2]*recurrent[i]);
        recurrent[i] = prediction_second_output[i];
    }

    int n_third_neurons = 2;
    float prediction_third_output[n_third_neurons];
    int third_layer_start = 14;
    for(int i = 0; i < n_third_neurons; i++){
        for(int j = 0; j < n_second_outputs; j++){
           prediction_third_output[i] += genome[(third_layer_start+i*n_second_outputs) + j] * prediction_second_output[j];
        }
        prediction_third_output[i] = activation(prediction_third_output[i]);
    }

    output.push_back(prediction_third_output[0] > 0.0 ? 1.0 : 0.0);
    output.push_back(prediction_third_output[1] > 0.0 ? 1.0 : 0.0);

    // action network
    int action_weights_start = 22;
    int n_action_inputs = 3;
    float action_first_output[n_action_inputs];
    action_first_output[0] = activation(genome[action_weights_start+0] * input[0] + genome[action_weights_start+1]);
    action_first_output[1] = activation(genome[action_weights_start+2] * input[1] + genome[action_weights_start+3]);
    action_first_output[2] = activation(genome[action_weights_start+4] * input[2] + genome[action_weights_start+5]);

    // hidden node outputs
    int n_hidden_neurons = 4;
    int hidden_weights_start = 28;
    float hidden_outputs[n_hidden_neurons];
    for(int i = 0; i < n_hidden_neurons; i++){
        for(int j = 0; j < n_action_inputs; j++){
            hidden_outputs[i] += genome[hidden_weights_start+(i*n_action_inputs) + j] * action_first_output[j];
        }
        hidden_outputs[i] = activation(hidden_outputs[i]);
    }

    int n_outputs_neurons = 3;
    int output_weights_start = 40;
    float action_output[3];

    for(int i = 0; i < n_outputs_neurons; i++){
        for(int j = 0; j < n_hidden_neurons; j++){
            action_output[i] += genome[output_weights_start+(i*n_hidden_neurons) + j] * hidden_outputs[j];
        }
        action_output[i] = activation(action_output[i]);
    }

    output.push_back(action_output[0]);
    output.push_back(0.5 * action_output[1] + 0.5);
    output.push_back(action_output[2]);

    return output;
}
