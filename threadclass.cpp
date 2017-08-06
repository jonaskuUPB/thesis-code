#include "threadclass.h"
#include "Utils.h"
#include <fstream>


ThreadClass::ThreadClass():t() {}

void ThreadClass::SetupEnvironment(std::string expName, std::map<std::string, std::string> s)
{
    env = new Environment(expName);
    env->setupExperiment(s);
}

void ThreadClass::StartEvoProcess()
{
    env->initEvolution();
    t = std::thread(&ThreadClass::ThreadEvoMain,this);
}

void ThreadClass::StartReplayProcess(){
    env->initExperimentReplay();
    env->generateRandomANNs();
    QString path = QString();
    path.append(QString::fromStdString(env->genome_folder));
    path.append(QString::fromStdString("gen_0"));
    std::vector<std::vector<float>> temp_genomes = Utils::readAllGenomesFrom(path);
    env->setPopulation(temp_genomes);
    t = std::thread(&ThreadClass::ThreadEvoMain,this);
}

void ThreadClass::StartNSGA2Process(NSGA2Type nsga2Params, void *inp, void *out) {
    t = std::thread(&ThreadClass::ThreadNSGA2Main,this, nsga2Params, inp, out);
}

void ThreadClass::ThreadNSGA2Main(NSGA2Type nsga2Params, void *inp, void *out) {
    InitNSGA2(&nsga2Params, inp, out, Environment::setNSGA2Genome);
    NSGA2(&nsga2Params, inp, out, Environment::setNSGA2Genome);
}

void ThreadClass::Join()
{
    t.join();
}

bool ThreadClass::IsJoinable()
{
    return t.joinable();
}

void ThreadClass::ThreadEvoMain()
{
    while(!env->finished){
        env->updateEnvironment();
    }

    /*if(post_evaluation){
        //std::cout << "Starting simulation of experiment " << env->expId << std::endl;
        std::vector<std::vector<float>> data_action;
        std::vector<std::vector<float>> data_speed;
        std::vector<std::vector<float>> data_rotation;
        int max_steps = 3000;

        int n_gens = std::stoi(env->settings["n_generations_int"]);
        for(int g = 0; g < n_gens; g++){
            std::string f_path = env->genome_folder + "gen_"+ std::to_string(g);
            std::vector<float> genome = Utils::readBestGenomeFrom(QString::fromStdString(f_path));
            env->reset();
            env->setGenomeForAllAgents(genome);

            std::vector<float> temp_actions_per_gen;
            std::vector<float> temp_speeds_per_gen;
            std::vector<float> temp_rotations_per_gen;
            for(int step = 0; step < max_steps; step++){
                env->doWorldStep();
                temp_actions_per_gen.push_back(env->getAverageAction());
                temp_speeds_per_gen.push_back(env->getAverageSpeed());
                temp_rotations_per_gen.push_back(env->getAverageDeltaAngle());
            }

            data_action.push_back(temp_actions_per_gen);
            data_speed.push_back(temp_speeds_per_gen);
            data_rotation.push_back(temp_rotations_per_gen);
        }

        std::ofstream file_stream;

        std::string action_fname = env->stats_folder+"ActionsPerBestGenomePerGeneration";
        file_stream.open(action_fname);
        for(auto line : data_action){
            for(auto v : line)
                file_stream << v << " ";
            file_stream << "\n";
        }
        file_stream.close();
        data_action.clear();

        std::string speed_fname = env->stats_folder+"SpeedPerBestGenomePerGeneration";
        file_stream.open(speed_fname);
        for(auto line : data_speed){
            for(auto v : line)
                file_stream << v << " ";
            file_stream << "\n";
        }
        file_stream.close();
        data_speed.clear();

        std::string rotation_fname = env->stats_folder+"RotationPerBestGenomePerGeneration";
        file_stream.open(rotation_fname);
        for(auto line : data_rotation){
            for(auto v : line)
                file_stream << v << " ";
            file_stream << "\n";
        }
        file_stream.close();
        data_rotation.clear();

        std::string f_path = env->genome_folder + "gen_"+ std::to_string(n_gens-1);
        std::vector<std::vector<float>> last_gens_genomes = Utils::readAllGenomesFrom(QString::fromStdString(f_path));
        for(auto g : last_gens_genomes){
            env->reset();
            env->setGenomeForAllAgents(g);

            std::vector<float> temp_actions_per_gen;
            std::vector<float> temp_speeds_per_gen;
            std::vector<float> temp_rotations_per_gen;
            for(int step = 0; step < max_steps; step++){
                env->doWorldStep();
                temp_actions_per_gen.push_back(env->getAverageAction());
                temp_speeds_per_gen.push_back(env->getAverageSpeed());
                temp_rotations_per_gen.push_back(env->getAverageDeltaAngle());
            }

            data_action.push_back(temp_actions_per_gen);
            data_speed.push_back(temp_speeds_per_gen);
            data_rotation.push_back(temp_rotations_per_gen);
        }

        action_fname = env->stats_folder+"ActionsPerGenomePerLastGeneration";
        file_stream.open(action_fname);
        for(auto line : data_action){
            for(auto v : line)
                file_stream << v << " ";
            file_stream << "\n";
        }
        file_stream.close();
        data_action.clear();

        speed_fname = env->stats_folder+"SpeedPerGenomePerLastGeneration";
        file_stream.open(speed_fname);
        for(auto line : data_speed){
            for(auto v : line)
                file_stream << v << " ";
            file_stream << "\n";
        }
        file_stream.close();
        data_speed.clear();

        rotation_fname = env->stats_folder+"RotationPerGenomePerLastGeneration";
        file_stream.open(rotation_fname);
        for(auto line : data_rotation){
            for(auto v : line)
                file_stream << v << " ";
            file_stream << "\n";
        }
        file_stream.close();
        data_rotation.clear();
    }*/
}

void ThreadClass::printResults(){
    env->printResults();
}
