#ifndef MOCK_RDM_UTILS_H
#define MOCK_RDM_UTILS_H

#include <gmock/gmock.h>

extern "C" {
    #include "rdm_types.h"
    #include "rdm.h"
    #include "rdm_utils.h"
    #include "system_utils.h"
}

class MockRdmUtils {
public:
    MOCK_METHOD(void*, doCurlInit, (), ());
    MOCK_METHOD(INT32, doHttpFileDownload, (void*, FileDwnl_t*, MtlsAuth_t*, unsigned int, char*, int*), ());
    MOCK_METHOD(void, doStopDownload, (void *), ());
    MOCK_METHOD(void, copyCommandOutput, (char*, char*, int), ());
};

#endif // MOCK_RDM_UTILS_H
