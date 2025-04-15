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
    MOCK_METHOD1(checkFileSystem, INT32(const CHAR*));
    MOCK_METHOD3(findPFile, INT32(const CHAR*, const CHAR*, CHAR*));
    MOCK_METHOD1(emptyFolder, INT32(const CHAR*));
    MOCK_METHOD1(getFreeSpace, UINT32(const CHAR*));
    MOCK_METHOD1(createDir, INT32(const CHAR*));
    MOCK_METHOD0(doCurlInit, void*());
    //MOCK_METHOD6(doHttpFileDownload, INT32(void*, FileDwnl_t*, MtlsAuth_t*, unsigned int, char*, int*));
    MOCK_METHOD1(doStopDownload, void(void*));
};

#endif // MOCK_RDM_UTILS_H
