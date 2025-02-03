#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_jsonquery.h"
#include "rdm.h"
#include "rdm_types.h"
#include "rdm_utils.h"
#include <cJSON.h>
#include <fstream>

using ::testing::Return;
using ::testing::_;

class RdmJsonTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a sample JSON file for testing
        std::ofstream jsonFile("test_manifest.json");
        jsonFile << R"({
            "apps": [
                {"name": "App1", "size": "10MB", "version": "1.0", "is_versioned": "true"},
                {"name": "App2", "size": "20MB", "version": "2.0", "is_versioned": "false"}
            ]
        })";
        jsonFile.close();
    }

    void TearDown() override {
        remove("test_manifest.json");
    }
};

TEST_F(RdmJsonTest, Test_rdmJSONGetLen_Success) {
    INT32 len = 0;
    ASSERT_EQ(rdmJSONGetLen("test_manifest.json", &len), RDM_SUCCESS);
    ASSERT_EQ(len, 2);
}

TEST_F(RdmJsonTest, Test_rdmJSONGetLen_InvalidFile) {
    INT32 len = 0;
    ASSERT_EQ(rdmJSONGetLen("invalid.json", &len), RDM_FAILURE);
}

TEST_F(RdmJsonTest, Test_rdmJSONQuery_Success) {
    CHAR output[256] = {0};
    ASSERT_EQ(rdmJSONQuery("test_manifest.json", "/apps/0/name", output), RDM_SUCCESS);
    ASSERT_STREQ(output, "App1");
}

TEST_F(RdmJsonTest, Test_rdmJSONQuery_InvalidPath) {
    CHAR output[256] = {0};
    ASSERT_EQ(rdmJSONQuery("test_manifest.json", "/invalid_path", output), RDM_FAILURE);
}

TEST_F(RdmJsonTest, Test_rdmJSONGetAppNames_Success) {
    CHAR appName[256] = {0};
    ASSERT_EQ(rdmJSONGetAppNames(0, appName), RDM_SUCCESS);
    ASSERT_STREQ(appName, "App1");
}

TEST_F(RdmJsonTest, Test_rdmJSONGetAppNames_InvalidIndex) {
    CHAR appName[256] = {0};
    ASSERT_EQ(rdmJSONGetAppNames(10, appName), RDM_FAILURE);
}

TEST_F(RdmJsonTest, Test_rdmJSONGetAppDetID_Success) {
    RDMAPPDetails appDetails;
    ASSERT_EQ(rdmJSONGetAppDetID(0, &appDetails), RDM_SUCCESS);
    ASSERT_STREQ(appDetails.app_name, "App1");
    ASSERT_STREQ(appDetails.pkg_ver, "1.0");
}

TEST_F(RdmJsonTest, Test_rdmJSONGetAppDetName_Success) {
    RDMAPPDetails appDetails;
    ASSERT_EQ(rdmJSONGetAppDetName("App1", &appDetails), RDM_SUCCESS);
    ASSERT_STREQ(appDetails.app_name, "App1");
    ASSERT_STREQ(appDetails.pkg_ver, "1.0");
}

TEST_F(RdmJsonTest, Test_rdmJSONGetAppDetName_InvalidName) {
    RDMAPPDetails appDetails;
    ASSERT_EQ(rdmJSONGetAppDetName("InvalidApp", &appDetails), RDM_FAILURE);
}
