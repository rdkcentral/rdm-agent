#include <gmock/gmock.h>
#include <gtest/gtest.h>
using ::testing::Return; 
#include <cstring>
#include <openssl/evp.h>

// Include the header file for the functions being tested
extern "C" {
    #include "rdm_types.h"  
    #include "rdm_usbinstall.h"
   
}



#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmusbinstall_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256


// Test fixture class
class RdmUSBInstallTest : public ::testing::Test {
protected:
    RDMHandle rdmHandle;
    RDMAPPDetails appDetails;
    char usbPath[MAX_BUFF_SIZE];

    void SetUp() override {
        memset(&rdmHandle, 0, sizeof(rdmHandle));
        memset(&appDetails, 0, sizeof(appDetails));
        snprintf(usbPath, MAX_BUFF_SIZE, "/mock/usb_path");
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test rdmUSBInstall with valid inputs
TEST_F(RdmUSBInstallTest, RdmUSBInstall_Success) {
    char* mockUsbApps[RDM_MAX_USB_APP] = { strdup("app1_signed.tar"), strdup("app2_signed.tar") };
    int mockNumApps = 2;

    EXPECT_CALL(findPFileAll, (usbPath, RDM_USB_TAR_SEARCH, ::testing::_, ::testing::_, RDM_MAX_USB_APP))
        .WillOnce(::testing::DoAll(::testing::SetArrayArgument<2>(mockUsbApps, mockUsbApps + mockNumApps),
                                   ::testing::SetArgPointee<3>(mockNumApps)));

    EXPECT_CALL(getPartChar, (::testing::_, '/'))
        .WillRepeatedly(::testing::Return(strdup("app1_signed.tar")));
    EXPECT_CALL(getPartChar, (::testing::_, '_'))
        .WillRepeatedly(::testing::Return(strdup("app1")));
    EXPECT_CALL(getPartStr, (::testing::_, "signed"))
        .WillRepeatedly(::testing::Return(strdup("signed")));

    EXPECT_CALL(rdmJSONGetAppDetName, (::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(rdmUpdateAppDetails, (&rdmHandle, ::testing::_, 0));
    EXPECT_CALL(rdmPrintAppDetails, (::testing::_));
    EXPECT_CALL(rdmDownloadApp, (::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(0));

    int result = rdmUSBInstall(&rdmHandle, &appDetails, usbPath);

    EXPECT_EQ(result, RDM_SUCCESS);

    // Cleanup mock allocated memory
    for (int i = 0; i < mockNumApps; ++i) {
        free(mockUsbApps[i]);
    }
}

// Test rdmUSBInstall with no apps found
TEST_F(RdmUSBInstallTest, RdmUSBInstall_NoAppsFound) {
    int mockNumApps = 0;

    EXPECT_CALL(findPFileAll, (usbPath, RDM_USB_TAR_SEARCH, ::testing::_, ::testing::_, RDM_MAX_USB_APP))
        .WillOnce(::testing::SetArgPointee<3>(mockNumApps));

    int result = rdmUSBInstall(&rdmHandle, &appDetails, usbPath);

    EXPECT_EQ(result, RDM_SUCCESS);
}

// Test rdmUSBInstall with JSON parsing failure
TEST_F(RdmUSBInstallTest, RdmUSBInstall_JSONParsingFailure) {
    char* mockUsbApps[RDM_MAX_USB_APP] = { strdup("app1_signed.tar") };
    int mockNumApps = 1;

    EXPECT_CALL(findPFileAll, (usbPath, RDM_USB_TAR_SEARCH, ::testing::_, ::testing::_, RDM_MAX_USB_APP))
        .WillOnce(::testing::DoAll(::testing::SetArrayArgument<2>(mockUsbApps, mockUsbApps + mockNumApps),
                                   ::testing::SetArgPointee<3>(mockNumApps)));

    EXPECT_CALL(getPartChar, (::testing::_, '/'))
        .WillRepeatedly(::testing::Return(strdup("app1_signed.tar")));
    EXPECT_CALL(getPartChar, (::testing::_, '_'))
        .WillRepeatedly(::testing::Return(strdup("app1")));
    EXPECT_CALL(getPartStr, (::testing::_, "signed"))
        .WillRepeatedly(::testing::Return(strdup("signed")));

    EXPECT_CALL(rdmJSONGetAppDetName, (::testing::_, ::testing::_))
        .WillOnce(::testing::Return(1)); // Simulate failure

    int result = rdmUSBInstall(&rdmHandle, &appDetails, usbPath);

    EXPECT_EQ(result, RDM_SUCCESS);

    // Cleanup mock allocated memory
    for (int i = 0; i < mockNumApps; ++i) {
        free(mockUsbApps[i]);
    }
}
