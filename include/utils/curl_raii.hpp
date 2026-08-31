#pragma once
#include <curl/curl.h>
#include <string>

namespace stablecoin_tracker {

inline size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

class CurlEasy {
public:
    CurlEasy() : handle_(curl_easy_init()) {}
    ~CurlEasy() {
        if (handle_) {
            curl_easy_cleanup(handle_);
        }
    }
    CurlEasy(const CurlEasy&) = delete;
    CurlEasy& operator=(const CurlEasy&) = delete;

    CURL* get() const { return handle_; }
    explicit operator bool() const { return handle_ != nullptr; }

    void setCommonOptions(const std::string& url, std::string& response,
                          long timeout_sec = 30, long connect_sec = 10) {
        curl_easy_setopt(handle_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
        curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(handle_, CURLOPT_TIMEOUT, timeout_sec);
        curl_easy_setopt(handle_, CURLOPT_CONNECTTIMEOUT, connect_sec);
        curl_easy_setopt(handle_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle_, CURLOPT_USERAGENT, "StablecoinStabilityTracker/1.0");
    }

    long responseCode() const {
        long http_code = 0;
        curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &http_code);
        return http_code;
    }

private:
    CURL* handle_;
};

}
