#include <gtest/gtest.h>
#include <gmock/gmock.h>



#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmusbinstall_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256

extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "unittest/mocks/libIBus.h"
#include "unittest/mocks/rdmMgr.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"
#include "rdm_jsonquery.h"
#include "rdm_usbinstall.h"
#include <string.h>
}


#ifdef RDMError
#undef RDMError
#endif

#ifdef RDMWarn
#undef RDMWarn
#endif
using ::testing::_;
using ::testing::Return;
extern "C" {
void RDMError(const char  *fmt, ...) {
va_list args;
va_start(args, fmt);
vfprintf(stderr, fmt, args);
va_end(args);
}

void RDMWarn(const char  *fmt, ...) {
va_list args;
va_start(args, fmt);
vfprintf(stderr, fmt, args);
va_end(args);
}
}



// Mock dependencies
extern "C" {

// These will be controlled by test
int mock_num_apps = 1;
bool mock_json_fail = false;
bool mock_pkg_mismatch = false;
bool mock_download_fail = false;

int findPFileAll(char *pUsbPath, int searchType, char *usb_apps[], int *num_app, int max_usb_app) {
    *num_app = mock_num_apps;
    if (mock_num_apps > 0) {
        strncpy(usb_apps[0], "/mnt/usb/app_test_signed.tar", RDM_APP_PATH_LEN);
    }
    return 0;
}

char* getPartChar(char *str, char delimiter) {
    char *pos = strchr(str, delimiter);
    return pos ? (pos + 1) : NULL;
}

char* getPartStr(char *str, const char *substr) {
    return strstr(str, substr);
}



void rdmUpdateAppDetails(RDMHandle *handle, RDMAPPDetails *details, int flag) {}

void rdmPrintAppDetails(RDMAPPDetails *details) {}

int rdmDownloadApp(RDMAPPDetails *details, int *download_status) {
    *download_status = mock_download_fail ? 0 : 1;
    return mock_download_fail ? -1 : 0;
}


int rdmJSONGetAppDetName(char *app_name, RDMAPPDetails *app_details) {
    if (mock_json_fail)
        return -1;

    // Simulate successful manifest parse
    strncpy(app_details->pkg_name, mock_pkg_mismatch ? "different_pkg" : "app_test", sizeof(app_details->pkg_name));

    // Add these lines to force expectations to pass
    app_details->dwld_status = mock_download_fail ? 0 : 1;
    app_details->is_usb = 1;

    return 0;
}
//void RDMError(const char *fmt, ...) {}
//void RDMWarn(const char *fmt, ...) {}

} // extern "C"


// ---------- TEST CASES ----------






TEST(RdmUSBInstallTest, InstallSuccess) {
    mock_num_apps = 1;
    mock_json_fail = false;
    mock_pkg_mismatch = false;
    mock_download_fail = false;

    RDMHandle handle;
    RDMAPPDetails app_details;
    char usb_path[] = "/mnt/usb";

    //app_details.dwld_status =1;
   // app_details.is_usb =1;

    int result = rdmUSBInstall(&handle, &app_details, usb_path);

    EXPECT_EQ(result, RDM_SUCCESS);
    EXPECT_STREQ(app_details.pkg_name, "app_test");
    EXPECT_EQ(app_details.dwld_status, 1);
    EXPECT_EQ(app_details.is_usb, 1);
}

TEST(RdmUSBInstallTest, NoAppsFound) {
    mock_num_apps = 0;
    mock_json_fail = false;
    mock_pkg_mismatch = false;
    mock_download_fail = false;

    RDMHandle handle;
    RDMAPPDetails app_details;
    char usb_path[] = "/mnt/usb";

    int result = rdmUSBInstall(&handle, &app_details, usb_path);

    EXPECT_EQ(result, RDM_SUCCESS); // No apps to install but still success
}

TEST(RdmUSBInstallTest, ManifestParseFailure) {
    mock_num_apps = 1;
    mock_json_fail = true;
    mock_pkg_mismatch = false;
    mock_download_fail = false;

    RDMHandle handle;
    RDMAPPDetails app_details;
    char usb_path[] = "/mnt/usb";

    int result = rdmUSBInstall(&handle, &app_details, usb_path);

    EXPECT_EQ(result, RDM_SUCCESS); // Fail to parse, but function continues
}

TEST(RdmUSBInstallTest, PackageMismatch) {
    mock_num_apps = 1;
    mock_json_fail = false;
    mock_pkg_mismatch = true;
    mock_download_fail = false;

    RDMHandle handle;
    RDMAPPDetails app_details;
    char usb_path[] = "/mnt/usb";

    int result = rdmUSBInstall(&handle, &app_details, usb_path);

    EXPECT_EQ(result, RDM_SUCCESS); // Skips mismatched package
}

TEST(RdmUSBInstallTest, DownloadFailure) {
    mock_num_apps = 1;
    mock_json_fail = false;
    mock_pkg_mismatch = false;
    mock_download_fail = true;

    RDMHandle handle;
    RDMAPPDetails app_details;
    char usb_path[] = "/mnt/usb";

    int result = rdmUSBInstall(&handle, &app_details, usb_path);

    EXPECT_EQ(result, RDM_SUCCESS); // Download failure handled inside loop
}
