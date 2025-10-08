#include <iostream>
#include "OrderBook.h"

int main() {
    OrderBook ob;

    // Basic sequence: buyer then seller (full fill)
    ob.addOrder(Order(1, Side::BUY, 100.0, 10));
    ob.addOrder(Order(2, Side::SELL, 99.0, 10));

    // Partial fill scenario
    ob.addOrder(Order(3, Side::BUY, 101.0, 5));
    ob.addOrder(Order(4, Side::SELL, 101.0, 3)); // matches 3 -> leftover BUY qty 2 at 101

    // Market order example (price=0 => market)
    ob.addOrder(Order(5, Side::SELL, 0.0, 4)); // will consume best bids until qty filled or book empty

    ob.printTradeHistory();
    ob.printOrderBook();

    return 0;
}
