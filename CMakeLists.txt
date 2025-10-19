cmake_minimum_required(VERSION 3.14)
project(StablecoinStabilityTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
add_library(core
    src/core/config.cpp
    src/core/thread_pool.cpp
    src/core/config_manager.cpp
    include/core/config.hpp
    include/core/thread_pool.hpp
    include/core/types.hpp
    include/core/config_manager.hpp
)
target_include_directories(core PUBLIC include)

# Data Ingestion module
add_library(data_ingestion
    src/data_ingestion/api_client.cpp
    src/data_ingestion/websocket_handler.cpp
    src/data_ingestion/data_normalizer.cpp
    src/data_ingestion/coingecko_client.cpp
    src/data_ingestion/etherscan_client.cpp
    src/data_ingestion/defi_client.cpp
    src/data_ingestion/sentiment_client.cpp
    include/data/api_client.hpp
    include/data/websocket_handler.hpp
    include/data/data_normalizer.hpp
    include/data/coingecko_client.hpp
    include/data/etherscan_client.hpp
    include/data/defi_client.hpp
    include/data/sentiment_client.hpp
)
target_include_directories(data_ingestion PUBLIC include)
target_link_libraries(data_ingestion core curl)

# Analytics module
add_library(analytics
    src/analytics/historical_pattern_engine.cpp
    src/analytics/onchain_flow_analyzer.cpp
    src/analytics/pattern_engine.cpp
    include/analytics/historical_pattern_engine.hpp
    include/analytics/onchain_flow_analyzer.hpp
    include/analytics/pattern_engine.h
)
target_include_directories(analytics PUBLIC include)

target_link_libraries(analytics core)

# Prediction Engine module
add_library(prediction_engine
    src/prediction_engine/model_interface.cpp
    src/prediction_engine/risk_scorer.cpp
    include/prediction_engine/model_interface.hpp
    include/prediction_engine/risk_scorer.hpp
)
target_include_directories(prediction_engine PUBLIC include)

target_link_libraries(prediction_engine core)

# Alerting module
add_library(alerting
    src/alerting/threshold_watcher.cpp
    src/alerting/notification_dispatcher.cpp
    include/alerting/threshold_watcher.hpp
    include/alerting/notification_dispatcher.hpp
)
target_include_directories(alerting PUBLIC include)

target_link_libraries(alerting core)

# Utils
add_library(utils
    src/utils/json_parser.cpp
    src/utils/logger.cpp
    src/utils/time_series.cpp
    src/utils/perf_logger.cpp
    include/utils/json_parser.hpp
    include/utils/logger.hpp
    include/utils/time_series.hpp
    include/utils/perf_logger.hpp
)
target_include_directories(utils PUBLIC include)
target_link_libraries(utils core)

# Monitoring library
add_library(monitoring
    src/monitoring/alert_router.cpp
    include/monitoring/alert_router.h
)
target_include_directories(monitoring PUBLIC include)
target_link_libraries(monitoring core)

# Main executable
add_executable(StablecoinStabilityTracker src/main.cpp)
target_link_libraries(StablecoinStabilityTracker
    data_ingestion analytics prediction_engine alerting utils monitoring
)

# Tests
enable_testing()
add_executable(test_data_normalizer StablecoinStabilityTracker/tests/test_data_normalizer.cpp)
target_link_libraries(test_data_normalizer data_ingestion)
add_test(NAME TestDataNormalizer COMMAND test_data_normalizer)

add_executable(test_pattern_matcher StablecoinStabilityTracker/tests/test_main.cpp)
target_link_libraries(test_pattern_matcher analytics)
add_test(NAME TestPatternMatcher COMMAND test_pattern_matcher)

add_executable(test_risk_scorer tests/test_risk_scorer.cpp)
target_link_libraries(test_risk_scorer prediction_engine)

add_executable(test_pattern_engine tests/test_pattern_engine.cpp)
target_link_libraries(test_pattern_engine analytics)

add_executable(test_summary_parser tests/test_summary_parser.cpp)
target_link_libraries(test_summary_parser)

include_directories(include include/third_party)
