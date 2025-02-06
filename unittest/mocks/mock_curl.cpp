#include "mock_curl.h"

MockCurl* gMockCurl = nullptr;  // Define the global mock instance

extern "C" {
    CURL* curl_easy_init() {
        return gMockCurl ? gMockCurl->curl_easy_init() : nullptr;
    }

    void curl_easy_cleanup(CURL* handle) {
        if (gMockCurl) gMockCurl->curl_easy_cleanup(handle);
    }

    CURLcode curl_easy_setopt(CURL* handle, CURLoption option, ...) {
        return gMockCurl ? gMockCurl->curl_easy_setopt(handle, option) : CURLE_OK;
    }

    CURLcode curl_easy_perform(CURL* handle) {
        return gMockCurl ? gMockCurl->curl_easy_perform(handle) : CURLE_OK;
    }

    CURLcode curl_easy_getinfo(CURL* handle, CURLINFO info, void* output) {
        return gMockCurl ? gMockCurl->curl_easy_getinfo(handle, info, output) : CURLE_OK;
    }
}
