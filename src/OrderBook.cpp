#include "OrderBook.h"
#include <iostream>
#include <algorithm>

OrderBook::OrderBook() : running(true) {}

OrderBook::~OrderBook() {
    stopProcessing();
}

// Add order to queue
void OrderBook::addOrder(const Order &order) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        orderQueue.push(order);
    }
    cv.notify_one();
}

// Stop consumer thread
void OrderBook::stopProcessing() {
    running = false;
    cv.notify_all();
}

// Consumer thread loop
void OrderBook::processOrders() {
    while (running) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !orderQueue.empty() || !running; });

        if (!running) break;
        Order order = orderQueue.front();
        orderQueue.pop();
        lock.unlock();

        // Matching logic for BUY orders
        if (order.side == Side::BUY) {
            while (!asks.empty() && order.price >= asks.begin()->first && order.quantity > 0) {
                auto &queue = asks.begin()->second;
                Order &bestAsk = queue.front();
                int tradedQty = std::min(order.quantity, bestAsk.quantity);

                trades.emplace_back(order.id, bestAsk.id, bestAsk.price, tradedQty);

                order.quantity -= tradedQty;
                bestAsk.quantity -= tradedQty;

                if (bestAsk.quantity == 0) {
                    queue.pop_front();
                    if (queue.empty()) asks.erase(asks.begin());
                }
            }
            if (order.quantity > 0) {
                bids[order.price].push_back(order);
            }
        }
        // Matching logic for SELL orders
        else {
            while (!bids.empty() && order.price <= bids.begin()->first && order.quantity > 0) {
                auto &queue = bids.begin()->second;
                Order &bestBid = queue.front();
                int tradedQty = std::min(order.quantity, bestBid.quantity);

                trades.emplace_back(bestBid.id, order.id, bestBid.price, tradedQty);

                order.quantity -= tradedQty;
                bestBid.quantity -= tradedQty;

                if (bestBid.quantity == 0) {
                    queue.pop_front();
                    if (queue.empty()) bids.erase(bids.begin());
                }
            }
            if (order.quantity > 0) {
                asks[order.price].push_back(order);
            }
        }
    }
}

// Print trades
void OrderBook::printTradeHistory() const {
    std::cout << "\n--- Trade History ---\n";
    for (const auto &t : trades) {
        t.print();
    }
}

// Print remaining order book
void OrderBook::printOrderBook() const {
    std::cout << "\n--- Order Book ---\n";
    std::cout << "Bids:\n";
    for (auto &level : bids) {
        for (auto &o : level.second) o.print();
    }
    std::cout << "Asks:\n";
    for (auto &level : asks) {
        for (auto &o : level.second) o.print();
    }
    std::cout << "------------------\n";
}
