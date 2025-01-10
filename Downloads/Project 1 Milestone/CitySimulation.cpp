#include "CitySimulation.h" 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

CitySimulation::CitySimulation(int height, int width)
    : height(height), width(width) {
    region.resize(height);
    for (int i = 0; i < height; ++i) {
        region[i].resize(width);
    }
}

void CitySimulation::loadRegionFromFile(const string& filename) {
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        int row = 0;
        while (getline(file, line) && row < height) {
            cout << "Processing row " << row << endl;
            for (int col = 0; col < width && col < line.size(); col++) {
                Cell cell;
                cell.type = line[col];
                cell.population = 0;
                cell.pollution = 0;
                region[row][col] = cell;
                cout << "Processed cell at row " << row << ", col " << col 
                     << ": " << cell.type << endl;
            }
            row++;
        }
    } else {
        cout << "Error opening file: " << filename << endl;
    }
}

void CitySimulation::printRegion() const {
    cout << "Region Layout:" << endl;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            cout << region[row][col].type << ' ';
        }
        cout << endl;
    }
}
