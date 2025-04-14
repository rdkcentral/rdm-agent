#ifndef MOCK_RDM_DOWNLOADUTILS_H
#define MOCK_RDM_DOWNLOADUTILS_H

#include <gmock/gmock.h>

extern "C" {
    #include "rdm_types.h"
    #include "rdm.h"
    #include "rdm_utils.h"
    #include "rdm_downloadutils.h"
}

class MockRdmDownloadUtils {
public:
        MOCK_METHOD1(rdmDownloadMgr, int(RDMAPPDetails*));
        MOCK_METHOD2(rdmDwnlUnInstallApp, void(const char*, const char*));
        MOCK_METHOD1(rdmDownloadVerApp, int(RDMAPPDetails*));
        MOCK_METHOD1(rdmDwnlGetCert, int(MockMtlsAuth_t*));
};

#endif // MOCK_RDM_DOWNLOADUTILS_H
