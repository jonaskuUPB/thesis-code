#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QWidget>
#include <QtGui>
#include <QtOpenGL>
#include <Box2D/Box2D.h>
#include <stdio.h>
#include "agent.h"
#include "wall.h"
#include <QTimer>
#include <iostream>
#include <mutex>
#include <set>

#include "contactlistener.h"

#define WIDTH   1280
#define HEIGHT  720

class Environment : QObject
{
public:
    //constructor and destructor
    explicit Environment(std::string expName = "");
    virtual ~Environment();

    std::string expId = "";

    //called when settings change
    void setupExperiment(std::map<std::string, std::string> s);

    //alternative calls for settings
    void reset();
    void setSettings(std::map<std::string, std::string> settings);
    std::map<std::string, std::string> getSettings();
    std::map<std::string, std::string> settings;

    //methods and attributes maintaining the population
    std::vector<float>mutate(std::vector<float> genome, float f);
    std::vector<float>mutateP(std::vector<float> genome, float f);
    int rouletteWheelSelection(std::vector<float> fitnesses);
    void generateRandomANNs();
    void setPopulation(std::vector<std::vector<float>> genomes);
    std::vector<std::vector<float>> population_genomes;
    std::vector<std::vector<float>> population_genomes_backup;
    std::vector<std::vector<std::vector<float>>> all_genomes;

    //attributes for the fitness
    std::vector<std::vector<std::vector<float>>> run_fitnesses;
    std::vector<std::vector<float>> generation_fitnesses;
    std::vector<float> genome_fitnesses;
    float current_fitness = 0.0;

    //methods to calculate stats
    void calculateClusters();
    void savePoseAndSpeed();
    void saveActions();
    void saveCoveredDistance();
    float getAverageSpeed();
    float getAverageDeltaAngle();
    float getAverageAction();
    std::vector<std::vector<float>> data_action;
    std::vector<std::vector<float>> data_speed;
    std::vector<std::vector<float>> data_rotation;
    std::vector<std::vector<int>> data_cluster_size;
    std::vector<std::vector<float>> data_distance;
    std::vector<std::vector<float>> data_k_distance;
    std::vector<float> temp_actions_per_gen;
    std::vector<float> temp_speeds_per_gen;
    std::vector<float> temp_rotations_per_gen;
    std::vector<int> temp_cluster_size_per_gen;
    std::vector<float> temp_distance_per_gen;
    std::vector<float> temp_k_distance_per_gen;

    //logging
    bool logging = false;
    int logging_steps = 3000;
    std::vector<std::vector<float>> log_actions;

    //methods and attributes taking care of the folder structure
    void setup_run_folder();
    std::string base_path = "results/";
    std::string exp_folder;
    std::string genome_folder;
    std::string trajectory_folder;
    std::string settings_folder;
    std::string run_folder;
    std::string stats_folder;

    int mode = 1; //0=play single genome; 1=evolution; 2=replay_experiment

    //interaction
    void mousePressEvent(QMouseEvent *e);
    void clickedAgent();

    //updateMethods: TODO delete unecessary ones
    void updateEnvironment();
    void updateWithoutEvoProcess();
    void doWorldStep();


    //MISC:
    std::vector<Agent*> &getAgents();
    b2World* getWorld();
    bool isRunning();
    Wall *wall;
    std::set<std::set<int>> cluster;
    std::map<int, float> distance;
    int genome_counter = 0;
    void setGenomeForAllAgents(std::vector<float> genome);
    void printResults();
    bool finished = false;
    bool environment_initialized = false;

signals:
    void genomeFinished();
    void generationFinished();
    void runFinished();

protected:
    //int run_id = 0;


    b2World* _world;
    QTimer timer;
    int steps_to_make = 0;

    std::vector<Agent*> agents;
    bool running = true;
    bool genome_set = false;
    ContactListener* cl;
    int timeStepScale = 1;
    unsigned long int steps = 0;
    int run_counter = 0;
    int generation_counter = 0;
    int step_counter = 0;

    //setting variables
    int setting_steps_per_genome = 0;
    int setting_elitism = 0;
    int setting_n_generations = 0;
    int setting_n_genomes = 0;
    int setting_n_runs = 0;
    int setting_type_evo = 0;
    int seed = 0;

    static std::mutex cout_mutex;
    float mutation_rate = 0.1;
    int action_layer_start = 0;

    std::mt19937 mt;

private:
    void finished_genome();
    void finished_generation();
    void finished_run();
    void finished_experiment();

    std::vector<float> next_genome;

    void save_genome_stats();
    void save_generation_stats(std::vector<int> sorted_indices);

    std::time_t start_gen_time;

};

#endif
