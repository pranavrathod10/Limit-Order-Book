#pragma once
#include <chrono>

/**
 * @struct Trade
 * @brief Represents a completed trade (a match between a buy and sell order).
 */
struct Trade {
    int buyId;              // ID of buyer's order
    int sellId;             // ID of seller's order
    double price;           // Executed trade price
    int qty;                // Quantity traded
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    Trade(int buyId_, int sellId_, double price_, int qty_)
        : buyId(buyId_), sellId(sellId_), price(price_), qty(qty_),
          timestamp(std::chrono::system_clock::now()) {}
};
