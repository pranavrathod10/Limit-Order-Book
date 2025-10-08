#pragma once
#include <chrono>
#include <string>

/**
 * @enum Side
 * @brief Defines whether an order is a BUY or SELL.
 */
enum class Side { BUY, SELL };

/**
 * @struct Order
 * @brief Represents a single order in the limit order book.
 *
 * Contains metadata like ID, side, price, quantity, and timestamp.
 */
struct Order {
    int id;                 // Unique order ID
    Side side;              // BUY or SELL
    double price;           // Limit price (0 for market order)
    int qty;                // Quantity to trade
    std::chrono::time_point<std::chrono::system_clock> timestamp;  // For time-priority matching

    // ✅ Default constructor (needed for std::deque temporary initialization)
    Order() : id(0), side(Side::BUY), price(0.0), qty(0),
              timestamp(std::chrono::system_clock::now()) {}

    // ✅ Parameterized constructor
    Order(int id_, Side side_, double price_, int qty_)
        : id(id_), side(side_), price(price_), qty(qty_),
          timestamp(std::chrono::system_clock::now()) {}
};
