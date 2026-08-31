## Introduction

**Stablecoin Stability Tracker** is a C++17 application that monitors major stablecoins (USDT, DAI, USDC), analyzes risk signals (peg deviation, volatility, on-chain metrics, sentiment, TVL), and flags potential depeg events. It fetches prices from CoinGecko and optional on-chain data from Etherscan, computes a composite risk score, and writes alerts and reports.

## Features

* Historical price ingestion from CoinGecko (cached, rate-limited)
* Optional Etherscan token-transfer metrics (correct per-token decimals)
* Peg deviation and log-return volatility
* Historical depeg pattern matching (cosine similarity)
* Configurable thresholds, weights, and confidence labels
* JSON, CSV, and text reports

Sentiment and DeFi TVL are **mocked** until real APIs are wired; the UI labels them as `[MOCK]`.

## Prerequisites

* C++17 compiler (GCC 9+, Clang 10+, Apple Clang)
* CMake 3.14 or higher
* libcurl
* Internet access for live API calls (tests do not require it)

## Installation

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Configuration

Copy the examples if you want a local override:

* `config/config.json` — coins, thresholds, window, risk weights (committed sample works out of the box)
* `config/settings.cfg.example` — copy to `config/settings.cfg` and add an Etherscan key, or set `ETHERSCAN_API_KEY`

Example `config.json`:

```json
{
  "stablecoins": ["USDT", "USDC", "DAI"],
  "alert_thresholds": {
    "USDT": 0.5,
    "USDC": 0.5,
    "DAI": 0.5
  },
  "historical_window_days": 7,
  "target_price": 1.0,
  "depeg_threshold": 0.02,
  "enabled_data_sources": ["price", "onchain", "sentiment", "tvl"],
  "risk_weights": {
    "peg_deviation": 0.25,
    "volatility": 0.25,
    "onchain": 0.20,
    "sentiment": 0.15,
    "tvl": 0.15
  }
}
```

## Project Structure

```
.
├── config/
│   ├── config.json
│   ├── config.json.example
│   └── settings.cfg.example
├── include/
├── src/
│   └── main.cpp
├── tests/
├── CMakeLists.txt
└── README.md
```

Runtime output (gitignored): `data/`, `logs/`, `output/`, `.cache/`.

## Running

From the repo root (after building):

```bash
./build/StablecoinStabilityTracker
./build/StablecoinStabilityTracker --test-mode
```

## Logging & Output

* **logs/alerts.log**: Alerts with the actual risk drivers
* **output/alerts.json**: JSON array of alerts
* **output/performance.csv**: Per-coin timing
* **data/final_report.txt**: Human-readable summary
* **data/history/**: Per-coin JSON with risk components

## Testing

Tests are standalone C++ executables registered with CTest (no GoogleTest).

```bash
cd build
ctest --output-on-failure
```

## Future Improvements

* Real sentiment and TVL APIs
* Redis or Kafka for scalable ingestion
* Email/Slack notifications
* Dashboard
