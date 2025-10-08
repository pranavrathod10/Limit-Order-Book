#pragma once
#include <map>
#include <deque>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "Order.h"
#include "Trade.h"

/**
 * @class OrderBook
 * @brief Thread-safe, concurrent limit order book implementation.
 *
 * Supports asynchronous order submission using a background worker thread.
 * Uses mutexes and condition variables to synchronize access between threads.
 */
class OrderBook {
private:
    // --- Core order book data structures ---
    // Each price level maps to a deque (queue) of orders at that price.
    // Bids are sorted in descending order (highest price first).
    // Asks are sorted in ascending order (lowest price first).
    std::map<double, std::deque<Order>, std::greater<double>> bids;
    std::map<double, std::deque<Order>> asks;

    // List of completed trades for recordkeeping
    std::vector<Trade> tradeHistory;

    // --- Concurrency & threading members ---
    std::deque<Order> orderQueue;           // Orders waiting to be processed
    std::mutex mtx;                         // Protects shared state
    std::condition_variable cv;             // Signals when new orders arrive
    bool stopProcessing = false;            // Used to gracefully stop the worker
    std::thread worker;                     // Dedicated background thread

    // --- Internal helper functions ---
    void processOrders();                   // Worker thread main loop
    void matchOrders();                     // (Optional future use)
    
public:
    OrderBook();
    ~OrderBook();

    /**
     * @brief Public API — submit an order asynchronously.
     * The order is queued and processed by a worker thread.
     */
    void submitOrder(const Order& order);

    /**
     * @brief Synchronously add an order (core matching logic).
     * Called internally by worker after pulling from queue.
     */
    void addOrder(const Order& order);

    // Display current book and trades
    void printOrderBook() const;
    void printTradeHistory() const;
};
