// main.cpp
/*#include <iostream>
#include <string>
#include "CitySimulation.h"

int main() {
    std::string configFile;
    std::cout << "Enter simulation configuration file: ";
    std::getline(std::cin, configFile);

    CitySimulation sim;
    if (!sim.loadConfiguration(configFile)) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }

    sim.runSimulation();
    sim.printFinalStats();
    sim.analyzeArea();

    return 0;
}

// Cell.h
#ifndef CELL_H
#define CELL_H

enum class ZoneType {
    RESIDENTIAL,
    INDUSTRIAL,
    COMMERCIAL,
    ROAD,
    POWERLINE,
    POWERLINE_OVER_ROAD,
    POWER_PLANT,
    EMPTY
};

class Cell {
public:
    Cell(ZoneType type = ZoneType::EMPTY);
    
    ZoneType getType() const;
    int getPopulation() const;
    void setPopulation(int pop);
    bool canGrow(int adjacentPop, int requiredPop, int requiredAdjacent) const;
    bool isAdjacent(const Cell& other) const;
    bool hasPower() const;
    void setPowered(bool hasPower);

private:
    ZoneType type;
    int population;
    bool powered;
};

#endif

// Cell.cpp
#include "Cell.h"

Cell::Cell(ZoneType type) : type(type), population(0), powered(false) {}

ZoneType Cell::getType() const {
    return type;
}

int Cell::getPopulation() const {
    return population;
}

void Cell::setPopulation(int pop) {
    population = pop;
}

bool Cell::canGrow(int adjacentPop, int requiredPop, int requiredAdjacent) const {
    return population == requiredPop && adjacentPop >= requiredAdjacent;
}

bool Cell::isAdjacent(const Cell& other) const {
    return other.getPopulation() > 0;
}

bool Cell::hasPower() const {
    return powered;
}

void Cell::setPowered(bool hasPower) {
    powered = hasPower;
}

// CitySimulation.h
#ifndef CITY_SIMULATION_H
#define CITY_SIMULATION_H

#include <vector>
#include <string>
#include "Cell.h"

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

    bool loadRegionLayout(const std::string& filename);
    void printRegionState();
    void simulateTimeStep();
    void updatePollution();
    bool hasChanged(const std::vector<std::vector<Cell>>& previousState);
    void calculateResources();
    void growCells();
    bool isValidCoordinate(int x, int y) const;
    int getAdjacentPopulation(int x, int y, int minPop) const;
};

#endif

// CitySimulation.cpp
// CitySimulation.cpp
#include "CitySimulation.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

bool CitySimulation::loadConfiguration(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    std::string layoutFile;
    std::getline(file, layoutFile);
    file >> maxTimeSteps >> refreshRate;

    return loadRegionLayout(layoutFile);
}

bool CitySimulation::loadRegionLayout(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    std::string line;
    while (std::getline(file, line)) {
        std::vector<Cell> row;
        std::stringstream ss(line);
        std::string cell;
        
        while (std::getline(ss, cell, ',')) {
            if (cell.empty()) continue;
            
            ZoneType type;
            switch (cell[0]) {
                case 'R': type = ZoneType::RESIDENTIAL; break;
                case 'I': type = ZoneType::INDUSTRIAL; break;
                case 'C': type = ZoneType::COMMERCIAL; break;
                case '-': type = ZoneType::ROAD; break;
                case 'T': type = ZoneType::POWERLINE; break;
                case '#': type = ZoneType::POWERLINE_OVER_ROAD; break;
                case 'P': type = ZoneType::POWER_PLANT; break;
                default: type = ZoneType::EMPTY;
            }
            row.push_back(Cell(type));
        }
        if (!row.empty()) {
            region.push_back(row);
            pollution.push_back(std::vector<int>(row.size(), 0));
        }
    }

    // Initialize available resources
    availableWorkers = 0;
    availableGoods = 0;
    return true;
}

void CitySimulation::printRegionState() {
    for (size_t i = 0; i < region.size(); ++i) {
        for (size_t j = 0; j < region[i].size(); ++j) {
            const Cell& cell = region[i][j];
            if (cell.getPopulation() == 0) {
                switch (cell.getType()) {
                    case ZoneType::RESIDENTIAL: std::cout << "R"; break;
                    case ZoneType::INDUSTRIAL: std::cout << "I"; break;
                    case ZoneType::COMMERCIAL: std::cout << "C"; break;
                    case ZoneType::ROAD: std::cout << "-"; break;
                    case ZoneType::POWERLINE: std::cout << "T"; break;
                    case ZoneType::POWERLINE_OVER_ROAD: std::cout << "#"; break;
                    case ZoneType::POWER_PLANT: std::cout << "P"; break;
                    default: std::cout << " ";
                }
            } else {
                std::cout << cell.getPopulation();
            }
            if (j < region[i].size() - 1) std::cout << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void CitySimulation::runSimulation() {
    printRegionState();  // Initial state (time step 0)
    
    std::vector<std::vector<Cell>> previousState;
    for (currentTimeStep = 1; currentTimeStep <= maxTimeSteps; ++currentTimeStep) {
        previousState = region;
        simulateTimeStep();
        
        if (currentTimeStep % refreshRate == 0) {
            std::cout << "Time step: " << currentTimeStep << std::endl;
            std::cout << "Available workers: " << availableWorkers << std::endl;
            std::cout << "Available goods: " << availableGoods << std::endl;
            printRegionState();
        }
        
        if (!hasChanged(previousState)) break;
    }
}

void CitySimulation::calculateResources() {
    availableWorkers = 0;
    availableGoods = 0;
    
    // Count total residential population as available workers
    for (const auto& row : region) {
        for (const Cell& cell : row) {
            if (cell.getType() == ZoneType::RESIDENTIAL) {
                availableWorkers += cell.getPopulation();
            }
        }
    }
    
    // Count total industrial population for goods production
    for (const auto& row : region) {
        for (const Cell& cell : row) {
            if (cell.getType() == ZoneType::INDUSTRIAL) {
                availableGoods += cell.getPopulation();
            }
        }
    }
}

bool CitySimulation::hasChanged(const std::vector<std::vector<Cell>>& previousState) {
    if (region.size() != previousState.size()) return true;
    
    for (size_t i = 0; i < region.size(); ++i) {
        if (region[i].size() != previousState[i].size()) return true;
        for (size_t j = 0; j < region[i].size(); ++j) {
            if (region[i][j].getPopulation() != previousState[i][j].getPopulation()) {
                return true;
            }
        }
    }
    return false;
}

bool CitySimulation::isValidCoordinate(int x, int y) const {
    return y >= 0 && y < static_cast<int>(region.size()) &&
           x >= 0 && x < static_cast<int>(region[y].size());
}

int CitySimulation::getAdjacentPopulation(int x, int y, int minPop) const {
    int total = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            
            int newX = x + dx;
            int newY = y + dy;
            
            if (isValidCoordinate(newX, newY)) {
                const Cell& neighbor = region[newY][newX];
                if (neighbor.getPopulation() >= minPop) {
                    total++;
                }
            }
        }
    }
    return total;
}

void CitySimulation::updatePollution() {
    std::vector<std::vector<int>> newPollution(region.size(), 
        std::vector<int>(region[0].size(), 0));
    
    // Calculate base pollution from industrial zones
    for (size_t y = 0; y < region.size(); ++y) {
        for (size_t x = 0; x < region[y].size(); ++x) {
            if (region[y][x].getType() == ZoneType::INDUSTRIAL) {
                int pollutionLevel = region[y][x].getPopulation();
                // Spread pollution to adjacent cells with decreasing intensity
                for (int dy = -3; dy <= 3; ++dy) {
                    for (int dx = -3; dx <= 3; ++dx) {
                        int newY = y + dy;
                        int newX = x + dx;
                        if (isValidCoordinate(newX, newY)) {
                            int distance = std::max(std::abs(dx), std::abs(dy));
                            int spreadPollution = std::max(0, pollutionLevel - distance);
                            newPollution[newY][newX] += spreadPollution;
                        }
                    }
                }
            }
        }
    }
    
    pollution = newPollution;
}

void CitySimulation::growCells() {
    std::vector<std::pair<int, int>> commercialCells;
    std::vector<std::pair<int, int>> industrialCells;
    std::vector<std::pair<int, int>> residentialCells;
    
    // Identify cells that can potentially grow
    for (int y = 0; y < static_cast<int>(region.size()); ++y) {
        for (int x = 0; x < static_cast<int>(region[y].size()); ++x) {
            Cell& cell = region[y][x];
            switch (cell.getType()) {
                case ZoneType::COMMERCIAL: commercialCells.push_back({x, y}); break;
                case ZoneType::INDUSTRIAL: industrialCells.push_back({x, y}); break;
                case ZoneType::RESIDENTIAL: residentialCells.push_back({x, y}); break;
                default: break;
            }
        }
    }
    
    // Sort cells based on priority rules
    auto sortCells = [this](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        Cell& cellA = region[a.second][a.first];
        Cell& cellB = region[b.second][b.first];
        
        if (cellA.getPopulation() != cellB.getPopulation())
            return cellA.getPopulation() > cellB.getPopulation();
            
        int popA = getAdjacentPopulation(a.first, a.second, 1);
        int popB = getAdjacentPopulation(b.first, b.second, 1);
        if (popA != popB)
            return popA > popB;
            
        if (a.second != b.second)
            return a.second < b.second;
            
        return a.first < b.first;
    };
    
    std::sort(commercialCells.begin(), commercialCells.end(), sortCells);
    std::sort(industrialCells.begin(), industrialCells.end(), sortCells);
    std::sort(residentialCells.begin(), residentialCells.end(), sortCells);

    // Process commercial zones first
    for (const auto& [x, y] : commercialCells) {
        Cell& cell = region[y][x];
        int pop = cell.getPopulation();
        int adjPop = getAdjacentPopulation(x, y, pop);
        
        bool hasPowerline = false;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (isValidCoordinate(x + dx, y + dy)) {
                    Cell& neighbor = region[y + dy][x + dx];
                    if (neighbor.getType() == ZoneType::POWERLINE ||
                        neighbor.getType() == ZoneType::POWERLINE_OVER_ROAD ||
                        neighbor.getType() == ZoneType::POWER_PLANT) {
                        hasPowerline = true;
                        break;
                    }
                }
            }
            if (hasPowerline) break;
        }

        // Commercial growth conditions
        if ((pop == 0 && (hasPowerline || adjPop >= 1) && availableWorkers >= 1 && availableGoods >= 1) ||
            (pop == 1 && adjPop >= 2 && availableWorkers >= 1 && availableGoods >= 1)) {
            cell.setPopulation(pop + 1);
            availableWorkers--;
            availableGoods--;
        }
    }

    // Process industrial zones second
    for (const auto& [x, y] : industrialCells) {
        Cell& cell = region[y][x];
        int pop = cell.getPopulation();
        int adjPop = getAdjacentPopulation(x, y, pop);
        
        bool hasPowerline = false;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (isValidCoordinate(x + dx, y + dy)) {
                    Cell& neighbor = region[y + dy][x + dx];
                    if (neighbor.getType() == ZoneType::POWERLINE ||
                        neighbor.getType() == ZoneType::POWERLINE_OVER_ROAD ||
                        neighbor.getType() == ZoneType::POWER_PLANT) {
                        hasPowerline = true;
                        break;
                    }
                }
            }
            if (hasPowerline) break;
        }

        // Industrial growth conditions
        if ((pop == 0 && (hasPowerline || adjPop >= 1) && availableWorkers >= 2) ||
            (pop == 1 && adjPop >= 2 && availableWorkers >= 2) ||
            (pop == 2 && adjPop >= 4 && availableWorkers >= 2)) {
            cell.setPopulation(pop + 1);
            availableWorkers -= 2;
        }
    }

    // Process residential zones last
    for (const auto& [x, y] : residentialCells) {
        Cell& cell = region[y][x];
        int pop = cell.getPopulation();
        int adjPop = getAdjacentPopulation(x, y, pop);
        
        bool hasPowerline = false;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (isValidCoordinate(x + dx, y + dy)) {
                    Cell& neighbor = region[y + dy][x + dx];
                    if (neighbor.getType() == ZoneType::POWERLINE ||
                        neighbor.getType() == ZoneType::POWERLINE_OVER_ROAD ||
                        neighbor.getType() == ZoneType::POWER_PLANT) {
                        hasPowerline = true;
                        break;
                    }
                }
            }
            if (hasPowerline) break;
        }

        // Residential growth conditions
        if ((pop == 0 && (hasPowerline || adjPop >= 1)) ||
            (pop == 1 && adjPop >= 2) ||
            (pop == 2 && adjPop >= 4) ||
            (pop == 3 && adjPop >= 6) ||
            (pop == 4 && adjPop >= 8)) {
            cell.setPopulation(pop + 1);
        }
    }
}

void CitySimulation::printFinalStats() {
    int totalResidential = 0;
    int totalIndustrial = 0;
    int totalCommercial = 0;
    int totalPollution = 0;
    
    for (size_t y = 0; y < region.size(); ++y) {
        for (size_t x = 0; x < region[y].size(); ++x) {
            const Cell& cell = region[y][x];
            switch (cell.getType()) {
                case ZoneType::RESIDENTIAL:
                    totalResidential += cell.getPopulation();
                    break;
                case ZoneType::INDUSTRIAL:
                    totalIndustrial += cell.getPopulation();
                    break;
                case ZoneType::COMMERCIAL:
                    totalCommercial += cell.getPopulation();
                    break;
                default:
                    break;
            }
            totalPollution += pollution[y][x];
        }
    }
    
    std::cout << "\nFinal Statistics:" << std::endl;
    std::cout << "Total Residential Population: " << totalResidential << std::endl;
    std::cout << "Total Industrial Population: " << totalIndustrial << std::endl;
    std::cout << "Total Commercial Population: " << totalCommercial << std::endl;
    std::cout << "Total Regional Pollution: " << totalPollution << std::endl;
    
// Continuing from where we left off in CitySimulation::printFinalStats()
    std::cout << "\nPollution Map:" << std::endl;
    for (const auto& row : pollution) {
        for (size_t x = 0; x < row.size(); ++x) {
            std::cout << row[x];
            if (x < row.size() - 1) std::cout << ",";
        }
        std::cout << std::endl;
    }
}

void CitySimulation::analyzeArea() {
    int x1, y1, x2, y2;
    do {
        std::cout << "\nEnter coordinates for area analysis (x1 y1 x2 y2): ";
        std::cin >> x1 >> y1 >> x2 >> y2;
        
        // Ensure x1,y1 is top-left and x2,y2 is bottom-right
        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);
        
        if (!isValidCoordinate(x1, y1) || !isValidCoordinate(x2, y2)) {
            std::cout << "Invalid coordinates. Please try again." << std::endl;
            std::cout << "Valid range is (0,0) to (" 
                      << region[0].size() - 1 << "," 
                      << region.size() - 1 << ")" << std::endl;
        }
    } while (!isValidCoordinate(x1, y1) || !isValidCoordinate(x2, y2));

    int areaResidential = 0;
    int areaIndustrial = 0;
    int areaCommercial = 0;
    int areaPollution = 0;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            const Cell& cell = region[y][x];
            switch (cell.getType()) {
                case ZoneType::RESIDENTIAL:
                    areaResidential += cell.getPopulation();
                    break;
                case ZoneType::INDUSTRIAL:
                    areaIndustrial += cell.getPopulation();
                    break;
                case ZoneType::COMMERCIAL:
                    areaCommercial += cell.getPopulation();
                    break;
                default:
                    break;
            }
            areaPollution += pollution[y][x];
        }
    }

    std::cout << "\nArea Analysis Results:" << std::endl;
    std::cout << "Residential Population: " << areaResidential << std::endl;
    std::cout << "Industrial Population: " << areaIndustrial << std::endl;
    std::cout << "Commercial Population: " << areaCommercial << std::endl;
    std::cout << "Total Pollution: " << areaPollution << std::endl;
}

*/