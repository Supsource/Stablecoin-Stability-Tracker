#pragma once
#include <string>

namespace stablecoin_tracker {
struct TvlFetch {
    double tvl = 0.0;
    bool is_mock = true;
};
TvlFetch fetchTVL(const std::string& coinGeckoId);
}
