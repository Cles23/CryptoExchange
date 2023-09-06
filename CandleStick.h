#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include <vector>

class CandleStick
{
    public:
        CandleStick(const std::string& timestamp,
                    double open,
                    double high,
                    double low,
                    double close);
        static std::string ProcessProduct(OrderBook& ob);
        static std::string ProcessType();
        static std::vector<CandleStick> createCandleSticks(OrderBook& ob);
        static void plotCandleStick(const std::vector<CandleStick>& data);
        static std::vector<std::string> buildCandlestickStringList(const CandleStick& candleStick, int space, double max, double min);

    private:
            std::string timestamp;
            double open;
            double high;
            double low;
            double close;

};


