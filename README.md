# Limit Order Book Engine (C++)

A multithreaded trading engine simulation built from scratch.

## Commit 1 – Repo Skeleton

**What this commit does:**

- Adds the base types `Order` and `Trade`.
- Demonstrates object creation and printing.
- Confirms compiler setup.

**To build and run:**

```bash
g++ -std=c++17 src/main.cpp -Iinclude -o lob_smoke
./lob_smoke
```

## Commit 2 – OrderBook Interface

**Highlights:**

- Added `OrderBook` class to manage BUY/SELL orders.
- Introduced thread-safe maps for order storage.
- Demonstrated adding and printing orders.
