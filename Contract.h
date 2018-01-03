//
// Created by triki on 12/19/17.
//

#ifndef TICKCALCULATOR_CONTRACT_H
#define TICKCALCULATOR_CONTRACT_H

#include <string>
using namespace std;

class Contract {
public:
    float marketPrice;
    int numberOfTrades;
    int plTicks;
    string time;
    string symbol;
    int numBuys;
    int numSells;

    Contract(string symbol);
};


#endif //TICKCALCULATOR_CONTRACT_H
