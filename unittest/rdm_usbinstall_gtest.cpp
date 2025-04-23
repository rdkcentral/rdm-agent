#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstring>

extern "C" {
    #include "rdm_types.h"
    #include "rdm_usbinstall.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmusbinstall_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
#define MAX_BUFF_SIZE 256  // Define MAX_BUFF_SIZE if not defined elsewhere

class MockRdmUsbInstall {
public:
    MOCK_METHOD(int, findPFileAll, (const char*, const char*, char*[], int*, int));
    MOCK_METHOD(char*, getPartChar, (const char*, char));
    MOCK_METHOD(char*, getPartStr, (const char*, const char*));
    MOCK_METHOD(int, rdmJSONGetAppDetName, (const char*, char*));
    MOCK_METHOD(int, rdmUpdateAppDetails, (RDMHandle*, const char*, int));
    MOCK_METHOD(void, rdmPrintAppDetails, (const char*));
    MOCK_METHOD(int, rdmDownloadApp, (const char*, const char*));
};

// Global mock object
MockRdmUsbInstall* global_mock = nullptr;

// Mock function definitions
extern "C" {
    int findPFileAll(const char* usbPath, const char* ext, char* apps[], int* numApps, int maxApps) {
        return global_mock->findPFileAll(usbPath, ext, apps, numApps, maxApps);
    }

    char* getPartChar(const char* str, char delimiter) {
        return global_mock->getPartChar(str, delimiter);
    }

    char* getPartStr(const char* str, const char* delimiter) {
        return global_mock->getPartStr(str, delimiter);
    }

    int rdmJSONGetAppDetName(const char* jsonFile, char* appName) {
        return global_mock->rdmJSONGetAppDetName(jsonFile, appName);
    }

    int rdmUpdateAppDetails(RDMHandle* handle, const char* appName, int flags) {
        return global_mock->rdmUpdateAppDetails(handle, appName, flags);
    }

    void rdmPrintAppDetails(const char* details) {
        global_mock->rdmPrintAppDetails(details);
    }

    int rdmDownloadApp(const char* appName, const char* destination) {
        return global_mock->rdmDownloadApp(appName, destination);
    }
}

class RdmUsbInstallTest : public ::testing::Test {
protected:
    RDMHandle rdmHandle;
    RDMAPPDetails appDetails;
    char usbPath[MAX_BUFF_SIZE];

    void SetUp() override {
        global_mock = new MockRdmUsbInstall();
        memset(&rdmHandle, 0, sizeof(rdmHandle));
        memset(&appDetails, 0, sizeof(appDetails));
        snprintf(usbPath, MAX_BUFF_SIZE, "/mock/usb_path");
    }

    void TearDown() override {
        delete global_mock;
        global_mock = nullptr;
    }
};

TEST_F(RdmUsbInstallTest, RdmUSBInstall_Success) {
    char* mockUsbApps[RDM_MAX_USB_APP] = { strdup("app1_signed.tar"), strdup("app2_signed.tar") };
    int mockNumApps = 2;

    EXPECT_CALL(*global_mock, findPFileAll(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(::testing::DoAll(::testing::SetArrayArgument<2>(mockUsbApps, mockUsbApps + mockNumApps),
                                   ::testing::SetArgPointee<3>(mockNumApps)));

    EXPECT_CALL(*global_mock, getPartChar(testing::_, testing::_))
        .WillRepeatedly(::testing::Return(strdup("app1_signed.tar")));
    EXPECT_CALL(*global_mock, getPartStr(testing::_, testing::_))
        .WillRepeatedly(::testing::Return(strdup("signed")));

    EXPECT_CALL(*global_mock, rdmJSONGetAppDetName(testing::_, testing::_))
        .WillRepeatedly(::testing::Return(0));
    EXPECT_CALL(*global_mock, rdmUpdateAppDetails(testing::_, testing::_))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(*global_mock, rdmPrintAppDetails(testing::_));
    EXPECT_CALL(*global_mock, rdmDownloadApp(testing::_, testing::_))
        .WillRepeatedly(::testing::Return(0));

    int result = rdmUSBInstall(&rdmHandle, &appDetails, usbPath);

    EXPECT_EQ(result, RDM_SUCCESS);

    // Cleanup mock allocated memory
    for (int i = 0; i < mockNumApps; ++i) {
        free(mockUsbApps[i]);
    }
}

TEST_F(RdmUsbInstallTest, RdmUSBInstall_NoAppsFound) {
    int mockNumApps = 0;

    EXPECT_CALL(*global_mock, findPFileAll(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(::testing::SetArgPointee<3>(mockNumApps));

    int result = rdmUSBInstall(&rdmHandle, &appDetails, usbPath);

    EXPECT_EQ(result, RDM_SUCCESS);
}

TEST_F(RdmUsbInstallTest, RdmUSBInstall_JSONParsingFailure) {
    char* mockUsbApps[RDM_MAX_USB_APP] = { strdup("app1_signed.tar") };
    int mockNumApps = 1;

    EXPECT_CALL(*global_mock, findPFileAll(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(::testing::DoAll(::testing::SetArrayArgument<2>(mockUsbApps, mockUsbApps + mockNumApps),
                                   ::testing::SetArgPointee<3>(mockNumApps)));

    EXPECT_CALL(*global_mock, getPartChar(testing::_, testing::_))
        .WillRepeatedly(::testing::Return(strdup("app1_signed.tar")));
    EXPECT_CALL(*global_mock, getPartStr(testing::_, testing::_))
        .WillRepeatedly(::testing::Return(strdup("signed")));

    EXPECT_CALL(*global_mock, rdmJSONGetAppDetName(testing::_, testing::_))
        .WillOnce(::testing::Return(1)); // Simulate failure

    int result = rdmUSBInstall(&rdmHandle, &appDetails, usbPath);

    EXPECT_EQ(result, RDM_SUCCESS);

    // Cleanup mock allocated memory
    for (int i = 0; i < mockNumApps; ++i) {
        free(mockUsbApps[i]);
    }
}
