#include "simulation.h"  
#include <fstream>  
#include <iostream>  
#include <sstream>  
#include <algorithm>  
#include <cmath>  
using namespace std;  
  
bool CitySimulation::loadConfiguration(const string& filename) {  
   ifstream file(filename);  
   if (!file) return false;  
  
   string layoutFile;  
   getline(file, layoutFile);  
   file >> maxTimeSteps >> refreshRate;  
  
   return loadRegionLayout(layoutFile);  
}  
  
bool CitySimulation::loadRegionLayout(const string& filename) {  
   ifstream file(filename);  
   if (!file) return false;  
  
   string line;  
   while (getline(file, line)) {  
      vector<Cell> row;  
      stringstream ss(line);  
      string cell;  
       
      while (getline(ss, cell, ',')) {  
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
        pollution.push_back(vector<int>(row.size(), 0));  
      }  
   }  
  
   availableWorkers = 0;  
   availableGoods = 0;  
   return true;  
}  
  
void CitySimulation::simulateTimeStep() {  
   calculateResources();  
   updatePollution();  
   growCells();  
   handleAlienInvasion();  
}  
  
void CitySimulation::printRegionState() {  
   for (int i = 0; i < region.size(); ++i) {  
      for (int j = 0; j < region[i].size(); ++j) {  
        const Cell& cell = region[i][j];  
        if (cell.getPopulation() == 0) {  
           switch (cell.getType()) {  
              case ZoneType::RESIDENTIAL: cout << "R"; break;  
              case ZoneType::INDUSTRIAL: cout << "I"; break;  
              case ZoneType::COMMERCIAL: cout << "C"; break;  
              case ZoneType::ROAD: cout << "-"; break;  
              case ZoneType::POWERLINE: cout << "T"; break;  
              case ZoneType::POWERLINE_OVER_ROAD: cout << "#"; break;  
              case ZoneType::POWER_PLANT: cout << "P"; break;  
              default: cout << " ";  
           }  
        } else {  
           cout << cell.getPopulation();  
        }  
        if (j < region[i].size() - 1) cout << ",";  
      }  
      cout << endl;  
   }  
   cout << endl;  
}  
  
void CitySimulation::runSimulation() {  
   printRegionState();  
    
   vector<vector<Cell>> previousState;  
   for (currentTimeStep = 1; currentTimeStep <= maxTimeSteps; ++currentTimeStep) {  
      previousState = region;  
      simulateTimeStep();  
       
      if (currentTimeStep % refreshRate == 0) {  
        cout << "Time step: " << currentTimeStep << endl;  
        cout << "Available workers: " << availableWorkers << endl;  
        cout << "Available goods: " << availableGoods << endl;  
        printRegionState();  
      }  
  
      if (!hasChanged(previousState)) break;  
   }  
}  
  
void CitySimulation::calculateResources() {  
   availableWorkers = 0;  
   availableGoods = 0;  
    
   for (const auto& row : region) {  
      for (const Cell& cell : row) {  
        if (cell.getType() == ZoneType::RESIDENTIAL) {  
           availableWorkers += cell.getPopulation();  
        }  
      }  
   }  
    
   for (const auto& row : region) {  
      for (const Cell& cell : row) {  
        if (cell.getType() == ZoneType::INDUSTRIAL) {  
           availableGoods += cell.getPopulation();  
        }  
      }  
   }  
}  
  
bool CitySimulation::hasChanged(const vector<vector<Cell>>& previousState) {  
   if (region.size() != previousState.size()) return true;  
    
   for (int i = 0; i < region.size(); ++i) {  
      if (region[i].size() != previousState[i].size()) return true;  
      for (int j = 0; j < region[i].size(); ++j) {  
        if (region[i][j].getPopulation() != previousState[i][j].getPopulation()) {  
           return true;  
        }  
      }  
   }  
   return false;  
}  
  
bool CitySimulation::isValidCoordinate(int x, int y) const {  
   return y >= 0 && y < region.size() &&  
        x >= 0 && x < region[y].size();  
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
   vector<vector<int>> newPollution(region.size(),  
      vector<int>(region[0].size(), 0));  
    
   for (int y = 0; y < region.size(); ++y) {  
      for (int x = 0; x < region[y].size(); ++x) {  
        if (region[y][x].getType() == ZoneType::INDUSTRIAL) {  
           int pollutionLevel = region[y][x].getPopulation();  
           for (int dy = -3; dy <= 3; ++dy) {  
              for (int dx = -3; dx <= 3; ++dx) {  
                int newY = y + dy;  
                int newX = x + dx;  
                if (isValidCoordinate(newX, newY)) {  
                   int distance = max(abs(dx), abs(dy));  
                   int spreadPollution = max(0, pollutionLevel - distance);  
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
  
   // Sort cells based on priority rules (this code remains unchanged)  
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
  
   // Process commercial zones  
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
  
      // Modified Commercial growth conditions with reduced requirements  
      if ((pop == 0 && (hasPowerline || adjPop >= 1) && availableWorkers >= 1 && availableGoods >= 1) ||  
        (pop == 1 && adjPop >= 1 && availableWorkers >= 1 && availableGoods >= 1)) {  
        cell.setPopulation(pop + 1);  
        availableWorkers--;  
        availableGoods--;  
      }  
   }  
  
   // Process industrial zones  
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
  
      // Modified Industrial growth conditions with reduced worker requirements  
      if ((pop == 0 && (hasPowerline || adjPop >= 1) && availableWorkers >= 1) ||  
        (pop == 1 && adjPop >= 2 && availableWorkers >= 1)) {  
        cell.setPopulation(pop + 1);  
        availableWorkers--; // Decreased workers requirement per step  
      }  
   }  
  
   // Process residential zones  
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
        (pop == 2 && adjPop >= 3) ||  // Reduced adjacent population requirements for faster growth  
        (pop == 3 && adjPop >= 4)) {  
        cell.setPopulation(pop + 1);  
      }  
   }  
}  
  
void CitySimulation::handleAlienInvasion() {
    // Randomly select a cell to invade
    int x = rand() % region[0].size();
    int y = rand() % region.size();
    Cell& cell = region[y][x];

    // Check if the cell is already invaded or if it's a power plant
    if (cell.isAlienInvaded() || cell.getType() == ZoneType::POWER_PLANT) {
        return;
    }

    // Mark the cell as invaded
    cell.setAlienInvaded(true);
    std::cout << "Alien invasion occurred at (" << x << ", " << y << ")!" << std::endl;

    // Reduce the population of adjacent cells
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int newX = x + dx;
            int newY = y + dy;
            if (isValidCoordinate(newX, newY)) {
                Cell& neighbor = region[newY][newX];
                int reducedPopulation = std::max(0, neighbor.getPopulation() - 1); // Prevent negative population
                if (neighbor.getPopulation() != reducedPopulation) {
                    std::cout << "Population reduced at (" << newX << ", " << newY << ")" 
                              << " from " << neighbor.getPopulation() 
                              << " to " << reducedPopulation << std::endl;
                }
                neighbor.setPopulation(reducedPopulation);
            }
        }
    }

    // Recalculate resources post-invasion
    calculateResources();

    // Update pollution to reflect changes caused by the invasion
    updatePollution();
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
  
        // Add pollution contribution for each cell type  
        if (cell.getType() == ZoneType::INDUSTRIAL) {  
           totalPollution += pollution[y][x];  
        }  
      }  
   }  
  
   // Output the final statistics  
   cout << "Final Stats:" << endl;  
   cout << "Total Residential Population: " << totalResidential << endl;  
   cout << "Total Industrial Population: " << totalIndustrial << endl;  
   cout << "Total Commercial Population: " << totalCommercial << endl;  
   cout << "Total Pollution: " << totalPollution << endl;  
    
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
   int areaAlienInvaded = 0;  
  
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
        if (cell.isAlienInvaded()) {  
           areaAlienInvaded++;  
        }  
      }  
   }  
  
   std::cout << "\nArea Analysis Results:" << std::endl;  
   std::cout << "Residential Population: " << areaResidential << std::endl;  
   std::cout << "Industrial Population: " << areaIndustrial << std::endl;  
   std::cout << "Commercial Population: " << areaCommercial << std::endl;  
   std::cout << "Total Pollution: " << areaPollution << std::endl;  
   std::cout << "Alien Invaded Cells: " << areaAlienInvaded << std::endl;  
}
