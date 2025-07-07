/**
 * Copyright 2023 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C" {
    #include "mocks/mock_rdm_rbus.h"
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "rdm_rbus_gtest_report.json"
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

    INT32 rbus_checkStatus() {
	return mockRdmRbus->rbus_checkStatus();
    }

    INT32 rbus_open(void* handle, INT8 *name) {
	return mockRdmRbus->rbus_open(handle, name);
    }

    INT32 rbus_close(void* handle) {
	return mockRdmRbus->rbus_close(handle);
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

TEST(rdmRbusGetRfc, rdmRbusGetRfc_Failure) {

    void* mockValue = static_cast<void*>(new int(0));
    char rdmRFCName[128] = "Path.To.My.RFC";
    INT8 *ipName = rdmRFCName;

    EXPECT_EQ(rdmRbusGetRfc(NULL, ipName, mockValue), RDM_FAILURE);
    EXPECT_EQ(rdmRbusGetRfc(mockValue, NULL, mockValue), RDM_FAILURE);
    EXPECT_EQ(rdmRbusGetRfc(mockValue, ipName, NULL), RDM_FAILURE);

    delete static_cast<int*>(mockValue);

}


GTEST_API_ int main(int argc, char *argv[]){
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );

    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);
    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleTest(&argc, argv);
    //testing::Mock::AllowLeak(mock);
    cout << "Starting rdm_rbus GTEST App ===================>" << endl;
    return RUN_ALL_TESTS();
}
