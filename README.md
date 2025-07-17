## Introduction

**StablecoinStabilityTracker** is a high-performance C++ application designed to monitor major stablecoins (USDT, DAI, USDC), analyze multiple risk signals (volatility, on-chain metrics, sentiment, TVL), and detect potential depeg events in real-time. The system fetches data from REST and WebSocket APIs, applies historical pattern matching, computes a composite risk score, and issues alerts when predefined thresholds are crossed.

## Features

* **Real-time data ingestion** from multiple sources (CoinGecko, Etherscan, custom sentiment API)
* **Asynchronous fetchers** for low-latency performance
* **Historical pattern matching** against known depeg events
* **Composite risk scoring** using weighted signals
* **Configurable thresholds** and confidence levels
* **Structured logging** and persistent output to JSON, CSV, and text files

## Prerequisites

* C++17 compatible compiler (e.g., GCC 9+, Clang 10+)
* CMake 3.15 or higher
* Internet access for API calls
* Optional: Redis or Kafka for future scaling (not required out-of-the-box)

## Installation

1. **Create build directory**

   ```bash
   mkdir build && cd build
   ```
2. **Configure and build**

   ```bash
   cmake ..
   make -j$(nproc)
   ```
3. **Or**
    ```bash
    ./build/StablecoinStabilityTracker
    ```

## Configuration

Configuration files are located in the `config/` directory:

* **config.json**: API keys, endpoints, and global settings
* **setting.cfg**: Threshold values for risk alerts, confidence levels, and logging preferences

Example `config.json`:

```json
{
  "coingecko_api": "https://api.coingecko.com/api/v3",
  "etherscan_api_key": "YOUR_KEY",
  "sentiment_endpoint": "https://api.sentiment.io/v1/score",
  "analysis_window_days": 30
}
```

## Project Structure

```
StablecoinStabilityTracker/
├── build/                    # Compiled binaries
├── config/
│   ├── config.json
│   └── setting.cfg
├── data/
│   ├── history/              # Historical JSON data per coin
│   └── final_report.txt      # Summary report
├── include/                  # Header files
│   ├── alerting/
│   ├── analytics/
│   ├── core/
│   ├── data/
│   ├── monitoring/
│   ├── prediction_engine/
│   ├── third_party/
│   └── utils/
├── logs/                     # Application logs
│   └── alerts.log
├── output/                   # Generated alerts and performance CSV
│   ├── alerts.json
│   └── performance.csv
├── src/                      # Source code
│   ├── alerting/
│   ├── analytics/
│   ├── core/
│   ├── data_ingestion/
│   ├── monitoring/
│   ├── prediction_engine/
│   ├── utils/
│   └── main.cpp
├── tests/                    # Unit tests
├── CMakeLists.txt
└── README.md                 # This documentation
```

## Module Descriptions

### Data Ingestion

* **api\_client.hpp/cpp**: Base class for REST clients
* **coingecko\_client**: Fetches historical and current prices
* **defi\_client** (etherscan\_client, sentiment\_client, websocket\_handler): Fetches on-chain metrics, sentiment scores, and live ticks
* **data\_normalizer**: Standardizes data formats across sources

### Analytics

* **pattern\_engine**: Implements historical depeg pattern matching
* **pattern\_matcher**: Searches for event signatures in time series data

### Prediction Engine

* **risk\_scorer**: Computes weighted risk score from volatility, on-chain, sentiment, TVL
* **model\_interface**: Defines the scoring algorithm

### Alerting

* **threshold\_watcher**: Compares risk score against configured thresholds
* **notification\_dispatcher**: Logs or sends alerts when a depeg risk is detected

### Core & Utilities

* **config\_manager**: Loads and validates configuration files
* **thread\_pool**: Manages asynchronous tasks
* **logger & perf\_logger**: Structured and performance logging
* **time\_series**: Data structures for time-indexed price points

### Monitoring

* **alert\_router**: Routes alerts to different outputs (JSON, logs)

## Running the Project

From the `build/` directory, run:

```bash
./StablecoinStabilityTracker
```

You should see output similar to:

```
=== Stablecoin Stability Tracker (Configurable) ===
[INFO] -------- Analyzing USDT --------
[INFO] ...
[FINAL REPORT] Written to ./data/final_report.txt
```

## Logging & Output

* **logs/alerts.log**: Rolling log of alerts and analysis steps
* **output/alerts.json**: JSON-formatted alerts with timestamp and risk details
* **output/performance.csv**: Aggregated metrics for backtesting and performance analysis
* **data/final\_report.txt**: Human-readable summary of the latest analysis run

## Testing

Unit tests are implemented using GoogleTest. To run tests:

```bash
cd build
ctest --output-on-failure
```

## Future Improvements

* Integrate Redis or Kafka for scalable ingestion
* Add real-time dashboard with Grafana
* Support additional stablecoins and on-chain signals
* Implement email/Slack notifications
* Enhance error handling and retry logic
