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
    MOCK_METHOD(void*, doCurlInit, (), ());
    MOCK_METHOD(void, doStopDownload, (void *), ());
    MOCK_METHOD(void, copyCommandOutput, (char*, char*, int), ());

};

#endif // MOCK_RDM_UTILS_H
