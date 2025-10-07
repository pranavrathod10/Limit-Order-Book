#include "../include/OrderBook.h"
#include <iostream>

int main() {
    std::cout << "=== Commit 2: OrderBook Interface Demo ===" << std::endl;

    OrderBook book;

    // Add some buy and sell orders
    book.addOrder(Side::BUY, 101.0, 10);
    book.addOrder(Side::BUY, 100.5, 5);
    book.addOrder(Side::SELL, 102.0, 7);
    book.addOrder(Side::SELL, 103.0, 3);

    // Print summary
    book.printOrderBook();

    return 0;
}
