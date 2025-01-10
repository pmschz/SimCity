#ifndef CITYSIMULATION_H
#define CITYSIMULATION_H
#include <vector>
#include <string>
using namespace std;

struct Cell {
    char type;
    int population;
    int pollution;
};

class CitySimulation {
public:
    CitySimulation(int height, int width);
    void loadRegionFromFile(const string& filename);
    void printRegion() const;

private:
    vector<vector<Cell> > region;
    int height;
    int width;
};

#endif
