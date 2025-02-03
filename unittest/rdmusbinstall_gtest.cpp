#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_usbinstall.h"
#include "rdm.h"
#include "rdm_types.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_download.h"
#include "rdm_downloadutils.h"

using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::_;

// Mocking dependent functions
class MockRDMUtils {
public:
    MOCK_METHOD(void, findPFileAll, (const CHAR*, const CHAR*, CHAR**, INT32*, INT32), ());
    MOCK_METHOD(CHAR*, getPartChar, (CHAR*, char), ());
    MOCK_METHOD(CHAR*, getPartStr, (CHAR*, const char*), ());
    MOCK_METHOD(INT32, rdmJSONGetAppDetName, (CHAR*, RDMAPPDetails*), ());
    MOCK_METHOD(void, rdmUpdateAppDetails, (RDMHandle*, RDMAPPDetails*, INT32), ());
    MOCK_METHOD(void, rdmPrintAppDetails, (RDMAPPDetails*), ());
    MOCK_METHOD(INT32, rdmDownloadApp, (RDMAPPDetails*, INT32*), ());
};

MockRDMUtils* mockRDM;

// Wrappers for the mock functions
extern "C" void findPFileAll(const CHAR* path, const CHAR* search, CHAR** apps, INT32* num, INT32 max) {
    mockRDM->findPFileAll(path, search, apps, num, max);
}
extern "C" CHAR* getPartChar(CHAR* str, char delim) {
    return mockRDM->getPartChar(str, delim);
}
extern "C" CHAR* getPartStr(CHAR* str, const char* search) {
    return mockRDM->getPartStr(str, search);
}
extern "C" INT32 rdmJSONGetAppDetName(CHAR* name, RDMAPPDetails* details) {
    return mockRDM->rdmJSONGetAppDetName(name, details);
}
extern "C" void rdmUpdateAppDetails(RDMHandle* handle, RDMAPPDetails* details, INT32 flag) {
    mockRDM->rdmUpdateAppDetails(handle, details, flag);
}
extern "C" void rdmPrintAppDetails(RDMAPPDetails* details) {
    mockRDM->rdmPrintAppDetails(details);
}
extern "C" INT32 rdmDownloadApp(RDMAPPDetails* details, INT32* status) {
    return mockRDM->rdmDownloadApp(details, status);
}

class RdmUSBInstallTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRDM = new MockRDMUtils();
    }
    void TearDown() override {
        delete mockRDM;
    }
};

// Test rdmUSBInstall
TEST_F(RdmUSBInstallTest, Test_rdmUSBInstall_Success) {
    RDMHandle rdmHandle;
    RDMAPPDetails rdmAppDetails;
    CHAR usbPath[] = "/mnt/usb";
    CHAR* mockApps[RDM_MAX_USB_APP] = {"app1_signed.pkg", "app2_signed.pkg"};
    INT32 numApps = 2;

    EXPECT_CALL(*mockRDM, findPFileAll(_, _, _, _, _))
        .WillOnce(SetArgPointee<3>(numApps));
    EXPECT_CALL(*mockRDM, getPartChar(_, '/'))
        .WillRepeatedly(Return("app1"));
    EXPECT_CALL(*mockRDM, getPartChar(_, '_'))
        .WillRepeatedly(Return("app1_v1"));
    EXPECT_CALL(*mockRDM, getPartStr(_, "signed"))
        .WillRepeatedly(Return("app1_signed"));
    EXPECT_CALL(*mockRDM, rdmJSONGetAppDetName(_, _))
        .WillRepeatedly(Return(RDM_SUCCESS));
    EXPECT_CALL(*mockRDM, rdmDownloadApp(_, _))
        .WillRepeatedly(Return(RDM_SUCCESS));
    EXPECT_CALL(*mockRDM, rdmUpdateAppDetails(_, _, _)).Times(numApps);
    EXPECT_CALL(*mockRDM, rdmPrintAppDetails(_)).Times(numApps);

    ASSERT_EQ(rdmUSBInstall(&rdmHandle, &rdmAppDetails, usbPath), RDM_SUCCESS);
}
