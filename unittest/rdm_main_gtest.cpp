// test_rdm_main.cpp

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm.h"
//#include "rdmMgr.h"
#include "unittest/mocks/mock_rdm_rbus.h"

MockRdmRbus* g_mockRdmRbus = nullptr;

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
    prdmHandle->pApp_det = (RDMAPPDetails*)malloc(sizeof(RDMAPPDetails));
    EXPECT_CALL(*g_mockRdmRbus, rdmRbusUnInit(testing::_))
        .Times(1);

    rdmUnInit(prdmHandle);
    EXPECT_EQ(prdmHandle->pApp_det, nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
