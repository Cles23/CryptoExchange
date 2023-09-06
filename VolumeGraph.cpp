#include "VolumeGraph.h"
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
VolumeGraph::VolumeGraph(const std::string& timestamp,
                         double volume)


        :   timestamp(timestamp),
            volume(volume)
{
}

/*Get the user to input a product and type as well as handle invalid inputs*/
std::string VolumeGraph::ProcessProduct(OrderBook& ob)
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
std::string VolumeGraph::ProcessType()
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


// Implement function for getting Volume Data
std::vector<VolumeGraph> VolumeGraph::createVolumeGraph(OrderBook& ob)
{

    OrderBookType type = OrderBookEntry::stringToOrderBookType(VolumeGraph::ProcessType());
    std::string product = VolumeGraph::ProcessProduct(ob);
    std::vector<std::string> timestamp = {currentTime};

    std::vector<VolumeGraph> vg_Vector;
    std::vector<OrderBookEntry> prevOrders = ob.getOrders(type, product, previousTime);

    // Get volume for each timeframe
    for (int i = 0; i < 5; i++)
    {
        std::vector<OrderBookEntry> curOrders = ob.getOrders(type, product, timestamp[i]);

        // Calculate the total volume for the current timeframe
        double totalVolume = 0.0;
        for (const auto& order : curOrders)
        {
            if (type == OrderBookType::ask)
            {
                totalVolume += order.amount;
            }
            else if (type == OrderBookType::bid)
            {
                totalVolume += order.amount * order.price;
            }
        }

        VolumeGraph vg(timestamp[i], totalVolume);
        vg_Vector.push_back(vg);

        // Now this timeframe becomes the previous timeframe for the next iteration
        prevOrders = curOrders;

        // Get the next timestamp
        if (i < 4) // We don't need the next timestamp for the last iteration
        {
            std::string nextTime = ob.getNextTime(timestamp[i]);
            timestamp.push_back(nextTime);
        }
    }
// Print the volume Data
    for (const auto& cs : vg_Vector)
    {
        std::cout << "Timestamp : " << cs.timestamp << "\n"
                  << "Volume      : " << cs.volume << "\n"
                  << "===============================" << std::endl;
    }
    return vg_Vector;
}


// Function to create each bar for the volume graph
std::vector<std::string> VolumeGraph::buildVolumeGraphStringList(const VolumeGraph& volumeGraph, int space, double max, double min) {
    double writtenSpace = (max - min) / space;

    int volumeGraphStart = space - round((max - volumeGraph.volume) / writtenSpace);
    int volumeGraphEnd = round((0 - min) / writtenSpace);

    std::vector<std::string> stringList(space, "");
    for (int i = space, j = 0; i > 0; i--, j++) {
        if (i > volumeGraphStart || i < volumeGraphEnd) {
            stringList[j] = "     ";
        } else {
            if (i == volumeGraphStart) {
                stringList[j] = "-----";
            } else {
                stringList[j] = "|   |";
            }
        }
    }
    return stringList;
}

// Function to Plot the Graph
void VolumeGraph::plotVolumeGraph(const std::vector<VolumeGraph>& data) {
    // determine the min and max volume
    double minVolume = 0.0;
    double maxVolume = std::numeric_limits<double>::min();
    const int space = 19;
    for (const auto& volumeGraph : data) {
        if (volumeGraph.volume > maxVolume) maxVolume = volumeGraph.volume;
    }

    std::vector<std::string> finalStringList(space, "");
    std::vector<std::string> stringList(space, "");
    for (const auto& volumeGraph : data) {
        stringList = buildVolumeGraphStringList(volumeGraph, space, maxVolume, minVolume);
        for (int i = 0; i < space; i++) {
            finalStringList[i] += stringList[i] + "         ";
        }
    }
    for (int i = 0; i < space; i++) {
        double lineValue = maxVolume - ((maxVolume - minVolume) * i / (space - 1));
        // Convert the value to a string
        std::string aux = std::to_string(lineValue);
        aux = aux.substr(0, 8);
        std::cout << std::setw(aux.length()) << aux << "| " << finalStringList[i] << std::endl;

    }
    std::cout << std::string(8, ' ') << std::string(65, '-') << std::endl;
// Print the timestamps at the bottom of the graph
    int j = 8;
    for (size_t i = 0; i < data.size(); i++) {
        // Extract the HH:MM:SS part of the timestamp
        std::string timestamp = data[i].timestamp;
        std::string time = timestamp.substr(timestamp.find(" ") + 1, 8);

        // Print the time, centered under the corresponding VolumeGraph
        std::cout << std::string(j, ' ') << time;
        j = 6;
    }
    std::cout << std::endl;
}

