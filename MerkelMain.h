#pragma once
#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"

//OrderBook orderBook{"20200317.csv"};
extern OrderBook orderBook;
extern std::string currentTime;
extern std::string previousTime;

class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();

    private: 
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        int getUserOption();
        void processUserOption(int userOption);
        void getCandleStick();
        void getVolumeGraph();
        Wallet wallet;
};
