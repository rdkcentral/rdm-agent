#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "libIBus.h"
#include "rdmMgr.h"
#include "rdm_types.h"
#include "rdm_utils.h"
#include "rdm_download.h"
#include <dirent.h>
#include <string.h>
#include <cstdlib>

// Mock IARM functions for isolated testing
class MockIARM {
public:
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Init, (const char*), ());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Connect, (), ());
    MOCK_METHOD(void, IARM_Bus_Disconnect, (), ());
    MOCK_METHOD(void, IARM_Bus_Term, (), ());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_BroadcastEvent, (const char*, IARM_EventId_t, void*, size_t), ());
};

MockIARM* mockIARM = nullptr;

// Helper functions to mock behavior
IARM_Result_t IARM_Bus_Init(const char* name) {
    return mockIARM->IARM_Bus_Init(name);
}
IARM_Result_t IARM_Bus_Connect() {
    return mockIARM->IARM_Bus_Connect();
}
void IARM_Bus_Disconnect() {
    mockIARM->IARM_Bus_Disconnect();
}
void IARM_Bus_Term() {
    mockIARM->IARM_Bus_Term();
}
IARM_Result_t IARM_Bus_BroadcastEvent(const char* owner, IARM_EventId_t eventId, void* data, size_t len) {
    return mockIARM->IARM_Bus_BroadcastEvent(owner, eventId, data, len);
}

class RdmTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockIARM = new MockIARM();
    }

    void TearDown() override {
        delete mockIARM;
        mockIARM = nullptr;
    }
};

// Test rdmIARMEvntSendStatus
TEST_F(RdmTest, rdmIARMEvntSendStatus_Success) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init("AppDownloadEvent"))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_BroadcastEvent(IARM_BUS_RDMMGR_NAME, IARM_BUS_RDMMGR_EVENT_APPDOWNLOADS_CHANGED, ::testing::_, sizeof(UINT8)))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Disconnect());
    EXPECT_CALL(*mockIARM, IARM_Bus_Term());

    EXPECT_EQ(rdmIARMEvntSendStatus(1), RDM_SUCCESS);
}

TEST_F(RdmTest, rdmIARMEvntSendStatus_InitFailure) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init("AppDownloadEvent"))
        .WillOnce(::testing::Return(IARM_RESULT_IPCCORE_FAIL));

    EXPECT_EQ(rdmIARMEvntSendStatus(1), RDM_FAILURE);
}

TEST_F(RdmTest, rdmIARMEvntSendStatus_ConnectFailure) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init("AppDownloadEvent"))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillOnce(::testing::Return(IARM_RESULT_IPCCORE_FAIL));
    EXPECT_CALL(*mockIARM, IARM_Bus_Term());

    EXPECT_EQ(rdmIARMEvntSendStatus(1), RDM_FAILURE);
}

// Test rdmIARMEvntSendPayload
TEST_F(RdmTest, rdmIARMEvntSendPayload_Success) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init("AppDownloadEvent"))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Connect())
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_BroadcastEvent(IARM_BUS_RDMMGR_NAME, IARM_BUS_RDMMGR_EVENT_APP_INSTALLATION_STATUS, ::testing::_, sizeof(IARM_Bus_RDMMgr_EventData_t)))
        .WillOnce(::testing::Return(IARM_RESULT_SUCCESS));
    EXPECT_CALL(*mockIARM, IARM_Bus_Disconnect());
    EXPECT_CALL(*mockIARM, IARM_Bus_Term());

    EXPECT_EQ(rdmIARMEvntSendPayload((CHAR*)"test_pkg", (CHAR*)"1.0", (CHAR*)"/path/to/pkg", 1), RDM_SUCCESS);
}

TEST_F(RdmTest, rdmIARMEvntSendPayload_InitFailure) {
    EXPECT_CALL(*mockIARM, IARM_Bus_Init("AppDownloadEvent"))
        .WillOnce(::testing::Return(IARM_RESULT_IPCCORE_FAIL));

    EXPECT_EQ(rdmIARMEvntSendPayload((CHAR*)"test_pkg", (CHAR*)"1.0", (CHAR*)"/path/to/pkg", 1), RDM_FAILURE);
}

// Test rdmListDirectory
TEST(RdmDirectoryTest, rdmListDirectory_Success) {
    CHAR* dirList[10];
    INT32 numDirs = 0;

    // Create a temporary directory for testing
    system("mkdir -p /tmp/test_rdm_dir && touch /tmp/test_rdm_dir/file1 /tmp/test_rdm_dir/file2");

    EXPECT_EQ(rdmListDirectory((CHAR*)"/tmp/test_rdm_dir", dirList, &numDirs), RDM_SUCCESS);
    EXPECT_EQ(numDirs, 2);

    for (int i = 0; i < numDirs; i++) {
        free(dirList[i]);
    }

    system("rm -rf /tmp/test_rdm_dir");
}

TEST(RdmDirectoryTest, rdmListDirectory_Failure) {
    CHAR* dirList[10];
    INT32 numDirs = 0;

    EXPECT_EQ(rdmListDirectory((CHAR*)"/nonexistent", dirList, &numDirs), RDM_FAILURE);
}

TEST(RdmDirectoryTest, rdmListDirectory_NullArgument) {
    CHAR* dirList[10];
    INT32 numDirs = 0;

    EXPECT_EQ(rdmListDirectory(nullptr, dirList, &numDirs), RDM_FAILURE);
}
