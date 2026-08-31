cmake_minimum_required(VERSION 3.14)
project(StablecoinStabilityTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

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

add_library(data_ingestion
    src/data_ingestion/api_client.cpp
    src/data_ingestion/websocket_handler.cpp
    src/data_ingestion/data_normalizer.cpp
    src/data_ingestion/coingecko_client.cpp
    src/data_ingestion/etherscan_client.cpp
    src/data_ingestion/defi_client.cpp
    src/data_ingestion/sentiment_client.cpp
)
target_include_directories(data_ingestion PUBLIC include)
target_link_libraries(data_ingestion PUBLIC core CURL::libcurl)

add_library(analytics
    src/analytics/historical_pattern_engine.cpp
    src/analytics/onchain_flow_analyzer.cpp
    src/analytics/pattern_engine.cpp
    src/analytics/pattern_matcher.cpp
)
target_include_directories(analytics PUBLIC include)
target_link_libraries(analytics PUBLIC core)

add_library(prediction_engine
    src/prediction_engine/model_interface.cpp
    src/prediction_engine/risk_scorer.cpp
)
target_include_directories(prediction_engine PUBLIC include)
target_link_libraries(prediction_engine PUBLIC core)

add_library(alerting
    src/alerting/threshold_watcher.cpp
    src/alerting/notification_dispatcher.cpp
)
target_include_directories(alerting PUBLIC include)
target_link_libraries(alerting PUBLIC core prediction_engine)

add_library(utils
    src/utils/json_parser.cpp
    src/utils/logger.cpp
    src/utils/time_series.cpp
    src/utils/perf_logger.cpp
)
target_include_directories(utils PUBLIC include)
target_link_libraries(utils PUBLIC core)

add_library(monitoring
    src/monitoring/alert_router.cpp
)
target_include_directories(monitoring PUBLIC include)
target_link_libraries(monitoring PUBLIC core)

add_executable(StablecoinStabilityTracker src/main.cpp)
target_link_libraries(StablecoinStabilityTracker
    data_ingestion analytics prediction_engine alerting utils monitoring
)

enable_testing()

add_executable(test_data_normalizer StablecoinStabilityTracker/tests/test_data_normalizer.cpp)
target_link_libraries(test_data_normalizer data_ingestion)
add_test(NAME TestDataNormalizer COMMAND test_data_normalizer)

add_executable(test_depeg_detection StablecoinStabilityTracker/tests/test_main.cpp)
target_link_libraries(test_depeg_detection analytics)
add_test(NAME TestDepegDetection COMMAND test_depeg_detection)

add_executable(test_risk_scorer tests/test_risk_scorer.cpp)
target_link_libraries(test_risk_scorer prediction_engine)
add_test(NAME TestRiskScorer COMMAND test_risk_scorer)

add_executable(test_pattern_engine tests/test_pattern_engine.cpp)
target_link_libraries(test_pattern_engine analytics)
add_test(NAME TestPatternEngine COMMAND test_pattern_engine)

add_executable(test_pattern_matcher tests/test_pattern_matcher.cpp)
target_link_libraries(test_pattern_matcher analytics)
add_test(NAME TestPatternMatcher COMMAND test_pattern_matcher)

add_executable(test_summary_parser tests/test_summary_parser.cpp)
target_link_libraries(test_summary_parser)
add_test(NAME TestSummaryParser COMMAND test_summary_parser)

add_executable(test_token_decimals tests/test_token_decimals.cpp)
target_link_libraries(test_token_decimals data_ingestion)
add_test(NAME TestTokenDecimals COMMAND test_token_decimals)

include_directories(include include/third_party)
