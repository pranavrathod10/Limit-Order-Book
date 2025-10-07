#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include "Trade.h"
#include <map>
#include <vector>
#include <mutex>
#include <iostream>

/**
 * OrderBook class manages all BUY and SELL orders.
 * - BUY orders are stored in descending price order (higher price = better).
 * - SELL orders are stored in ascending price order (lower price = better).
 * 
 * We'll later add thread safety and order matching.
 */
class OrderBook {
private:
    // Maps: price -> list of orders
    std::map<double, std::vector<Order>, std::greater<double>> buyOrders;
    std::map<double, std::vector<Order>> sellOrders;

    // Mutex for thread safety (used later)
    mutable std::mutex bookMutex;

    int nextOrderId = 1;

public:
    // Add a new order to the book
    void addOrder(Side side, double price, int quantity);

    // Print summary of both sides
    void printOrderBook() const;

    // Clear all orders
    void clearBook();

    // Generate next unique order ID
    int getNextOrderId() { return nextOrderId++; }
};

inline void OrderBook::addOrder(Side side, double price, int quantity) {
    std::lock_guard<std::mutex> lock(bookMutex);
    int id = getNextOrderId();
    Order order(id, side, price, quantity);

    if (side == Side::BUY) {
        buyOrders[price].push_back(order);
    } else {
        sellOrders[price].push_back(order);
    }

    std::cout << "Added ";
    order.print();
}

inline void OrderBook::printOrderBook() const {
    std::lock_guard<std::mutex> lock(bookMutex);

    std::cout << "\n--- ORDER BOOK ---\n";
    std::cout << "BUY ORDERS:\n";
    for (const auto& [price, orders] : buyOrders) {
        std::cout << "Price " << price << " -> ";
        for (const auto& o : orders)
            std::cout << "[" << o.quantity << "] ";
        std::cout << "\n";
    }

    std::cout << "SELL ORDERS:\n";
    for (const auto& [price, orders] : sellOrders) {
        std::cout << "Price " << price << " -> ";
        for (const auto& o : orders)
            std::cout << "[" << o.quantity << "] ";
        std::cout << "\n";
    }
    std::cout << "------------------\n";
}

inline void OrderBook::clearBook() {
    std::lock_guard<std::mutex> lock(bookMutex);
    buyOrders.clear();
    sellOrders.clear();
}


#endif
