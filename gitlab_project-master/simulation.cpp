#include "simulation.h"
#include <fstream>
#include <sstream>

// Constructor to initialize the city grid
City::City(int width, int height) : width(width), height(height) {
    grid.resize(height, std::vector<Cell>(width, {'-', 0, 0}));
}

// Function to read the configuration file
void City::readConfiguration(const std::string& configFileName, std::string& regionLayoutFileName, int& maxTimeSteps, int& refreshRate) {
    std::ifstream configFile(configFileName);
    if (!configFile) {
        std::cerr << "Could not open the configuration file." << std::endl;
        return;
    }

    configFile >> regionLayoutFileName >> maxTimeSteps >> refreshRate;
    configFile.close();
}

// Function to read the region layout file
void City::readRegionLayout(const std::string& regionLayoutFileName) {
    std::ifstream regionFile(regionLayoutFileName);
    if (!regionFile) {
        std::cerr << "Could not open the region layout file." << std::endl;
        return;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            regionFile >> grid[y][x].type;
            grid[y][x].population = 0; // Initialize population
            grid[y][x].pollution = 0; // Initialize pollution
        }
    }

    regionFile.close();
}

// Function to display the region state
void City::displayRegion() {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            std::cout << cell.type;
            if (cell.population > 0) std::cout << cell.population;  // Show population if non-zero
            std::cout << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Function to simulate the city development
void City::simulate(int steps, int refreshRate) {
    std::cout << "Initial Region State (Time Step 0):" << std::endl;
    displayRegion();
    
    for (int step = 1; step <= steps; ++step) {
        simulateStep();

        if (step % refreshRate == 0) {
            std::cout << "Region State (Time Step " << step << "):" << std::endl;
            displayRegion();
        }
    }
}

// Function to perform a single simulation step
void City::simulateStep() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Cell& cell = grid[y][x];
            int adjacentPopulation = countAdjacentPopulation(x, y);

            if (cell.type == 'R') {
                simulateResidentialZone(cell, adjacentPopulation);
            } else if (cell.type == 'C') {
                simulateCommercialZone(cell, adjacentPopulation);
            } else if (cell.type == 'I') {
                simulateIndustrialZone(cell, adjacentPopulation);
            }
        }
    }
}

// Function to count adjacent population for a given cell
int City::countAdjacentPopulation(int x, int y) {
    int population = 0;
    if (x > 0 && grid[y][x - 1].type != '-') population += grid[y][x - 1].population; // West
    if (x < width - 1 && grid[y][x + 1].type != '-') population += grid[y][x + 1].population; // East
    if (y > 0 && grid[y - 1][x].type != '-') population += grid[y - 1][x].population; // North
    if (y < height - 1 && grid[y + 1][x].type != '-') population += grid[y + 1][x].population; // South
    return population;
}

// Define zone simulation functions
void City::simulateResidentialZone(Cell& zone, int adjacentPopulation) {
    if (zone.population == 0) {
        if (adjacentPopulation >= 1)
            zone.population++;
    } else if (zone.population <= 4) {
        int requiredAdjacentPopulation = zone.population * 2;
        if (adjacentPopulation >= requiredAdjacentPopulation)
            zone.population++;
    }
}

void City::simulateCommercialZone(Cell& zone, int adjacentPopulation) {
    if (zone.population < 5 && adjacentPopulation > 3) {
        zone.population++;
    }
}

void City::simulateIndustrialZone(Cell& zone, int adjacentPopulation) {
    if (zone.population < 10 && adjacentPopulation > 5) {
        zone.population++;
    }
}

