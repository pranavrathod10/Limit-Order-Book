#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "OrderBook.h"

/**
 * Simulates a client sending multiple orders
 */
void clientSimulator(OrderBook &ob, int clientId) {
    for (int i = 0; i < 5; i++) {
        Side side = (i % 2 == 0) ? Side::BUY : Side::SELL;
        double price = 100 + (rand() % 10);   // Price between 100–109
        int qty = 1 + (rand() % 5);           // Quantity 1–5

        ob.addOrder(Order(clientId * 10 + i, side, price, qty));

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
    std::cout << "Program started\n";

    OrderBook ob;

    // Start the order processing thread (consumer)
    std::thread processor(&OrderBook::processOrders, &ob);

    // Start multiple client threads (producers)
    std::vector<std::thread> clients;
    for (int i = 0; i < 3; ++i) {
        clients.emplace_back(clientSimulator, std::ref(ob), i + 1);
    }

    // Wait for all client threads to finish
    for (auto &t : clients) t.join();

    // Give some time for all orders to be processed
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Stop the processing thread safely
    ob.stopProcessing();
    processor.join();

    // Print all executed trades and remaining order book
    ob.printTradeHistory();
    ob.printOrderBook();

    return 0;
}
