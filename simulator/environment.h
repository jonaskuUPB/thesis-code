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
    explicit Environment();
    virtual ~Environment();
    void setupExperiment(std::map<std::string, std::string> s);
    void reset();
    void makeSteps(int i);
    void setSettings(std::map<std::string, std::string> settings);
    std::map<std::string, std::string> getSettings();
    void mousePressEvent(QMouseEvent *e);
    std::vector<Agent*> &getAgents();
    b2World* getWorld();
    bool isRunning();
    std::vector<float>mutate(std::vector<float> genome, float f);
    std::vector<float>mutateP(std::vector<float> genome, float f);
    int rouletteWheelSelection(std::vector<float> fitnesses);
    std::vector<std::vector<float>> population_genomes;
    std::vector<std::vector<float>> population_genomes_backup;

    std::vector<std::vector<std::vector<float>>> all_genomes;
    std::vector<std::vector<std::vector<float>>> run_fitnesses;
    std::vector<std::vector<float>> generation_fitnesses;
    std::vector<float> genome_fitnesses;
    void calculateClusters();
    void savePoseAndSpeed();
    void saveActions();
    void saveCoveredDistance();
    float current_fitness = 0.0;
    int genome_counter = 0;
    void clickedAgent();
    void stepsMade(unsigned long int s);
    void avgFitness(float f);
    void toggle_simulation_step();
    void updateEnvironment();
    void updateWithoutEvoProcess();
    void doWorldStep();
    float getAverageSpeed();
    float getAverageDeltaAngle();
    float getAverageAction();
    void setGenomeForAllAgents(std::vector<float> genome);
    void setTimeStepScale(int s);
    void set_evolution_type(bool t);
    void printResults();
    bool finished = false;
    bool environment_initialized = false;
    bool logging = false;
    int logging_steps = 3000;
    std::vector<std::vector<float>> log_actions;

    std::vector<std::vector<float>> data_action;
    std::vector<std::vector<float>> data_speed;
    std::vector<std::vector<float>> data_rotation;
    std::vector<float> temp_actions_per_gen;
    std::vector<float> temp_speeds_per_gen;
    std::vector<float> temp_rotations_per_gen;

    Wall *wall;
    std::set<std::set<int>> cluster;
    std::map<int, float> distance;

    std::string base_path = "results/";
    static std::string current_folder;
    std::string genome_folder;
    std::string trajectory_folder;
    std::string settings_folder;
    std::string exp_folder;
    std::string actions_folder;

    std::map<std::string, std::string> settings;
    int exp_id = 0;

    int mode = 1; //0=play single genome; 1=evolution; 2=replay_experiment

    void generateRandomANNs();
    void setPopulation(std::vector<std::vector<float>> genomes);

signals:
    void genomeFinished();
    void generationFinished();
    void runFinished();

protected:
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
    int setting_steps_per_genome = 0;
    int setting_elitism = 0;
    int setting_n_generations = 0;
    int setting_n_genomes = 0;
    int setting_n_runs = 0;
    int setting_type_evo = 0;
    int seed = 0;
    static std::mutex cout_mutex;
    static int exp_counter;
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
