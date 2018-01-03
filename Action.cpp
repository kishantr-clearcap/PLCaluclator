//
// Created by triki on 12/19/17.
//

#include "Action.h"


Action::Action(string date, string symbol, string bidPrice, string askPrice, string bidSize, string askSize, string tv, float theoBand) {
    this->date = date;
    this->symbol = symbol;
    this->bidPrice = stof(bidPrice);
    this->askPrice = stof(askPrice);
    this->bidSize = stoi(bidSize);
    this->askSize = stoi(askSize);
    this->tv = stof(tv);
    this->theoBand = theoBand;

    if(CheckIfBuyOrSell()) {
        isBuyOrSell = true;
    }


} //Action contains an individual action event. This could be a buy or non buy. Anything that gets recorded is considered an action, so pretty much any line in the csv becomes one


bool Action::CheckIfBuyOrSell() {

    if(tv - askPrice >= theoBand) { // BUY
        isBuyOrSell = true;
        isBuy = true;
    }
    else if (bidPrice - tv >= theoBand) { //SELL
        isBuyOrSell = true;
        isBuy = false;
    }
    return isBuyOrSell;
}

