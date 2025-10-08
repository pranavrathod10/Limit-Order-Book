#pragma once
#include <map>
#include <list>
#include <unordered_map>
#include <vector>
#include <mutex>

#include "Order.h"
#include "Trade.h"

/*
  OrderBook - single-symbol limit order book (single-threaded core)

  Design highlights:
  - Per-price level storage: std::map<price, std::list<Order>>
    * buyBook: std::greater<double> -> highest bid first
    * sellBook: default (std::less) -> lowest ask first
  - Use std::list for orders at a price level to get:
    * O(1) pop_front (FIFO behavior)
    * stable iterators so we can cancel orders in O(1)
  - indexMap: unordered_map<orderId -> (side, price, iterator)> for O(1) cancel
  - matchOrder logic implements price-time priority and partial fills
  - tradeHistory: vector<Trade> to record executed trades
*/

class OrderBook {
public:
    OrderBook() = default;
    ~OrderBook() = default;

    // Public API (single-threaded)
    // Add an order (limit or market — market represented with price <= 0)
    void addOrder(const Order &order);

    // Cancel an order by ID. Returns true if removed from book (not matched).
    bool cancelOrder(int orderId);

    // Snapshot printers (const — do not modify state)
    void printOrderBook() const;
    void printTradeHistory() const;

private:
    // types for convenience
    using OrderList = std::list<Order>;

    struct OrderLocation {
        Side side;
        double price;
        OrderList::iterator it;
    };

    // Books: price -> list of orders at that price
    std::map<double, OrderList, std::greater<double>> buyBook; // highest -> lowest
    std::map<double, OrderList> sellBook;                      // lowest -> highest

    // index: orderId -> location in book (for O(1) cancel)
    std::unordered_map<int, OrderLocation> indexMap;

    // trade history
    std::vector<Trade> tradeHistory;

    // Internal helpers (operate on copies or called with proper locking in multithreaded commit)
    void matchOrder(Order incoming);        // consumes as much as possible and records trades
    void insertResidual(Order &incoming);   // inserts leftover limit order into appropriate book
};
