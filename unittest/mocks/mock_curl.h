#ifndef MOCK_CURL_H
#define MOCK_CURL_H

#include <gmock/gmock.h>
#include <curl/curl.h>

class MockCurl {
public:
    MOCK_METHOD(CURL*, curl_easy_init, (), ());
    MOCK_METHOD(void, curl_easy_cleanup, (CURL*), ());
    MOCK_METHOD(CURLcode, curl_easy_setopt, (CURL*, CURLoption, ...), ());
    MOCK_METHOD(CURLcode, curl_easy_perform, (CURL*), ());
    MOCK_METHOD(CURLcode, curl_easy_getinfo, (CURL*, CURLINFO, void*), ());
};

extern MockCurl* gMockCurl;  // Global instance to be used in tests

#endif // MOCK_CURL_H
