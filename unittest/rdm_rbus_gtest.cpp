#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C" {
    #include "mocks/mock_rdm_rbus.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmdownload_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
#define RBUS_ERROR_SUCCESS 0
using ::testing::_;
using ::testing::Return;

MockRdmRbus* mockRdmRbus = new MockRdmRbus();

extern "C"{
    rbusValueType_t rbusValue_GetType(void* paramName) {
        return mockRdmRbus->rbusValue_GetType(paramName);
    }

    INT32 rbus_get(void* handle, INT8* val, void** paramName) {
        return mockRdmRbus->rbus_get(handle, val, paramName);
    }

    bool rbusValue_GetBoolean(void* paramName) {
        return mockRdmRbus->rbusValue_GetBoolean(paramName);
    }

    INT8* rbusValue_ToString(void* str, void* paramName, int len) {
	return mockRdmRbus->rbusValue_ToString(str, paramName, len);
    }

    void rbusValue_Release(void* ptr) {
	return mockRdmRbus->rbusValue_Release(ptr);
    }
}

// Test rdmRbusGetRfc
TEST(rdmRbusGetRfc, rdmRbusGetRfc_SuccessBool) {

    void* mockValue = static_cast<void*>(new int(0));
    char rdmRFCName[128] = "Path.To.My.RFC";
    INT8 *ipName = rdmRFCName; 
    
    EXPECT_CALL(*mockRdmRbus, rbusValue_GetType(::testing::_))
        .WillOnce(Return(RBUS_BOOLEAN));

    EXPECT_CALL(*mockRdmRbus, rbus_get(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));

    //MOCK_METHOD(INT8*, rbusValue_ToString, (void*, void*, int), ());
    EXPECT_CALL(*mockRdmRbus, rbusValue_GetBoolean(::testing::_))
        .WillOnce(Return(true));

    EXPECT_CALL(*mockRdmRbus, rbusValue_Release(::testing::_));

    EXPECT_EQ(rdmRbusGetRfc(mockValue, ipName, mockValue), RDM_SUCCESS);

    delete static_cast<int*>(mockValue);
    delete mockRdmRbus;

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

