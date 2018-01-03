//
// Created by triki on 12/19/17.
//

#ifndef UNTITLED_ACTION_H
#define UNTITLED_ACTION_H
#include <string>

using namespace std;


class Action {

public:

    float tv;
    float bidPrice;
    float askPrice;
    int bidSize;
    int askSize;
    string symbol;
    string date;
    float theoBand;

    bool isBuyOrSell = false;
    bool isBuy = false;

    Action(string date, string symbol, string bidPrice, string askPrice, string bidSize, string askSize, string tv, float theoBand);


private:
    bool CheckIfBuyOrSell();

};


#endif //UNTITLED_ACTION_H
