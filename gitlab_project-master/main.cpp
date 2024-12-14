#include "simulation.h"
#include <iostream>
#include <string>

int main() {
    // Configuration variables
    std::string configFileName = "config1.txt";
    std::string regionLayoutFileName;
    int maxTimeSteps = 0;
    int refreshRate = 0;

    // Create a City object with specific dimensions
    int cityWidth = 10; // Adjust as needed
    int cityHeight = 10; // Adjust as needed
    City city(cityWidth, cityHeight);

    // Read the configuration file
    city.readConfiguration(configFileName, regionLayoutFileName, maxTimeSteps, refreshRate);

    // Read the region layout file
    city.readRegionLayout(regionLayoutFileName);

    // Simulate city development
    city.simulate(maxTimeSteps, refreshRate);

    return 0;
}
