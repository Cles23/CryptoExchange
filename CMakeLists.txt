cmake_minimum_required(VERSION 3.25)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
        CSVReader.cpp
        CSVReader.h
        main.cpp
        MerkelMain.cpp
        MerkelMain.h
        OrderBook.cpp
        OrderBook.h
        OrderBookEntry.cpp
        OrderBookEntry.h
        Wallet.cpp
        Wallet.h
        CandleStick.cpp CandleStick.h VolumeGraph.cpp VolumeGraph.h)
