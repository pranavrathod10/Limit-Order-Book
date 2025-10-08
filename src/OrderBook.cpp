#include "OrderBook.h"
#include <iostream>
#include <algorithm>
#include <chrono>

//
// Implementation notes:
//
// - matchOrder handles both BUY and SELL incoming orders.
// - If incoming.price <= 0.0 we treat it as a MARKET order: consume book liquidity until filled or book exhausted.
// - Trade price: we use the resting order's price (common, clear rule).
// - indexMap maintains iterators so cancelOrder can remove an order in O(1).
// - This file is single-threaded correct; Commit 2 will add thread-safety wrappers.
//

// Insert remaining portion of a limit order into the book (record indexMap)
void OrderBook::insertResidual(Order &incoming) {
    if (incoming.quantity <= 0) return;

    if (incoming.side == Side::BUY) {
        auto &lvl = buyBook[incoming.price];
        lvl.push_back(incoming);
        auto it = std::prev(lvl.end());
        indexMap[incoming.id] = OrderLocation{Side::BUY, incoming.price, it};
    } else {
        auto &lvl = sellBook[incoming.price];
        lvl.push_back(incoming);
        auto it = std::prev(lvl.end());
        indexMap[incoming.id] = OrderLocation{Side::SELL, incoming.price, it};
    }
}

// Core matching engine for a single incoming order (price-time priority)
void OrderBook::matchOrder(Order incoming) {
    bool isMarket = incoming.price <= 0.0;

    if (incoming.side == Side::BUY) {
        // Try to match with lowest asks first
        while (incoming.quantity > 0 && !sellBook.empty()) {
            auto bestIt = sellBook.begin(); // lowest ask
            double bestPrice = bestIt->first;

            // Stop if incoming is limit and best ask > limit price
            if (!isMarket && bestPrice > incoming.price) break;

            auto &queue = bestIt->second;
            // match against orders at this price level FIFO
            while (incoming.quantity > 0 && !queue.empty()) {
                Order &resting = queue.front();
                int traded = std::min(incoming.quantity, resting.quantity);

                // Record trade: buyId = incoming.id, sellId = resting.id
                tradeHistory.emplace_back(incoming.id, resting.id, resting.price, traded);
                // update timestamp of trade to now (Trade constructor does this)

                incoming.quantity -= traded;
                resting.quantity -= traded;

                if (resting.quantity == 0) {
                    // remove resting order and its index entry
                    indexMap.erase(resting.id);
                    queue.pop_front();
                }
            }

            // if level empty remove price level
            if (queue.empty()) sellBook.erase(bestIt);
        }

        // leftover for limit orders goes to book; market leftover is discarded
        if (incoming.quantity > 0 && !isMarket) insertResidual(incoming);
    } else { // SELL incoming
        while (incoming.quantity > 0 && !buyBook.empty()) {
            auto bestIt = buyBook.begin(); // highest bid
            double bestPrice = bestIt->first;

            if (!isMarket && bestPrice < incoming.price) break;

            auto &queue = bestIt->second;
            while (incoming.quantity > 0 && !queue.empty()) {
                Order &resting = queue.front();
                int traded = std::min(incoming.quantity, resting.quantity);

                tradeHistory.emplace_back(resting.id, incoming.id, resting.price, traded);

                incoming.quantity -= traded;
                resting.quantity -= traded;

                if (resting.quantity == 0) {
                    indexMap.erase(resting.id);
                    queue.pop_front();
                }
            }

            if (queue.empty()) buyBook.erase(bestIt);
        }

        if (incoming.quantity > 0 && !isMarket) insertResidual(incoming);
    }
}

// Public API: add order (single-threaded)
void OrderBook::addOrder(const Order &order) {
    // copy because we will modify quantity
    Order incoming = order;

    // If the order id was already in the book (duplicate), we reject by ignoring.
    if (indexMap.find(incoming.id) != indexMap.end()) {
        std::cerr << "addOrder: duplicate order id " << incoming.id << " — ignoring\n";
        return;
    }

    // Matching happens before adding to resting book
    matchOrder(incoming);
}

// Cancel an existing resting order (returns true if removed)
bool OrderBook::cancelOrder(int orderId) {
    auto it = indexMap.find(orderId);
    if (it == indexMap.end()) return false;

    OrderLocation loc = it->second;
    if (loc.side == Side::BUY) {
        auto lvlIt = buyBook.find(loc.price);
        if (lvlIt != buyBook.end()) {
            lvlIt->second.erase(loc.it);            // O(1) with list
            if (lvlIt->second.empty()) buyBook.erase(lvlIt);
        }
    } else {
        auto lvlIt = sellBook.find(loc.price);
        if (lvlIt != sellBook.end()) {
            lvlIt->second.erase(loc.it);
            if (lvlIt->second.empty()) sellBook.erase(lvlIt);
        }
    }
    indexMap.erase(it);
    return true;
}

// Print book snapshot
void OrderBook::printOrderBook() const {
    std::cout << "\n=== ORDER BOOK SNAPSHOT ===\n";

    std::cout << "ASKS (sell side, low->high):\n";
    for (const auto &lvl : sellBook) {
        int total = 0;
        for (const auto &o : lvl.second) total += o.quantity;
        std::cout << "  " << lvl.first << " : " << total << '\n';
        for (const auto &o : lvl.second) {
            std::cout << "    ";
            o.print();
        }
    }

    std::cout << "BIDS (buy side, high->low):\n";
    for (const auto &lvl : buyBook) {
        int total = 0;
        for (const auto &o : lvl.second) total += o.quantity;
        std::cout << "  " << lvl.first << " : " << total << '\n';
        for (const auto &o : lvl.second) {
            std::cout << "    ";
            o.print();
        }
    }
    std::cout << "===========================\n";
}

// Print trade history
void OrderBook::printTradeHistory() const {
    std::cout << "\n=== TRADE HISTORY (" << tradeHistory.size() << ") ===\n";
    for (const auto &t : tradeHistory) t.print();
    std::cout << "=================================\n";
}
