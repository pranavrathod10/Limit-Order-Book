#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <chrono>
#include <iostream>

/**
 * Enum class for order side.
 * Ensures type safety between BUY and SELL.
 */
enum class Side {
    BUY,
    SELL
};

/**
 * Order represents a single buy or sell order.
 */
struct Order {
    int id;                 // Unique order ID
    Side side;              // BUY or SELL
    double price;           // Price (0 for market orders)
    int quantity;           // Quantity to trade
    long long timestamp;    // Timestamp in microseconds

    // Constructor
    Order(int id_, Side side_, double price_, int quantity_)
        : id(id_), side(side_), price(price_), quantity(quantity_) 
    {
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();
    }

    // Print order info
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
