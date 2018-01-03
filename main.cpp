#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Action.h"
#include "Contract.h"
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;
float theoBand;

void PopulateSymbolsToActions(ifstream &csvToParse, unordered_map<string, vector<Action>> &symbolToActionMap, unordered_map<string, vector<Action>> &symbolToBuyOrSells);
void FindMarketPrices(unordered_map<string, vector<Action>> &symbolToActionMap, vector<Contract> &contractVector);
void CalculatePLTicks(vector<Contract> &contractVector, unordered_map<string, vector<Action>> &symbolToBuyOrSells);
void Print(vector<Contract> contractVector);
int main(int argc, char *argv[]) {

    if(argc != 3) {
        cout << "Incorrect arguments. Please input csv file and TV-band." << endl;
    }

    ifstream csvToParse;
    csvToParse.open(argv[1]);

    if (!csvToParse) {
        cerr << "Unable to open file";
        exit(1);
    }
    unordered_map<string, vector<Action>> symbolToActionMap;
    unordered_map<string, vector<Action>> symbolToBuyOrSells;

    theoBand = atof(argv[2]);

    PopulateSymbolsToActions(csvToParse, symbolToActionMap, symbolToBuyOrSells);
    csvToParse.close();


    vector<Contract> contractVector;
    for ( auto it = symbolToActionMap.begin(); it != symbolToActionMap.end(); ++it ) {
        Contract contractToInsert(it->first);
        contractVector.push_back(contractToInsert);
    }

    for(int i = 0; i < contractVector.size(); i++) {
        contractVector[i].numberOfTrades = symbolToBuyOrSells[contractVector[i].symbol].size();
    }


    FindMarketPrices(symbolToActionMap, contractVector);
    CalculatePLTicks(contractVector, symbolToBuyOrSells);
    Print(contractVector);

    return 0;
}


void PopulateSymbolsToActions(ifstream &csvToParse, unordered_map<string, vector<Action>> &symbolToActionMap, unordered_map<string, vector<Action>> &symbolToBuyOrSells) {
    string x = "";
    int netPosition = 0;
    while (getline(csvToParse, x)) {
        string s = "";
        stringstream ss(x);
        vector<string> vectorOfActionArguments;
        while( ss.good() ) {
            string substr;
            getline(ss, substr, ',');
            vectorOfActionArguments.push_back(substr);
        }
        string symbol = vectorOfActionArguments[1];
        if (symbolToActionMap.find(symbol) == symbolToActionMap.end()) {
            vector<Action> actionsVector;
            symbolToActionMap.insert(std::make_pair(symbol, actionsVector));
            symbolToBuyOrSells.insert(std::make_pair(symbol, actionsVector));
        }
        else {
            Action actionToInsert(vectorOfActionArguments[0], vectorOfActionArguments[1], vectorOfActionArguments[2], vectorOfActionArguments[3], vectorOfActionArguments[4], vectorOfActionArguments[5], vectorOfActionArguments[6], theoBand);
            symbolToActionMap[symbol].push_back(actionToInsert);
            if(actionToInsert.isBuyOrSell) {
//                if(netPosition == 200 || netPosition == -200) {
//                    cout << "LIMIT" << endl;
//                }
                if(actionToInsert.symbol == "DI1N18" || actionToInsert.symbol == "DI1F18" || actionToInsert.symbol == "DI1F19" || actionToInsert.symbol == "DI1N19") {
                    //symbolToBuyOrSells[symbol].push_back(actionToInsert);
                    continue;
                }
                if(netPosition < 200 && actionToInsert.isBuy) { //If its a buy and netPos under 200
                    symbolToBuyOrSells[symbol].push_back(actionToInsert);
                    netPosition++;
                }
                else if (netPosition > -200 && !actionToInsert.isBuy) { //If its a sell and netPos above -200
                    symbolToBuyOrSells[symbol].push_back(actionToInsert);
                    netPosition--;
                }
            }
        }
    }
    cout << "EOD Net Position: " << netPosition << endl;
}

void FindMarketPrices(unordered_map<string, vector<Action>> &symbolToActionMap, vector<Contract> &contractVector) {
    for (int i = 0;  i < contractVector.size(); i++) {
        vector<Action> actionsToIterate = symbolToActionMap[contractVector[i].symbol];
        int seconds = INT_MAX;
        float markPrice;
        string time;
        for(int i = 0; i < actionsToIterate.size(); i++) {
            tm current;

            char* pch;
            char *arr = &actionsToIterate[i].date[0];
            pch = strtok(arr, " ,.-:");
            current.tm_year = atoi(pch); //get the year value
            current.tm_mon = atoi(strtok(NULL, " ,.-:"));  //get the month value
            current.tm_mday = atoi(strtok(NULL, " ,.-:")); //get the day value
            current.tm_hour = atoi(strtok(NULL, " ,.-:")); //get the hour value
            current.tm_min = atoi(strtok(NULL, " ,.-:")); //get the min value
            current.tm_sec = atoi(strtok(NULL, " ,.-:")); //get the sec value
            struct tm sixteen;
            sixteen.tm_year = current.tm_year;
            sixteen.tm_mon = current.tm_mon;
            sixteen.tm_mday = current.tm_mday;
            sixteen.tm_hour = 16;
            sixteen.tm_min = 0;
            sixteen.tm_sec = 0;

            if(current.tm_hour < 16) {
                int compseconds = difftime(mktime(&sixteen), mktime(&current));
                if(compseconds < seconds) {
                    seconds = compseconds;
                    time = actionsToIterate[i].date;
                    markPrice = actionsToIterate[i].tv;
                }

            }
        }
        contractVector[i].marketPrice = markPrice;
        contractVector[i].time = time;
    }
}

void CalculatePLTicks(vector<Contract> &contractVector, unordered_map<string, vector<Action>> &symbolToBuyOrSells) {
    for(int i = 0; i < contractVector.size(); i++) {
        vector<Action> BSActionsForSymbol = symbolToBuyOrSells[contractVector[i].symbol];
        float marketPriceForSymbol = contractVector[i].marketPrice;
        int totalPLTicksForSymbol = 0;
        int numBuys = 0;
        int numSells = 0;
        for(int j = 0; j < BSActionsForSymbol.size(); j++) {
            Action currentAction = BSActionsForSymbol[j];
            int plTicksForAction = 0;
            if(currentAction.isBuy) { //One and negative one should be the quantities eventually
                plTicksForAction = (marketPriceForSymbol - currentAction.askPrice) * (1) * 100;
                numBuys++;
            }
            else {
                plTicksForAction = (marketPriceForSymbol - currentAction.bidPrice) * (-1) * 100;
                numSells++;
            }
            totalPLTicksForSymbol += plTicksForAction;
        }
        contractVector[i].plTicks = totalPLTicksForSymbol;
        contractVector[i].numBuys = numBuys;
        contractVector[i].numSells = numSells;
    }
}

void Print(vector<Contract> contractVector) {
    cout << "Contract  " << " # of Trades " << " P/L(Ticks) " << "  Net Pos  " << endl;
    int totalPLTicks = 0;
    for(int i = 0; i < contractVector.size(); i++) {
        totalPLTicks += contractVector[i].plTicks;
        cout << contractVector[i].symbol << "         " << contractVector[i].numberOfTrades << "           " << contractVector[i].plTicks << "        " <<  contractVector[i].numBuys - contractVector[i].numSells <<  endl;
    }
    cout << "EOD PL: " << totalPLTicks << endl;
}