#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include <vector>

class VolumeGraph
{
public:
    VolumeGraph(const std::string& timestamp,
                double volume);
    static std::string ProcessProduct(OrderBook& ob);
    static std::string ProcessType();
    static std::vector<VolumeGraph> createVolumeGraph(OrderBook& ob);
    static void plotVolumeGraph(const std::vector<VolumeGraph>& data);
    static std::vector<std::string> buildVolumeGraphStringList(const VolumeGraph& volumeGraph, int space, double max, double min);

private:
    std::string timestamp;
    double volume;
};


