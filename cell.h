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
   Cell(ZoneType type);  
   ZoneType getType() const;  
   int getPopulation() const;  
   void setPopulation(int pop);  
   bool canGrow(int adjacentPop, int requiredPop, int requiredAdjacent) const;  
   bool isAdjacent(const Cell& other) const;  
   bool hasPower() const;  
   void setPowered(bool hasPower);  
   bool isAlienInvaded() const; // new method  
   void setAlienInvaded(bool invaded); // new method  
  
private:  
   ZoneType type;  
   int population;  
   bool powered;  
   bool isAlienInvadedFlag; // new member variable  
};

#endif