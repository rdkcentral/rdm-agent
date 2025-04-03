#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"


#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmmain_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256

//#include "rdmMgr.h"
//#include "unittest/mocks/mock_rdm_rbus.h"


class MockRdmRbus {
public:
    MOCK_METHOD(int, rdmRbusInit, (void **handle, const char *name), ());
    MOCK_METHOD(void, rdmRbusUnInit, (void *handle), ());
};

MockRdmRbus *g_mockRdmRbus = nullptr;

// Mock function implementations
extern "C" {
    int rdmRbusInit(void **handle, const char *name) {
        if (g_mockRdmRbus) {
            return g_mockRdmRbus->rdmRbusInit(handle, name);
        }
        return -1; // Return an error code if the mock object is not initialized
    }
	 void rdmRbusUnInit(void *handle) {
        if (g_mockRdmRbus) {
            g_mockRdmRbus->rdmRbusUnInit(handle);
        }
    }

    void rdmHelp() {
         printf("Usage:\n");
        printf("To Install apps from manifest : rdm\n");
        printf("To Install single app         : rdm -a <app_name>\n");
        printf("To Install from USB           : rdm -u <usb_path>\n");
        printf("To Install Versioned app      : rdm -v <app_name>\n");
        printf("-b - for broadband devices\n");
        printf("-o - for OSS\n");
        printf("To Print help                 : rdm -h\n");
    }
}

class RDMTest : public ::testing::Test {
protected:
    RDMHandle* prdmHandle;

    virtual void SetUp() {
        prdmHandle = (RDMHandle*)malloc(sizeof(RDMHandle));
        memset(prdmHandle, 0, sizeof(RDMHandle));
        g_mockRdmRbus = new MockRdmRbus();
    }

    virtual void TearDown() {
        if (prdmHandle) {
            free(prdmHandle);
        }
        delete g_mockRdmRbus;
		g_mockRdmRbus = nullptr;
    }
};

TEST_F(RDMTest, rdmInit_NullHandle) {
    EXPECT_EQ(rdmInit(NULL), RDM_FAILURE);
}

TEST_F(RDMTest, rdmInit_Success) {
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusInit(testing::_, testing::_))
        .WillOnce(testing::Return(RDM_SUCCESS));

    EXPECT_EQ(rdmInit(prdmHandle), RDM_SUCCESS);
    EXPECT_NE(prdmHandle->pApp_det, nullptr);
    free(prdmHandle->pApp_det);
}

TEST_F(RDMTest, rdmInit_Failure) {
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusInit(testing::_, testing::_))
        .WillOnce(testing::Return(RDM_FAILURE));

    EXPECT_EQ(rdmInit(prdmHandle), RDM_FAILURE);
    free(prdmHandle->pApp_det);
}

TEST_F(RDMTest, rdmUnInit) {
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusUnInit(prdmHandle->pRbusHandle));

    rdmUnInit(prdmHandle);

    EXPECT_EQ(prdmHandle->pApp_det, nullptr);
}
  TEST_F(RDMTest, rdmUnInit_NullAppDet) {
    if (prdmHandle->pApp_det) {
        free(prdmHandle->pApp_det);
        prdmHandle->pApp_det = nullptr;
    }

    EXPECT_CALL(*g_mockRdmRbus, rdmRbusUnInit(prdmHandle->pRbusHandle));

    rdmUnInit(prdmHandle);

    EXPECT_EQ(prdmHandle->pApp_det, nullptr);
}

TEST_F(RDMTest, rdmHelp_Output) {
    ::testing::internal::CaptureStdout();
    rdmHelp();
    std::string output = ::testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Usage:"), std::string::npos);
    EXPECT_NE(output.find("To Install apps from manifest : rdm"), std::string::npos);
    EXPECT_NE(output.find("To Install single app         : rdm -a <app_name>"), std::string::npos);
    EXPECT_NE(output.find("To Install from USB           : rdm -u <usb_path>"), std::string::npos);
    EXPECT_NE(output.find("To Install Versioned app      : rdm -v <app_name>"), std::string::npos);
    EXPECT_NE(output.find("-b - for broadband devices"), std::string::npos);
    EXPECT_NE(output.find("-o - for OSS"), std::string::npos);
    EXPECT_NE(output.find("To Print help                 : rdm -h"), std::string::npos);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}                          
