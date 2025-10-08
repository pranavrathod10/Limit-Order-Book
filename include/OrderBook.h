#pragma once
#include <map>
#include <deque>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include "Order.h"
#include "Trade.h"

class OrderBook {
private:
    std::map<double, std::deque<Order>, std::greater<double>> bids; // BUY orders
    std::map<double, std::deque<Order>> asks;                        // SELL orders

    std::queue<Order> orderQueue; // Orders waiting to be processed
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> running;

    std::vector<Trade> trades; // Executed trades

public:
    OrderBook();
    ~OrderBook();

    void addOrder(const Order &order);     // Thread-safe producer
    void processOrders();                  // Thread-safe consumer
    void stopProcessing();                 // Stop the consumer thread safely

    void printTradeHistory() const;
    void printOrderBook() const;
};
