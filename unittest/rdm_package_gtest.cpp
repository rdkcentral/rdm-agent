#include <gtest/gtest.h>
#include <gmock/gmock.h>
extern "C" {
#include "rdm_types.h"
#include "rdm_package.h" 
}

// ---- MOCKS FOR ALL EXTERNAL DEPENDENCIES ----
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;

// Mock all external C functions used in the package manager
extern "C" {
    // These are just examples; add all functions your code calls
    int fileCheck(const char*) { return 0; }
    int findSize(const char*) { return 123; }
    void removeFile(const char*) {}
    void RDMInfo(const char*, ...) {}
    void RDMError(const char*, ...) {}
    void t2CountNotify(const char*, int) {}
    int rdmMemDLAlloc(void*, int) { return RDM_SUCCESS; }
    void rdmMemDLFree(void*) {}
    void* doCurlInit() { static int dummy; return &dummy; }
    int getJsonRpcData(void*, void*, const char*, int*) { return 0; }
    void doStopDownload(void*) {}
    int rdmJRPCTokenData(char*, char*, size_t) { return 0; }
    int getProcessID(const char*, void*) { return 1; }
    void copyCommandOutput(const char*, char*, size_t) {}
    void rdmDwnlUnInstallApp(const char*, const char*) {}
    void rdmDwnlRunPostScripts(const char*, int) {}
    int sleep(int) { return 0; }
}

// ---- TEST FIXTURE ----
class RdmPackageTest : public ::testing::Test {
protected:
    RDMAPPDetails appDetails;
    void SetUp() override {
        memset(&appDetails, 0, sizeof(appDetails));
        strcpy(appDetails.app_name, "TestApp");
        strcpy(appDetails.app_dwnl_path, "/tmp/testapp/");
        strcpy(appDetails.app_dwnl_url, "http://example.com/testapp.pkg");
        strcpy(appDetails.app_home, "/opt/testapp/");
    }
};

// ---- TEST CASES ----

// Success path
TEST_F(RdmPackageTest, PackageMgr_SuccessfulFlow) {
    // All mocks already return success
    ASSERT_EQ(rdmPackageMgr(&appDetails), RDM_SUCCESS);
}

// Simulate signature validation failure
TEST_F(RdmPackageTest, PackageMgr_SignatureValidationFails) {
    // Mock fileCheck to simulate PACKAGE_SIGN_VERIFY_SUCCESS never appears, but PACKAGE_SIGN_VERIFY_FAILED does
    static int call = 0;
    fileCheck = [](const char* path) -> int {
        if (strcmp(path, PACKAGE_SIGN_VERIFY_SUCCESS) == 0) return 0;
        if (strcmp(path, PACKAGE_SIGN_VERIFY_FAILED) == 0) return 1;
        return 0;
    };
    ASSERT_EQ(rdmPackageMgr(&appDetails), RDM_FAILURE);
}

// Simulate packager not running (getProcessID always returns 0)
TEST_F(RdmPackageTest, InvokePackage_RetriesAndFails) {
    getProcessID = [](const char*, void*) -> int { return 0; };
    ASSERT_EQ(rdmPackageMgr(&appDetails), RDM_FAILURE);
}

// Simulate memory allocation failure in rdmPkgDwnlApplication
TEST_F(RdmPackageTest, DwnlApplication_MemoryAllocFails) {
    rdmMemDLAlloc = [](void*, int) -> int { return RDM_FAILURE; };
    ASSERT_EQ(rdmPackageMgr(&appDetails), RDM_FAILURE);
}

// Simulate curl init failure
TEST_F(RdmPackageTest, DwnlApplication_CurlInitFails) {
    rdmMemDLAlloc = [](void*, int) -> int { return RDM_SUCCESS; };
    doCurlInit = []() -> void* { return nullptr; };
    ASSERT_EQ(rdmPackageMgr(&appDetails), RDM_FAILURE);
}

// Simulate download failure (getJsonRpcData returns error)
TEST_F(RdmPackageTest, DwnlApplication_DownloadFails) {
    rdmMemDLAlloc = [](void*, int) -> int { return RDM_SUCCESS; };
    doCurlInit = []() -> void* { static int dummy; return &dummy; };
    getJsonRpcData = [](void*, void*, const char*, int*) -> int { return 1; };
    ASSERT_EQ(rdmPackageMgr(&appDetails), RDM_FAILURE);
}

