#include "../include/Order.h"
#include "../include/Trade.h"
#include <iostream>

int main() {
    std::cout << "=== Simple Limit Order Book Skeleton ===" << std::endl;

    // Create a few orders manually
    Order o1(1, Side::BUY, 100.5, 10);
    Order o2(2, Side::SELL, 101.0, 5);

    // Print them
    o1.print();
    o2.print();

    // Create a trade between them
    Trade t1(o1.id, o2.id, 100.75, 5);
    t1.print();

    std::cout << "=== End of test ===" << std::endl;
    return 0;
}
