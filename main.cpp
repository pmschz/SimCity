// main.cpp
#include <iostream>
#include <string>
#include "simulation.h"
using namespace std;

int main() {
    string configFile;
    cout << "Enter simulation configuration file: ";
    getline(cin, configFile);

    CitySimulation sim;
    if (!sim.loadConfiguration(configFile)) {
        cerr << "Failed to load configuration" << endl;
        return 1;
    }

    sim.runSimulation();
    sim.printFinalStats();
    sim.analyzeArea();

    return 0;
}
