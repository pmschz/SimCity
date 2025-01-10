#ifndef SIMULATION_H
#define SIMULATION_H
#include <vector>
#include <string>
#include "cell.h"

class Alien {
public:
    Alien(int x, int y);
    void move();
    int getX() const;
    int getY() const;
    bool affectsZone(ZoneType zoneType) const;

private:
    int x, y;  // Position of the alien in the grid
};

class CitySimulation {
public:
    bool loadConfiguration(const std::string& filename);
    void runSimulation();
    void printFinalStats();
    void analyzeArea();

private:
    std::vector<std::vector<Cell>> region;
    std::vector<std::vector<int>> pollution;
    int maxTimeSteps;
    int refreshRate;
    int currentTimeStep;
    int availableWorkers;
    int availableGoods;

    void spawnAlien();
    void moveAlien();
    void applyAlienEffects();
    void handleAlienInvasion();  // Added missing declaration

    bool loadRegionLayout(const std::string& filename);
    void printRegionState();
    void simulateTimeStep();
    void updatePollution();
    bool hasChanged(const std::vector<std::vector<Cell>>& previousState);
    void calculateResources();
    void growCells();
    bool isValidCoordinate(int x, int y) const;
    int getAdjacentPopulation(int x, int y, int minPop) const;

    // Alien-related functions
    Alien* alien;  // Pointer to the Alien object
};

#endif
