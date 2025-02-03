#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_rbus.h"
#include "rbus.h"
#include "rdm.h"
#include "rdm_types.h"
#include "rdm_utils.h"

using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::_;

// Mocking dependent functions
class MockRbus {
public:
    MOCK_METHOD(rbusError_t, rbus_open, (rbusHandle_t*, const char*), ());
    MOCK_METHOD(rbusError_t, rbus_close, (rbusHandle_t), ());
    MOCK_METHOD(rbusError_t, rbus_checkStatus, (), ());
    MOCK_METHOD(rbusError_t, rbus_get, (rbusHandle_t, const char*, rbusValue_t*), ());
};

MockRbus* mockRbus;

// Wrappers for the mock functions
extern "C" rbusError_t rbus_open(rbusHandle_t* handle, const char* name) {
    return mockRbus->rbus_open(handle, name);
}
extern "C" rbusError_t rbus_close(rbusHandle_t handle) {
    return mockRbus->rbus_close(handle);
}
extern "C" rbusError_t rbus_checkStatus() {
    return mockRbus->rbus_checkStatus();
}
extern "C" rbusError_t rbus_get(rbusHandle_t handle, const char* param, rbusValue_t* value) {
    return mockRbus->rbus_get(handle, param, value);
}

class RdmRbusTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRbus = new MockRbus();
    }
    void TearDown() override {
        delete mockRbus;
    }
};

// Positive Test Cases
TEST_F(RdmRbusTest, Test_rdmRbusInit_Success) {
    void* handle = nullptr;
    INT8 rbusName[] = "TestRbus";
    EXPECT_CALL(*mockRbus, rbus_checkStatus())
        .WillOnce(Return(RBUS_ENABLED));
    EXPECT_CALL(*mockRbus, rbus_open(_, _))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

    ASSERT_EQ(rdmRbusInit(&handle, rbusName), RDM_SUCCESS);
}

TEST_F(RdmRbusTest, Test_rdmRbusGetRfc_Success) {
    void* handle = (void*)1;
    INT8 rdmRFCName[] = "Device.Test.Param";
    INT32 value = 0;
    EXPECT_CALL(*mockRbus, rbus_get(_, _, _))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

    ASSERT_EQ(rdmRbusGetRfc(handle, rdmRFCName, &value), RDM_SUCCESS);
}

TEST_F(RdmRbusTest, Test_rdmRbusUnInit_Success) {
    void* handle = (void*)1;
    EXPECT_CALL(*mockRbus, rbus_close(_))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

    rdmRbusUnInit(handle);
}

// Negative Test Cases
TEST_F(RdmRbusTest, Test_rdmRbusInit_Fail_NullHandle) {
    INT8 rbusName[] = "TestRbus";
    ASSERT_EQ(rdmRbusInit(nullptr, rbusName), RDM_FAILURE);
}

TEST_F(RdmRbusTest, Test_rdmRbusInit_Fail_RbusNotEnabled) {
    void* handle = nullptr;
    INT8 rbusName[] = "TestRbus";
    EXPECT_CALL(*mockRbus, rbus_checkStatus())
        .WillOnce(Return(RBUS_ERROR_NOT_INITIALIZED));

    ASSERT_EQ(rdmRbusInit(&handle, rbusName), RDM_FAILURE);
}

TEST_F(RdmRbusTest, Test_rdmRbusGetRfc_Fail_NullHandle) {
    INT8 rdmRFCName[] = "Device.Test.Param";
    INT32 value = 0;
    ASSERT_EQ(rdmRbusGetRfc(nullptr, rdmRFCName, &value), RDM_FAILURE);
}

TEST_F(RdmRbusTest, Test_rdmRbusGetRfc_Fail_RbusGetFailure) {
    void* handle = (void*)1;
    INT8 rdmRFCName[] = "Device.Test.Param";
    INT32 value = 0;
    EXPECT_CALL(*mockRbus, rbus_get(_, _, _))
        .WillOnce(Return(RBUS_ERROR_INVALID_INPUT));

    ASSERT_EQ(rdmRbusGetRfc(handle, rdmRFCName, &value), RDM_FAILURE);
}

TEST_F(RdmRbusTest, Test_rdmRbusUnInit_Fail_NullHandle) {
    rdmRbusUnInit(nullptr);
}
