#ifndef TRADE_H
#define TRADE_H

#include <iostream>
#include <string>
#include "Order.h"

/**
 * Trade represents a matched transaction between a buy and sell order.
 */
struct Trade {
    int buyOrderId;
    int sellOrderId;
    double price;
    int quantity;
    long long timestamp;

    Trade(int b, int s, double p, int q)
        : buyOrderId(b), sellOrderId(s), price(p), quantity(q)
    {
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();
    }

    void print() const {
        std::cout << "Trade[BuyID=" << buyOrderId
                  << ", SellID=" << sellOrderId
                  << ", Price=" << price
                  << ", Qty=" << quantity
                  << ", Time=" << timestamp
                  << "]" << std::endl;
    }
};

#endif
