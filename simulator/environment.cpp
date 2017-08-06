#include "environment.h"
#include <Box2D/Box2D.h>
#include <iostream>
#include "threadclass.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <fstream>
#include <random>
#include "controllers/c1.h"
#include "controllers/c2.h"
#include "controllers/c3.h"
#include "controllers/c4.h"
#include "controllers/c5.h"
#include "Utils.h"
#include "nsga2.h"

std::mutex Environment::cout_mutex;

/*
 * Create an environment for a single experiment. An experiment consists of one or more runs that are executed with the same settings.
 */
Environment::Environment(std::string expName /*= ""*/)
{
    run_counter =0;
    // first experiment creates parent folder
    if(run_counter == 0){
        // current folder name (one per start of simulator)
        std::time_t t = std::time(NULL);
        char mbstr[100];
        std::strftime(mbstr, sizeof(mbstr), "%Y_%m_%d-%H_%M_%S", std::localtime(&t));
        std::string s(mbstr);
        expId = expName + "-" + s;
        exp_folder = base_path.append(expId+ "/");
        int dir_error = mkdir(exp_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_error){
            std::cout << "Error creating directory:" << exp_folder << std::endl;
        }
    }

    setup_run_folder();

    cl = new ContactListener();
}

Environment::~Environment() {
}

void Environment::setup_run_folder() {
    // experiment folder based on experiment id
    run_folder = exp_folder + "run_" + std::to_string(run_counter) + "/";
    genome_folder = run_folder + "genomes/";
    trajectory_folder = run_folder + "trajectory/";
    stats_folder = run_folder + "stats/";
    settings_folder = run_folder;

    int dir_error = mkdir(run_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_error){
        std::cout << "Error creating directory:" << run_folder << std::endl;
    }
    dir_error = mkdir(genome_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_error){
        std::cout << "Error creating directory:" << genome_folder << std::endl;
    }
    dir_error = mkdir(trajectory_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_error){
        std::cout << "Error creating directory:" << trajectory_folder << std::endl;
    }
    dir_error = mkdir(stats_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_error){
        std::cout << "Error creating directory:" << stats_folder << std::endl;
    }
}

void Environment::setupExperiment(std::map<std::string, std::string> s){
    std::cout << "Setting up experiment " << run_folder << "."  << std::endl;
    settings = s;
    std::string m_rate = settings["mutation_rate_float"];
    std::replace(m_rate.begin(),m_rate.end(),',','.');
    mutation_rate = std::atof(m_rate.c_str());
    mt.seed(std::stoi(settings["seed_int"]));
    setting_steps_per_genome = std::stoi(settings["n_steps_per_genome_int"]);
    setting_elitism = std::stoi(settings["elitism_int"]);
    setting_n_generations = std::stoi(settings["n_generations_int"]);
    setting_n_runs = std::stoi(settings["n_runs_int"]);
    setting_n_genomes = std::stoi(settings["n_genomes_int"]);
    b2Vec2 gravity(0.0f, 0.0f);
    _world = new b2World(gravity);
    _world->SetAllowSleeping(false);
    _world->SetContactListener(cl);

    // write settings to file
    std::string fname = "settings_run_";
    fname.append(std::to_string(run_counter)).append("_").append(settings["seed_int"]);
    std::ofstream file;
    file.open(settings_folder+fname);
    for(auto const& s : settings){
        file << s.first << "=" << s.second << "\n";
    }
    file.close();


    std::uniform_int_distribution<int> uniform_rand(0,RAND_MAX);
    std::cout << "Exp_id " << expId << "\t Seed:" << settings["seed_int"] << std::endl;

    Agent::counter = 0;
    int number_of_genes = 0;
    // setup controller based on settings
    for (int i = 0; i < std::stoi(settings["n_agents_int"]); i++) {
        IController* controller;
        if(settings["ann_type"] == "1"){
            controller = new C1();
        }
        if(settings["ann_type"] == "2"){
            controller = new C2();
        }
        if(settings["ann_type"] == "3"){
            controller = new C3();
        }
        if(settings["ann_type"] == "4"){
            controller = new C4();
        }
        if(settings["ann_type"] == "5"){
            controller = new C5();
        }
        number_of_genes = controller->getNumberOfGenes();
        action_layer_start = controller->getActionLayerStart();
        int dx = uniform_rand(mt) % (WIDTH - 10) + 5;
        int dy = uniform_rand(mt) % (HEIGHT - 10) + 5;
        int angle = ((uniform_rand(mt) / (RAND_MAX / 360.0f)) - 180.0f);
        agents.push_back(new Agent(_world, controller, b2Vec2(dx, dy), 20.0f, angle, 1.0f));
    }

    population_genomes.clear();
    if(settings["type_evo_int"] == "1"){ // homogen

        std::uniform_real_distribution<float> uniform_rand_float(-0.5,0.5);
        for(int i = 0; i < std::stoi(settings["n_genomes_int"]); i++){

            std::vector<float>temp_genome;
            for(int j = 0; j < number_of_genes; j++){
                float f = uniform_rand_float(mt);
                temp_genome.push_back(f);
            }
            population_genomes.push_back(temp_genome);
        }
    }
    population_genomes_backup = population_genomes;
    run_counter = 0;
    generation_counter = 0;
    current_fitness = 0.0;
    finished = false;

    genome_counter = 0;
    for(auto const& a : agents){
        a->genome = population_genomes[genome_counter]; // initial genome from population
    }
    next_genome = population_genomes[genome_counter+1];

    run_fitnesses.clear();
    generation_fitnesses.clear();
    genome_fitnesses.clear();
    steps = 0;
    // some initial steps
    for(int i = 0; i < 5; i++){
        _world->Step(1.0f / 60.0f, 8, 3);
    }


    start_gen_time = std::time(nullptr);

    environment_initialized = true;
}

void Environment::generateRandomANNs(){
    for(unsigned int gen = 0; gen < std::stoi(settings["n_generations_int"]); gen++){
        std::string fname = "gen_" + std::to_string(gen);
        std::ofstream file;
        file.open(genome_folder+fname);
        std::mt19937 mt;
        mt.seed(std::stoi("0"));
        std::uniform_real_distribution<float> uniform_rand_float(-0.5,0.5);
        for(unsigned int i = 0; i < std::stoi(settings["n_genomes_int"]); i++){
            file << 0; //dummy fitness
            for(unsigned int k = 0; k < 60; k++){
                float float_value = uniform_rand_float(mt);
                file << " " << float_value;
            }
            file << "\n";
        }
        file.close();
    }
}

void Environment::initGenomeReplay() {
    mode = 0;
}

void Environment::initEvolution() {
    mode = 1;
}

void Environment::initExperimentReplay() {
    mode = 2;
}

void Environment::initMultiObjectiveOptimization() {
    mode = 3;

    genomeIsSet = false;
    genomeEvaluationFinished = false;

    NSGA2Type nsga2Params;
    void *inp = this;
    void *out = NULL;

    double *minReal = (double *)malloc(2*sizeof(double));
    minReal[0] = 0;
    minReal[1] = 0;
    double *maxReal = (double *)malloc(2*sizeof(double));
    maxReal[0] = 5;
    maxReal[1] = 3;

    int *numBits = (int *)malloc(1*sizeof(int));
    double *minBin = (double *)malloc(1*sizeof(double));
    double *maxBin = (double *)malloc(1*sizeof(double));

    nsga2Params = SetParameters(0.5, 100, 150, 2, 2, 2, minReal, maxReal, 0.9, 0.5, 10, 20, 0, numBits, minBin, maxBin, 0, 0, 1, 1, 2, 0, 0, 0, 0);
    ThreadClass* t = new ThreadClass();
    t->StartNSGA2Process(nsga2Params, inp, out);
}

void Environment::setSettings(std::map<std::string, std::string> settings){
    this->settings = settings;
    reset();
}

std::map<std::string, std::string> Environment::getSettings(){
    return settings;
}

void Environment::printResults(){
    std::cout << "\nPrinting results\n";
    for(auto const& r : run_fitnesses){
        std::cout << "Run:" << std::distance(run_fitnesses.begin(),std::find(run_fitnesses.begin(),run_fitnesses.end(),r)) << "\n";
        for(auto const& g : r){
            QString s = "Generation ";
            s.append(QString::number(std::distance(r.begin(),std::find(r.begin(),r.end(),g))));
            s.append(":");

            float avg = 0.0f;
            for(auto const& gen : g){
                avg += gen;
                s.append("\t").append(QString::number(gen));
            }
            s.append("\t").append("Average: ").append(QString::number(avg/std::stoi(settings["n_genomes_int"]))).append("\n");
        }
    }
}

//Idea: Only one method updateEnvironment that checks if it needs to do nothing, update one step, update the next generation or read the next genome
void Environment::updateEnvironment() {
    if(!finished){
        if(run_counter < setting_n_runs){ // runs
            if(generation_counter < setting_n_generations){ // generation
                if(genome_counter < setting_n_genomes){ // genome
                    if(steps < setting_steps_per_genome){ // steps
                        if(mode == 3){
                            std::unique_lock<std::mutex> read_lock(genomeSetMutex);
                            while(!genomeIsSet) {
                                genomeSetCondition.wait(read_lock);
                            }
                            read_lock.unlock();
                            NSGA2_testproblem();
                            std::unique_lock<std::mutex> set_lock(genomeEvalFinishedMutex);
                            genomeEvaluationFinished = true;
                            genomeEvalFinishedCondition.notify_one();
                            set_lock.unlock();

                        } else {
                            _world->Step(1.0f / 60.0f, 32, 16);
                            if(!genome_set){
                                for(auto const& a : agents){
                                    a->genome = next_genome;
                                }
                                genome_set = true;
                            }
                            for(auto const& a : agents){
                                a->update();
                                current_fitness += a->getFitness();
                            }
                            if(mode>0){
                                steps++;
                                temp_actions_per_gen.push_back(getAverageAction());
                                temp_speeds_per_gen.push_back(getAverageSpeed());
                                temp_rotations_per_gen.push_back(getAverageDeltaAngle());
                                temp_k_distance_per_gen.push_back(getAverageKDistance());
                            }
                        }
                    }else{ // steps/genome finished
                        finished_genome();
                    }
                }else{ // all genomes finished
                    finished_generation();
                }
            }else{ // all generations finished
                finished_run();
            }
        }else{ // all runs finished => evaluation
            finished_experiment();
        }
    }
}

void Environment::save_genome_stats(){
    //calculate cluster size
    calculateClusters();
    for(auto const& c : cluster) {
        int size = c.size();
        temp_cluster_size_per_gen.push_back(size);
    }

    //calculate covered distances
    for(auto const& a : agents){
        float dst = a->getDistanceToInitialPosition();
        temp_distance_per_gen.push_back(dst);
    }

    data_action.push_back(temp_actions_per_gen);
    data_speed.push_back(temp_speeds_per_gen);
    data_rotation.push_back(temp_rotations_per_gen);
    data_k_distance.push_back(temp_k_distance_per_gen);
    data_distance.push_back(temp_distance_per_gen);
    data_cluster_size.push_back(temp_cluster_size_per_gen);

    temp_actions_per_gen.clear();
    temp_speeds_per_gen.clear();
    temp_rotations_per_gen.clear();
    temp_k_distance_per_gen.clear();
    temp_distance_per_gen.clear();
    temp_cluster_size_per_gen.clear();
}

void Environment::finished_genome() {
    steps = 0;

    if(mode>0){
        genome_set = false;
        save_genome_stats();
        float current_avg_fitness = current_fitness / (float)(std::stoi(settings["n_steps_per_genome_int"]) * std::stoi(settings["n_agents_int"]));
        current_fitness = 0.0;
        genome_fitnesses.push_back(current_avg_fitness);
        std::cout << "Exp ID " << expId << ": Generation " << generation_counter << "; Genome " << genome_counter << "; Fitness " << current_avg_fitness << std::endl;
        genome_counter++;
        if(genome_counter < setting_n_genomes){
            next_genome = population_genomes[genome_counter];
        }
    }
    for(auto const& a : agents){
        a->reset_to_initial_position();
    }
    emit genomeFinished();
}

void Environment::save_generation_stats(std::vector<int> sorted_indices) {
    std::unique_lock<std::mutex> lock(Environment::cout_mutex);

    std::string fname = "gen_" + std::to_string(generation_counter);
    std::ofstream file;
    file.open(genome_folder+fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        file << genome_fitnesses[sorted_indices[i]];
        for(unsigned int k = 0; k < population_genomes[sorted_indices[i]].size(); k++)
            file << " " << population_genomes[sorted_indices[i]][k];
        file << "\n";
    }
    file.close();

    std::ofstream file_stream;

    std::string action_fname = stats_folder+"action_gen_"+std::to_string(generation_counter);
    file_stream.open(action_fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        for(auto v : data_action[sorted_indices[i]])
            file_stream << v << " ";
        file_stream << "\n";
    }
    file_stream.close();
    data_action.clear();

    std::string speed_fname = stats_folder+"speed_gen_"+std::to_string(generation_counter);
    file_stream.open(speed_fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        for(auto v : data_speed[sorted_indices[i]]){
            file_stream << v << " ";
        }
        file_stream << "\n";
    }
    file_stream.close();
    data_speed.clear();

    std::string rotation_fname = stats_folder+"rotation_gen_"+std::to_string(generation_counter);
    file_stream.open(rotation_fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        for(auto v : data_rotation[sorted_indices[i]])
            file_stream << v << " ";
        file_stream << "\n";
    }
    file_stream.close();
    data_rotation.clear();

    std::string kdist_fname = stats_folder + "k_distance_gen_" + std::to_string(generation_counter);
    file_stream.open(kdist_fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        for(auto v : data_k_distance[sorted_indices[i]]){
            file_stream << v << " ";
        }
        file_stream << "\n";
    }
    file_stream.close();
    data_k_distance.clear();

    std::string dist_fname = stats_folder + "distance_covered_gen_" + std::to_string(generation_counter);
    file_stream.open(dist_fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        for(auto v : data_distance[sorted_indices[i]])
            file_stream << v << " ";
        file_stream << "\n";
    }
    file_stream.close();
    data_distance.clear();

    std::string cluster_fname = stats_folder + "cluster_size_gen_" + std::to_string(generation_counter);
    file_stream.open(cluster_fname);
    for(unsigned int i = 0; i < sorted_indices.size(); i++){
        for(auto v : data_cluster_size[sorted_indices[i]])
            file_stream << v << " ";
        file_stream << "\n";
    }
    file_stream.close();
    data_cluster_size.clear();

    std::cout << "Experiment "<< expId << ": Finished generation " << generation_counter << std::endl;
    lock.unlock();
}

void Environment::setPopulation(std::vector<std::vector<float>> genomes){
    population_genomes.clear();
    population_genomes = genomes;
    genome_counter = 0;
    for(auto const& a : agents){
        a->genome = population_genomes[genome_counter]; // initial genome from population
    }
    next_genome = population_genomes[genome_counter+1];
}

//called after finishing a whole generation in evolution
void Environment::finished_generation() {
    if(mode>0){
        genome_counter = 0;

        std::vector<int> sorted_indices(genome_fitnesses.size());
        std::size_t n(0);
        std::generate(std::begin(sorted_indices), std::end(sorted_indices), [&]{return n++;});

        std::sort( std::begin(sorted_indices), std::end(sorted_indices), [&](int i1, int i2){return genome_fitnesses[i1]> genome_fitnesses[i2];});

        save_generation_stats(sorted_indices);

        generation_counter++;

        generation_fitnesses.push_back(genome_fitnesses);
        all_genomes.push_back(population_genomes);

        // make offsprings
        std::vector<std::vector<float>> temp_genomes;
        if(mode==1){
            for(int elitist = 0; elitist < setting_elitism; elitist++){
                temp_genomes.push_back(population_genomes[sorted_indices[elitist]]);
            }
            for(int i = 0; i < (std::stoi(settings["n_genomes_int"])-std::stoi(settings["elitism_int"])); i++){
                temp_genomes.push_back(mutateP(population_genomes[rouletteWheelSelection(genome_fitnesses)],mutation_rate));
            }
        } else {
            QString path = QString();
            path.append(QString::fromStdString(genome_folder));
            path.append(QString::fromStdString("gen_"));
            path.append(QString::fromStdString(std::to_string(generation_counter)));
            temp_genomes = Utils::readAllGenomesFrom(path);
        }

        genome_fitnesses.clear();
        setPopulation(temp_genomes);
        temp_genomes.clear();
        for(auto const& a : agents){
            a->new_random_position(mt);
            a->reset_to_initial_position();
        }
    }
    time_t end_gen_time = std::time(nullptr);
    double result= difftime(end_gen_time, start_gen_time);
    std::cout << "Generation took " << result << " seconds\n";
    start_gen_time = end_gen_time;
    emit generationFinished();
}

//called after finishing all generations of a single run
void Environment::finished_run() {
    generation_counter = 0;
    std::unique_lock<std::mutex> lock(Environment::cout_mutex);
    std::string fname = "run_" + std::to_string(run_counter);
    fname.append("_exp-id_" + std::to_string(run_counter));
    std::ofstream file;
    file.open(exp_folder+fname);
    int gen = 0;
    for(auto const& generation : generation_fitnesses){
        file << ++gen;
        for(auto const& genome : generation){
            file << " " << genome;
        }
        file << "\n";
    }
    file.close();

    std::cout << "Experiment "<< expId << ": Finished run " << run_counter << std::endl;
    lock.unlock();
    population_genomes = population_genomes_backup;

    run_fitnesses.push_back(generation_fitnesses);
    generation_fitnesses.clear();

    //TODO: Set initial population

    //setup the new folder for the next run
    run_counter++;
    if(run_counter < setting_n_runs) {
        setup_run_folder();
    }
    emit runFinished();
}

//called after finishing all runs of an experiment
void Environment::finished_experiment(){
    finished = true;
    run_counter = 0;
}

float Environment::getAverageSpeed(){

    float avg_speed = 0.0;
    for(auto a : agents){
        avg_speed+= a->getSpeed();
    }
    return avg_speed / (float)agents.size();
}

float Environment::getAverageAction(){

    float avg_action = 0.0;
    for(auto a : agents){
        avg_action+= (a->getAction());
    }

    return avg_action / (float)agents.size();
}

float Environment::getAverageDeltaAngle(){

    float avg_angle = 0.0;
    for(auto a : agents){
        avg_angle+= a->getDeltaAngle();
    }

    return avg_angle / (float)agents.size();
}

float Environment::getAverageKDistance() {
    float avg_k_dist = 0;
    int k = 4;
    std::vector<float> distances;
    for(auto a : agents){
        distances.clear();
        for(auto b : agents){
            float dist = a->distanceTo(b->getBody()->GetPosition());
            distances.push_back(dist);
        }
        std::sort(distances.begin(), distances.end());
        avg_k_dist += distances[k];
    }
    return avg_k_dist / (float)agents.size();
}

void Environment::setGenomeForAllAgents(std::vector<float> genome){
    //reset();
    population_genomes.clear();
    population_genomes.push_back(genome);
    for(auto const& a : agents){
        a->genome = genome;
    }
}

void Environment::savePoseAndSpeed(){

    std::string fname = "trajectory_" + std::to_string(generation_counter);
    std::ofstream file;
    file.open(trajectory_folder+fname);
    for(auto &a : agents){
        file << (int)a->getBody()->GetPosition().x << " " << (int)a->getBody()->GetPosition().y << " " << a->getAngle() << " " << a->getSpeed() << "\n";
    }
    file.close();
}

void Environment::saveCoveredDistance(){

    std::string fname = "distance_" + std::to_string(run_counter);
    std::ofstream file;
    file.open(trajectory_folder+fname);
    for(auto &a : agents){
        QVectorIterator<QPoint> i(a->trace);
        while(i.hasNext()){
            QPoint p = i.next();
            file << (QString::number(p.x())).toStdString() << " " << (QString::number(p.y())).toStdString() << "\n";
        }
    }
    file.close();
}

void Environment::saveActions(){

    int sum_rotate = 0;
    int sum_move = 0;
    for(auto &a : agents){
        if(a->getSpeed() == 0){
            sum_rotate++;
        }else{
            sum_move++;
        }
    }

    std::vector<float> temp;
    temp.push_back((float)sum_rotate/agents.size());
    temp.push_back((float)sum_move/agents.size());

    log_actions.push_back(temp);
}

void Environment::calculateClusters(){
    cluster.clear();
    for(auto &a : agents){

        std::set<int> temp_cluster;
        temp_cluster.insert(a->id);
        std::map<int, Agent*> sensedAgents = a->getSensedEntities();
        for(auto sA : sensedAgents)
            temp_cluster.insert(sA.second->id);
        cluster.insert(temp_cluster);
    }
    bool merged = true;

    while(merged){
        merged = false;
        for(int i = 0; i < cluster.size()-1; i++){
            for(int j = i+1; j < cluster.size(); j++){
                std::set<int> c1 = *std::next(cluster.begin(), i);
                std::set<int> c2 = *std::next(cluster.begin(), j);

                std::set<int> temp_intersection;
                std::set_intersection(c1.begin(), c1.end(), c2.begin(), c2.end(), std::inserter(temp_intersection, temp_intersection.begin()));

                if(!temp_intersection.empty()){
                    std::set<int> temp_union;
                    std::set_union(c1.begin(), c1.end(), c2.begin(), c2.end(), std::inserter(temp_union, temp_union.begin()));
                    cluster.erase(c1);
                    cluster.erase(c2);
                    cluster.insert(temp_union);
                    merged = true;
                }
            }
        }
    }
}

void Environment::mousePressEvent(QMouseEvent *e){
    b2Vec2 clickedPos = b2Vec2(e->x()-10, e->y()-10);
    for(auto const& a : agents){
        b2Vec2 pos = a->getBody()->GetPosition();
        float32 dist = b2Distance(pos, clickedPos);
        if (dist < 20.0f){
            a->selected = !a->selected;
        }
    }
}

b2World* Environment::getWorld(){
    return _world;
}

std::vector<Agent *> &Environment::getAgents(){
    return agents;
}

void Environment::reset(){
    std::cout << "Resetting experiment " << expId << "." << std::endl;
    for (auto const& a : agents){
        delete a;
    }
    agents.clear();
    delete _world;
    setupExperiment(settings);
}

std::vector<float> Environment::mutate(std::vector<float> genome, float f){
    std::vector<float> r;
    std::uniform_real_distribution<float> uniform_rand(-0.05,0.05);
    for(unsigned int i = 0; i < genome.size(); i++){
        r.push_back(genome[i] + ((1.0f - (f / 4.0)) * uniform_rand(mt)));
    }
    return r;
}

std::vector<float> Environment::mutateP(std::vector<float> genome, float f){
    std::vector<float> r;
    std::uniform_real_distribution<float> uniform_p(0.0,1.0);
    std::normal_distribution<float> normal(0.0,1.0);
    for(unsigned int i = 0; i < action_layer_start; i++){
        float p = uniform_p(mt);
        r.push_back(genome[i] + (f > p ? normal(mt) : 0.0));
    }

    for(unsigned int i = action_layer_start; i < genome.size(); i++){
        float p = uniform_p(mt);
        r.push_back(genome[i] + (f > p ? normal(mt) : 0.0));
    }
    return r;
}

int Environment::rouletteWheelSelection(std::vector<float> fitnesses){

    std::vector<float> temp_fitnesses = fitnesses;
    float sum_fitnesses = 0.0;

    auto min_f = std::min_element(temp_fitnesses.begin(), temp_fitnesses.end());

    for(int i = 0; i <temp_fitnesses.size(); i++){
        temp_fitnesses[i] -= *min_f;
        sum_fitnesses += temp_fitnesses[i];
    }

    std::uniform_real_distribution<float> rnd(0.0,1.0);
    float rnd_value = rnd(mt) * sum_fitnesses;

    for(unsigned int i = 0; i < temp_fitnesses.size(); i++){
        rnd_value -= temp_fitnesses[i];
        if(rnd_value <= 0.0) return i;
    }

    return temp_fitnesses.size() - 1;
}

void Environment::NSGA2_testproblem() {
    //std::cout << "Test" << std::endl;
    obj_NSGA2[0] = 4.0*(xreal_NSGA2[0]*xreal_NSGA2[0] + xreal_NSGA2[1]*xreal_NSGA2[1]);
    obj_NSGA2[1] = pow((xreal_NSGA2[0]-5.0),2.0) + pow((xreal_NSGA2[1]-5.0),2.0);
    constr_NSGA2[0] = 1.0 - (pow((xreal_NSGA2[0]-5.0),2.0) + xreal_NSGA2[1]*xreal_NSGA2[1])/25.0;
    constr_NSGA2[1] = (pow((xreal_NSGA2[0]-8.0),2.0) + pow((xreal_NSGA2[1]+3.0),2.0))/7.7 - 1.0;
    //genomeEvaluationFinished = true;
}

void Environment::internal_setNSGA2Genome(double *xreal, double *xbin, int **gene, double *obj, double *constr, void *inp, void *out) {
    //std::cout << "Setting internally" << std::endl;
    std::unique_lock<std::mutex> set_lock(genomeSetMutex);
    xreal_NSGA2 = xreal;
    xbin_NSGA2 = xbin;
    gene_NSGA2 = gene;
    obj_NSGA2 = obj;
    constr_NSGA2 = constr;
    genomeIsSet = true;
    genomeEvaluationFinished = false;
    genomeSetCondition.notify_one();
    set_lock.unlock();


    //NSGA2_testproblem();

    std::unique_lock<std::mutex> read_lock(genomeEvalFinishedMutex);
    while(!genomeEvaluationFinished) {
        genomeEvalFinishedCondition.wait(read_lock);
    }
    read_lock.unlock();
    return;
}

void Environment::setNSGA2Genome(double *xreal, double *xbin, int **gene, double *obj, double *constr, void* inp, void* out) {
    //std::cout << "Settting NSGA" << std::endl;
    Environment *env = static_cast<Environment*>(inp);
    env->internal_setNSGA2Genome(xreal, xbin, gene, obj, constr, inp, out);
}
