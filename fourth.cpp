#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

struct Item {
    int value, weight;
};

struct Individual {
    vector<int> chromosome;
    int fitness;
};

int N, W;
vector<Item> items;
const int POP_SIZE = 100;
const int GENERATIONS = 1000;
const double MUTATION_RATE = 0.1;

random_device rd;
mt19937 gen(rd());

int fitness(const vector<int>& chromosome) {
    int value = 0, weight = 0;
    for (int i = 0; i < N; ++i) {
        if (chromosome[i]) {
            value += items[i].value;
            weight += items[i].weight;
        }
    }
    return (weight <= W) ? value : 0;
}

vector<int> random_chromosome() {
    vector<int> chromosome(N);
    for (int i = 0; i < N; ++i)
        chromosome[i] = gen() % 2;
    return chromosome;
}

vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2) {
    int point = gen() % N;
    vector<int> child(N);
    for (int i = 0; i < N; ++i)
        child[i] = (i < point) ? parent1[i] : parent2[i];
    return child;
}

void mutate(vector<int>& chromosome) {
    for (int i = 0; i < N; ++i) {
        if ((gen() % 100) / 100.0 < MUTATION_RATE)
            chromosome[i] = 1 - chromosome[i];
    }
}

int main() {
    cin >> N >> W;
    items.resize(N);
    for (int i = 0; i < N; ++i)
        cin >> items[i].value >> items[i].weight;
    
    auto start_time = high_resolution_clock::now();
    
    vector<Individual> population(POP_SIZE);
    for (auto &ind : population) {
        ind.chromosome = random_chromosome();
        ind.fitness = fitness(ind.chromosome);
    }
    
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        sort(population.rbegin(), population.rend(), [](const Individual& a, const Individual& b) {
            return a.fitness < b.fitness;
        });
        
        vector<Individual> new_population;
        for (int i = 0; i < POP_SIZE / 2; ++i) {
            auto child_chromosome = crossover(population[i].chromosome, population[i + 1].chromosome);
            mutate(child_chromosome);
            new_population.push_back({child_chromosome, fitness(child_chromosome)});
        }
        population.insert(population.end(), new_population.begin(), new_population.end());
        sort(population.rbegin(), population.rend(), [](const Individual& a, const Individual& b) {
            return a.fitness < b.fitness;
        });
        population.resize(POP_SIZE);
    }
    
    auto end_time = high_resolution_clock::now();
    duration<double> elapsed = end_time - start_time;
    
    cout << "Count: " << population.front().fitness << endl;
    cout << "time: " << elapsed.count() << endl;
    
    return 0;
}
