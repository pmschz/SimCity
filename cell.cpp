#include "cell.h"  
  
Cell::Cell(ZoneType type) : type(type), population(0), powered(false), isAlienInvadedFlag(false) {}  
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
  
bool Cell::isAlienInvaded() const {  
   return isAlienInvadedFlag;  
}  
  
void Cell::setAlienInvaded(bool invaded) {  
   isAlienInvadedFlag = invaded;  
}
