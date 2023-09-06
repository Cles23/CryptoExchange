#include "CandleStick.h"
#include "OrderBook.h"
#include <iostream>
#include "OrderBookEntry.h"
#include "MerkelMain.h"
#include <algorithm>
#include <vector>
#include <limits>
#include <iomanip>
#include <cmath>

// Constructor
CandleStick::CandleStick(const std::string& timestamp,
                         double open,
                         double high,
                         double low,
                         double close)
:   timestamp(timestamp),
    open(open),
    high(high),
    low(low),
    close(close)
{
}

//Get the user to input a product and type as well as handle invalid inputs
std::string CandleStick::ProcessProduct(OrderBook& ob)
{
    while (true){
        std::vector<std::string> validProducts = ob.getKnownProducts();
        std::string inputProduct;
        std::cout << "Enter a product: eg ETH/BTC" << std::endl;
        std::getline(std::cin, inputProduct);

        if(std::find(validProducts.begin(), validProducts.end(), inputProduct) != validProducts.end())
        {
            // If input product is found in validProducts, return inputProduct
            return inputProduct;
        }
        else
        {
            // If input product is not found in validProducts, return an error message
            std::cout << "Invalid product! Please try again.";
        }
    }

}
std::string CandleStick::ProcessType()
{
    while (true){
        std::vector<std::string> validTypes{"ask", "bid"};
        std::string inputType;
        std::cout << "Enter a type: ask or bid" << std::endl;
        std::getline(std::cin, inputType);

        if(std::find(validTypes.begin(), validTypes.end(), inputType) != validTypes.end())
        {
            // If input type is found in validTypes, return inputType
            return inputType;
        }
        else
        {
            // If input type is not found in validTypes, return an error message
            std::cout << "Invalid type! Please try again.";
        }
    }
}


// Implement function for getting candlesticks
std::vector<CandleStick> CandleStick::createCandleSticks(OrderBook& ob)
{

    OrderBookType type = OrderBookEntry::stringToOrderBookType(CandleStick::ProcessType());
    std::string product = CandleStick::ProcessProduct(ob);
    std::vector<std::string> timestamp = {currentTime};

    std::vector<CandleStick> cs_Vector;
    std::vector<OrderBookEntry> prevOrders = ob.getOrders(type, product, previousTime);

    // Get 1 candlestick for each timeframe
    for (int i = 0; i < 5; i++)
    {
        std::vector<OrderBookEntry> curOrders = ob.getOrders(type, product, timestamp[i]);

        // Calculate the average price for current timeframe (closing price)
        double totalCloseValue = 0.0;
        double totalCloseAmount = 0.0;
        for (const auto& order : curOrders)
        {
            totalCloseValue += order.price * order.amount;
            totalCloseAmount += order.amount;
        }
        double closePrice = totalCloseValue / totalCloseAmount; // average price per unit


        // Calculate the opening price (Previous time frame)
        double openPrice;
        double totalOpenValue = 0.0;
        double totalOpenAmount = 0.0;
        if (!prevOrders.empty())
        {
            // Calculate the average price for previous timeframe (opening price)
            for (const auto& order : prevOrders)
            {
                totalOpenValue += order.price * order.amount;
                totalOpenAmount += order.amount;
            }
            openPrice = totalOpenValue / totalOpenAmount; // average price per unit
        }
        else
        {
            openPrice = curOrders[0].price; // No previous timeframe available, so opening = first price of current timeframe
        }



        // Calculate the high and low prices for the current timeframe
        double highPrice = OrderBook::getHighPrice(curOrders);
        double lowPrice = OrderBook::getLowPrice(curOrders);

        // Create a new CandleStick object and push it to the vector
        CandleStick cs(timestamp[i], openPrice, highPrice, lowPrice, closePrice);
        cs_Vector.push_back(cs);

        prevOrders = curOrders;

        // Get the next timestamp
        if (i < 4)
        {
            std::string nextTime = ob.getNextTime(timestamp[i]);
            timestamp.push_back(nextTime);
        }
    }
// Print the candlesticks Data
    for (const auto& cs : cs_Vector)
    {
        std::cout << "Timestamp : " << cs.timestamp << "\n"
                  << "Open      : " << cs.open << "\n"
                  << "High      : " << cs.high << "\n"
                  << "Low       : " << cs.low << "\n"
                  << "Close     : " << cs.close << "\n"
                  << "===============================" << std::endl;
    }
    return cs_Vector;
}


// Function to create a string representation of the candlestick
std::vector<std::string> CandleStick::buildCandlestickStringList(const CandleStick& candlestick, int space, double max, double min) {
    double writtenSpace = (max - min) / space;
    // Calculate the start and end of the candlestick in the graph
    int candlestickStart = space - round((max - candlestick.high) / writtenSpace);
    int candlestickEnd = round((candlestick.low - min) / writtenSpace);
    // Calculate the start and end of the line in the candlestick
    bool isRed = candlestick.open > candlestick.close;
    int lineStart = space - round((max - (isRed ? candlestick.close : candlestick.open)) / writtenSpace);
    int lineEnd = round(((isRed ? candlestick.open : candlestick.close) - min) / writtenSpace);

    std::vector<std::string> stringList(space, "");
    for (int i = space, j = 0; i > 0; i--, j++) {
        if (i > candlestickStart || i < candlestickEnd) {
            stringList[j] = "     ";
        } else {
            // open and close
            if (i == lineStart || i == lineEnd) {
                stringList[j] = "-----";
            // body of the candlestick
            } else if (i < lineStart && i > lineEnd) {
                stringList[j] = "|   |";
            } else if (i > lineStart && i < lineEnd) {
                stringList[j] = "|   |";
            // wick of the candlestick
            } else if (i > lineStart || i < lineEnd) {
                stringList[j] = "  |  ";
            }
        }
    }
    return stringList;
}

// Function to Plot the Graph of the Candlesticks
void CandleStick::plotCandleStick(const std::vector<CandleStick>& data) {
    // Determine the size of the graph
    double minPrice = std::numeric_limits<double>::max();
    double maxPrice = std::numeric_limits<double>::min();
    const int space = 19;
    for (const auto& candlestick : data) {
        if (candlestick.low < minPrice) minPrice = candlestick.low;
        if (candlestick.high > maxPrice) maxPrice = candlestick.high;
    }

    // Build the string list for each candlestick
    std::vector<std::string> finalStringList(space, "");
    std::vector<std::string> stringList(space, "");
    for (const auto& candlestick : data) {
        stringList = buildCandlestickStringList(candlestick, space, maxPrice, minPrice);
        for (int i = 0; i < space; i++) {
            finalStringList[i] += stringList[i] + "         ";
        }
    }
    // Print the prices on the left side of the graph, and the candlesticks
    for (int i = 0; i < space; i++) {
        double lineValue = maxPrice - ((maxPrice - minPrice) * i / (space - 1));
        // Convert the value to a string
        std::string aux = std::to_string(lineValue);
        aux = aux.substr(0, 8);
        std::cout << std::setw(aux.length()) << aux << "| " << finalStringList[i] << std::endl;

    }
    // Print the bottom line
    std::cout << std::string(8, ' ') << std::string(65, '-') << std::endl;

    // Print the timestamps at the bottom of the graph
    int j = 8;
    for (size_t i = 0; i < data.size(); i++) {
        // Extract the HH:MM:SS part of the timestamp
        std::string timestamp = data[i].timestamp;
        std::string time = timestamp.substr(timestamp.find(" ") + 1, 8);

        // Print the time, centered under the corresponding candlestick
        std::cout << std::string(j, ' ') << time;
        j = 6;
    }
    std::cout << std::endl;
}
