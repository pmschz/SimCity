#include "CitySimulation.h"
#include <iostream>

using namespace std;

int main() {
    int height = 5;
    int width = 5;
    CitySimulation simulation(height, width);

    string filename = "region.txt";
    simulation.loadRegionFromFile(filename);
    simulation.printRegion();

    return 0;
}
