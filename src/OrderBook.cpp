#include "OrderBook.h"
#include <iostream>
#include <algorithm>

/**
 * @brief Constructor — starts the background worker thread.
 */
OrderBook::OrderBook() {
    worker = std::thread(&OrderBook::processOrders, this);
}

/**
 * @brief Destructor — gracefully shuts down worker thread.
 */
OrderBook::~OrderBook() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopProcessing = true;
    }
    cv.notify_all();          // Wake the thread if it’s waiting
    if (worker.joinable())    // Wait for it to finish
        worker.join();
}

/**
 * @brief Submits an order asynchronously.
 * Adds the order to the queue and notifies the background thread.
 */
void OrderBook::submitOrder(const Order& order) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        orderQueue.push_back(order);
    }
    cv.notify_one(); // Wake worker to process this order
}

/**
 * @brief Worker thread function that continuously processes queued orders.
 *
 * Runs in a loop, waiting for new orders.
 * Exits gracefully when `stopProcessing` is true AND queue is empty.
 */
void OrderBook::processOrders() {
    while (true) {
        Order order;

        {
            std::unique_lock<std::mutex> lock(mtx);

            // Wait until new order arrives OR shutdown is requested
            cv.wait(lock, [&]() { return !orderQueue.empty() || stopProcessing; });

            // Stop condition — no more orders and shutdown flag set
            if (stopProcessing && orderQueue.empty())
                break;

            // Retrieve next order
            order = orderQueue.front();
            orderQueue.pop_front();
        }

        // Process outside the lock to minimize contention
        addOrder(order);
    }
}

/**
 * @brief Synchronously add an order to the book and try to match it.
 *
 * - For BUY orders: match against best (lowest) asks.
 * - For SELL orders: match against best (highest) bids.
 */
void OrderBook::addOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(mtx);  // Prevent concurrent book modification

    if (order.side == Side::BUY) {
        // Try to match with best ask (lowest sell price)
        while (!asks.empty() && order.price >= asks.begin()->first && order.qty > 0) {
            auto& bestAskQueue = asks.begin()->second;
            Order& bestAsk = bestAskQueue.front();

            int tradedQty = std::min(order.qty, bestAsk.qty);
            tradeHistory.emplace_back(order.id, bestAsk.id, bestAsk.price, tradedQty);

            // Update remaining quantities
            bestAsk.qty -= tradedQty;
            const_cast<Order&>(order).qty -= tradedQty;

            // Remove exhausted orders
            if (bestAsk.qty == 0)
                bestAskQueue.pop_front();
            if (bestAskQueue.empty())
                asks.erase(asks.begin());
        }

        // If still quantity left unmatched, add to bid book
        if (order.qty > 0)
            bids[order.price].push_back(order);
    }
    else { // SELL order
        while (!bids.empty() && order.price <= bids.begin()->first && order.qty > 0) {
            auto& bestBidQueue = bids.begin()->second;
            Order& bestBid = bestBidQueue.front();

            int tradedQty = std::min(order.qty, bestBid.qty);
            tradeHistory.emplace_back(bestBid.id, order.id, bestBid.price, tradedQty);

            bestBid.qty -= tradedQty;
            const_cast<Order&>(order).qty -= tradedQty;

            if (bestBid.qty == 0)
                bestBidQueue.pop_front();
            if (bestBidQueue.empty())
                bids.erase(bids.begin());
        }

        if (order.qty > 0)
            asks[order.price].push_back(order);
    }
}

/**
 * @brief Print all executed trades so far.
 */
void OrderBook::printTradeHistory() const {
    std::cout << "\n--- Trade History ---\n";
    if (tradeHistory.empty()) {
        std::cout << "(no trades executed)\n";
        return;
    }

    for (const auto& t : tradeHistory)
        std::cout << "Buy " << t.buyId << " - Sell " << t.sellId
                  << " @ " << t.price << " x " << t.qty << "\n";
}

/**
 * @brief Print current order book snapshot.
 */
void OrderBook::printOrderBook() const {
    std::cout << "\n--- Order Book Snapshot ---\n";

    std::cout << "Bids (BUY):\n";
    for (const auto& [price, queue] : bids)
        std::cout << "  " << price << " => " << queue.size() << " orders\n";

    std::cout << "Asks (SELL):\n";
    for (const auto& [price, queue] : asks)
        std::cout << "  " << price << " => " << queue.size() << " orders\n";
}
