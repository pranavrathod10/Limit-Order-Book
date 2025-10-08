#ifndef TRADE_H
#define TRADE_H

#include <iostream>

/**
 * Trade represents a matched transaction between a buy and sell order.
 */
struct Trade {
    int buyId;
    int sellId;
    double price;
    int quantity;
    long long timestamp;

    Trade(int b, int s, double p, int q)
        : buyId(b), sellId(s), price(p), quantity(q) 
    {
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();
    }

    void print() const {
        std::cout << "Trade[BuyID=" << buyId
                  << ", SellID=" << sellId
                  << ", Price=" << price
                  << ", Qty=" << quantity
                  << ", Time=" << timestamp
                  << "]" << std::endl;
    }
};

#endif
