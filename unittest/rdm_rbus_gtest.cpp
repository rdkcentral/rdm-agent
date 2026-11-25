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
using namespace testing;
using ::testing::_;
using ::testing::Return;
using namespace std;

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

    void rbusValue_SetBoolean(rbusValue_t value, bool val){
        return mockRdmRbus->rbusValue_SetBoolean(value,val);
    }

    void rbusValue_Init(rbusValue_t* value){
            return mockRdmRbus->rbusValue_Init(value);
    }

    rbusError_t rbus_set(void* handle, char const* val, rbusValue_t value, void** paramName) {
        return mockRdmRbus->rbus_set(handle, val, value, paramName);
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

    EXPECT_EQ(rdmRbusGetRfc(mockValue, ipName, mockValue, sizeof(INT32)), RDM_SUCCESS);

    delete static_cast<int*>(mockValue);
    delete mockRdmRbus;

}

TEST(rdmRbusGetRfc, rdmRbusGetRfc_Failure) {

    void* mockValue = static_cast<void*>(new int(0));
    char rdmRFCName[128] = "Path.To.My.RFC";
    INT8 *ipName = rdmRFCName;

    EXPECT_EQ(rdmRbusGetRfc(NULL, ipName, mockValue, sizeof(INT32)), RDM_FAILURE);
    EXPECT_EQ(rdmRbusGetRfc(mockValue, NULL, mockValue, sizeof(INT32)), RDM_FAILURE);
    EXPECT_EQ(rdmRbusGetRfc(mockValue, ipName, NULL, sizeof(INT32)), RDM_FAILURE);

    delete static_cast<int*>(mockValue);

}

TEST(rdmRbusGetRfc, rdmRbusGetRfc_SuccessString) {

    mockRdmRbus = new MockRdmRbus();
    int handle = 0;
    char rdmRFCName[128] = "Path.To.My.RFC";
    char url[64] = {0};

    EXPECT_CALL(*mockRdmRbus, rbus_get(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));
    EXPECT_CALL(*mockRdmRbus, rbusValue_GetType(::testing::_))
        .WillOnce(Return(RBUS_STRING));
    EXPECT_CALL(*mockRdmRbus, rbusValue_ToString(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(strdup("https://example.com/rdm")));

    EXPECT_EQ(rdmRbusGetRfc(&handle, rdmRFCName, url, sizeof(url)), RDM_SUCCESS);
    EXPECT_STREQ(url, "https://example.com/rdm");

    delete mockRdmRbus;

}

TEST(rdmRbusGetRfc, rdmRbusGetRfc_StringLongerThanBufferRejected) {

    mockRdmRbus = new MockRdmRbus();
    int handle = 0;
    char rdmRFCName[128] = "Path.To.My.RFC";
    struct {
        char url[16];
        INT32 guard;
    } out;
    memset(out.url, 'x', sizeof(out.url));
    out.guard = 0x5A5A5A5A;
    std::string longValue(64, 'a');

    EXPECT_CALL(*mockRdmRbus, rbus_get(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(RBUS_ERROR_SUCCESS));
    EXPECT_CALL(*mockRdmRbus, rbusValue_GetType(::testing::_))
        .WillOnce(Return(RBUS_STRING));
    EXPECT_CALL(*mockRdmRbus, rbusValue_ToString(::testing::_, ::testing::_, ::testing::_))
        .WillOnce(Return(strdup(longValue.c_str())));

    EXPECT_EQ(rdmRbusGetRfc(&handle, rdmRFCName, out.url, sizeof(out.url)), RDM_FAILURE);
    EXPECT_EQ(out.url[0], '\0');
    EXPECT_EQ(out.guard, 0x5A5A5A5A);

    delete mockRdmRbus;

}

TEST(rdmRbusGetRfc, rdmRbusGetRfc_UndersizedOutputRejected) {

    mockRdmRbus = new MockRdmRbus();
    int handle = 0;
    char rdmRFCName[128] = "Path.To.My.RFC";
    char out[sizeof(INT32)] = {0};

    EXPECT_CALL(*mockRdmRbus, rbus_get(::testing::_, ::testing::_, ::testing::_)).Times(0);

    EXPECT_EQ(rdmRbusGetRfc(&handle, rdmRFCName, out, sizeof(INT32) - 1), RDM_FAILURE);

    delete mockRdmRbus;

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
