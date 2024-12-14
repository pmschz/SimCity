#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>
#include <iostream>

struct Cell {
    char type;          // Type of the zone: Residential (R), Commercial (C), Industrial (I), or Empty (-)
    int population;     // Population in the cell
    int pollution;      // Pollution in the cell
};

class City {
public:
    City(int width, int height);
    void readConfiguration(const std::string& configFileName, std::string& regionLayoutFileName, int& maxTimeSteps, int& refreshRate);
    void readRegionLayout(const std::string& regionLayoutFileName);
    void analyzeArea(int x1, int y1, int x2, int y2);
    void simulate(int steps, int refreshRate);
    void displayRegion();

private:
    int width, height;                      // Dimensions of the city grid
    std::vector<std::vector<Cell>> grid;   // 2D grid representing the city
    void simulateStep();
    int countAdjacentPopulation(int x, int y);
    void simulateResidentialZone(Cell& zone, int adjacentPopulation);
    void simulateCommercialZone(Cell& zone, int adjacentPopulation);
    void simulateIndustrialZone(Cell& zone, int adjacentPopulation);
};

#endif
