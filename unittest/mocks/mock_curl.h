#ifndef MOCK_CURL_H
#define MOCK_CURL_H

#include <curl/curl.h>
#include <gmock/gmock.h>
#include <cstdarg>

class MockCurl {
public:
    MOCK_METHOD0(curl_easy_init, CURL*());
    MOCK_METHOD1(curl_easy_cleanup, void(CURL*));
    #undef curl_easy_setopt
    #undef curl_easy_getinfo
    MOCK_METHOD3(curl_easy_setopt_long, CURLcode(CURL*, CURLoption, long));
    MOCK_METHOD3(curl_easy_setopt_ptr, CURLcode(CURL*, CURLoption, void*));
    MOCK_METHOD3(curl_easy_setopt_str, CURLcode(CURL*, CURLoption, const char*));

    // Provide specific overloads for curl_easy_getinfo
    MOCK_METHOD3(curl_easy_getinfo_long, CURLcode(CURL*, CURLINFO, long*));
    MOCK_METHOD3(curl_easy_getinfo_ptr, CURLcode(CURL*, CURLINFO, void**));
    MOCK_METHOD3(curl_easy_getinfo_double, CURLcode(CURL*, CURLINFO, double*));
    MOCK_METHOD1(curl_easy_perform, CURLcode(CURL*));

};
// Singleton pattern to allow easy access to the mock instance
extern MockCurl* mockCurl;

extern "C" {
    CURL* curl_easy_init() {
        return mockCurl->curl_easy_init();
    }

    void curl_easy_cleanup(CURL* handle) {
        mockCurl->curl_easy_cleanup(handle);
    }

    CURLcode curl_easy_setopt(CURL* handle, CURLoption option, ...) {
        va_list args;
        va_start(args, option);
        CURLcode result = CURLE_OK;

        switch(option) {
            case CURLOPT_URL:
            case CURLOPT_SSLCERTTYPE:
            case CURLOPT_SSLCERT:
            case CURLOPT_KEYPASSWD:
                result = mockCurl->curl_easy_setopt_str(handle, option, va_arg(args, const char*));
                break;
            case CURLOPT_SSLVERSION:
            case CURLOPT_CONNECTTIMEOUT:
            case CURLOPT_TIMEOUT:
            case CURLOPT_SSLENGINE_DEFAULT:
            case CURLOPT_SSL_VERIFYPEER:
                result = mockCurl->curl_easy_setopt_long(handle, option, va_arg(args, long));
                break;
            case CURLOPT_WRITEFUNCTION:
            case CURLOPT_WRITEDATA:
                result = mockCurl->curl_easy_setopt_ptr(handle, option, va_arg(args, void*));
                break;
            default:
                result = CURLE_UNKNOWN_OPTION;
                break;
        }

        va_end(args);
        return result;
    }

    CURLcode curl_easy_perform(CURL* handle) {
        return mockCurl->curl_easy_perform(handle);
    }

    CURLcode curl_easy_getinfo(CURL* handle, CURLINFO info, ...) {
        va_list args;
        va_start(args, info);
        CURLcode result = CURLE_OK;

        switch(info) {
            case CURLINFO_RESPONSE_CODE:
                result = mockCurl->curl_easy_getinfo_long(handle, info, va_arg(args, long*));
                break;
            case CURLINFO_CONTENT_TYPE:
                result = mockCurl->curl_easy_getinfo_ptr(handle, info, va_arg(args, void**));
                break;
            case CURLINFO_TOTAL_TIME:
                result = mockCurl->curl_easy_getinfo_double(handle, info, va_arg(args, double*));
                break;
            default:
                // Handle other cases or log an error
                result = CURLE_UNKNOWN_OPTION;
                break;         
        }

        va_end(args);
        return result;
    }
}

#endif // MOCK_CURL_H
