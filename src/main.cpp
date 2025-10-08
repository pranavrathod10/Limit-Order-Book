#include <iostream>
#include <thread>
#include "OrderBook.h"

/**
 * @brief Demonstrates a concurrent order submission scenario.
 *
 * Multiple threads submit buy/sell orders at the same time.
 * The OrderBook processes them safely in the background.
 */
int main() {
    OrderBook ob;

    // Spawn multiple threads submitting orders
    std::thread t1([&]() { ob.submitOrder(Order(1, Side::BUY, 100.0, 10)); });
    std::thread t2([&]() { ob.submitOrder(Order(2, Side::SELL, 99.0, 10)); });
    std::thread t3([&]() { ob.submitOrder(Order(3, Side::BUY, 101.0, 5)); });
    std::thread t4([&]() { ob.submitOrder(Order(4, Side::SELL, 101.0, 3)); });

    // Wait for all threads to finish submitting
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // Allow worker time to process all orders
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Display results
    ob.printTradeHistory();
    ob.printOrderBook();

    return 0;
}
