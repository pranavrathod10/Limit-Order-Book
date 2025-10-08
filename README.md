# Limit Order Book Simulation (C++)

A **multithreaded Limit Order Book** simulation in C++.  
This project mimics a **trading engine** where multiple clients submit buy and sell orders, and orders are matched based on price and time priority (FIFO).

---

## Overview

A Limit Order Book (LOB) is a core component of a trading system. It keeps track of:

- **Buy Orders (Bids):** Orders to buy a certain quantity at a maximum price.
- **Sell Orders (Asks):** Orders to sell a certain quantity at a minimum price.

**Matching rules:**

1. Orders are matched by **price** first, then by **time** (FIFO).
2. Market orders (price = 0) match against the best available orders immediately.
3. Remaining orders that are not fully matched stay in the book.

**Why multithreading?**

- In real trading systems, multiple clients submit orders simultaneously.
- Threads simulate **concurrent clients**, while **mutex locks** ensure the order book remains consistent.

---

## Building the Project

Make sure you have a C++17 compatible compiler (like `g++` or `clang++`).

```bash
# Navigate to project root
cd Limit-Order-Book

# Compile all source files
g++ -std=c++17 src/*.cpp -Iinclude -o lob_sim

# Run the simulation
./lob_sim
```

Example Output
--- TRADE HISTORY ---
Trade[BuyID=1, SellID=2, Price=101, Qty=5, Time=1697012345678]
Trade[BuyID=3, SellID=4, Price=102, Qty=3, Time=1697012345890]

--- ORDER BOOK ---
Bids (BUY):
BUY 2 @ 101 (id=3)

Asks (SELL):
SELL 4 @ 102 (id=4)

---
