#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/mock_rdm_rbus.h"

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdmdownload_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 256
using ::testing::_;
using ::testing::Return;

MockRdmRbus* mockRdmRbus = new MockRdmRbus();

extern "C"{
    rbusValueType_t rbusValue_GetType(blah) {
        return mockRdmRbus->rbusValue_GetType();
    }

    INT32 rbus_get(blah, blah, blah) {
        return mockRdmRbus->rbus_get();
    }

    bool rbusValue_GetBoolean(blah) {
        return mockRdmRbus->rbusValue_GetBoolean();
    }

    INT8* rbusValue_ToString(blah, blah, blah) {
	return mockRdmRbus->rbusValue_ToString();
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

    EXPECT_EQ(rdmRbusGetRfc(mockValue, ipName, mockValue), RDM_SUCCESS);

    delete static_cast<int*>(mockValue);
    delete mockRdmRbus;

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

