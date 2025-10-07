#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>
#include <iostream>

/**
 * Enum class for order side.
 * enum class ensures type-safety — can't mix BUY/SELL with int accidentally.
 */
enum class Side {
    BUY,
    SELL
};

/**
 * Order represents one limit or market order in the system.
 */
struct Order {
    int id;                 // unique ID for the order
    Side side;              // BUY or SELL
    double price;           // price (0 for market orders)
    int quantity;           // quantity to trade
    long long timestamp;    // time when order is created (microseconds)

    // Constructor
    Order(int id_, Side side_, double price_, int quantity_)
        : id(id_), side(side_), price(price_), quantity(quantity_)
    {
        // auto-generate timestamp using system clock
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();
    }

    // Helper to print an order
    void print() const {
        std::string s = (side == Side::BUY) ? "BUY" : "SELL";
        std::cout << "Order[ID=" << id
                  << ", Side=" << s
                  << ", Price=" << price
                  << ", Qty=" << quantity
                  << ", Time=" << timestamp
                  << "]" << std::endl;
    }
};

#endif
