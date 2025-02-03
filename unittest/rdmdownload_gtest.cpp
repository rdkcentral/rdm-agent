#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdm_downloadverapp.h"
#include "rdm_download.h"
#include "rdm_downloadutils.h"
#include "rdm_types.h"
#include "rdm.h"
#include "rdm_utils.h"

using ::testing::Return;
using ::testing::_;

class MockRdmDownload {
public:
    MOCK_METHOD(INT32, rdmDwnlValidation, (RDMAPPDetails*, CHAR*), ());
    MOCK_METHOD(INT32, rdmDownloadMgr, (RDMAPPDetails*), ());
    MOCK_METHOD(void, rdmDwnlUnInstallApp, (CHAR*, CHAR*), ());
    MOCK_METHOD(void, findPFileAll, (CHAR*, CHAR*, CHAR**, INT32*, INT32), ());
};

MockRdmDownload* mockRdmDownload;

extern "C" INT32 rdmDwnlValidation(RDMAPPDetails* pRdmAppDet, CHAR* pVer) {
    return mockRdmDownload->rdmDwnlValidation(pRdmAppDet, pVer);
}

extern "C" INT32 rdmDownloadMgr(RDMAPPDetails* pRdmAppDet) {
    return mockRdmDownload->rdmDownloadMgr(pRdmAppDet);
}

extern "C" void rdmDwnlUnInstallApp(CHAR* vdapp, CHAR* vhapp) {
    mockRdmDownload->rdmDwnlUnInstallApp(vdapp, vhapp);
}

extern "C" void findPFileAll(CHAR* path, CHAR* pattern, CHAR** pkg_json, INT32* num_ver, INT32 max_list) {
    mockRdmDownload->findPFileAll(path, pattern, pkg_json, num_ver, max_list);
}

class RdmDownloadVerAppTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockRdmDownload = new MockRdmDownload();
    }
    void TearDown() override {
        delete mockRdmDownload;
    }
};

TEST_F(RdmDownloadVerAppTest, Test_rdmDwnlVAVerifyApp_Success) {
    RDMAPPDetails appDet;
    CHAR version[] = "1.0.0";

    EXPECT_CALL(*mockRdmDownload, rdmDwnlValidation(_, _)).WillOnce(Return(0));
    EXPECT_EQ(rdmDwnlVAVerifyApp(&appDet, version), RDM_SUCCESS);
}

TEST_F(RdmDownloadVerAppTest, Test_rdmDwnlVAVerifyApp_Failure) {
    RDMAPPDetails appDet;
    CHAR version[] = "1.0.0";

    EXPECT_CALL(*mockRdmDownload, rdmDwnlValidation(_, _)).WillOnce(Return(RDM_FAILURE));
    EXPECT_EQ(rdmDwnlVAVerifyApp(&appDet, version), RDM_FAILURE);
}

TEST_F(RdmDownloadVerAppTest, Test_rdmDownloadVerApp_Success) {
    RDMAPPDetails appDet;
    memset(&appDet, 0, sizeof(appDet));
    strcpy(appDet.app_name, "TestApp");
    strcpy(appDet.pkg_ver, "1.0.0 2.0.0");

    EXPECT_CALL(*mockRdmDownload, rdmDwnlValidation(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmDownload, rdmDownloadMgr(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmDownload, rdmDwnlUnInstallApp(_, _)).Times(testing::AnyNumber());
    EXPECT_CALL(*mockRdmDownload, findPFileAll(_, _, _, _, _)).Times(testing::AnyNumber());

    EXPECT_EQ(rdmDownloadVerApp(&appDet), RDM_SUCCESS);
}

TEST_F(RdmDownloadVerAppTest, Test_rdmDownloadVerApp_FailInstall) {
    RDMAPPDetails appDet;
    memset(&appDet, 0, sizeof(appDet));
    strcpy(appDet.app_name, "TestApp");
    strcpy(appDet.pkg_ver, "1.0.0 2.0.0");

    EXPECT_CALL(*mockRdmDownload, rdmDwnlValidation(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockRdmDownload, rdmDownloadMgr(_)).WillRepeatedly(Return(RDM_FAILURE));
    EXPECT_CALL(*mockRdmDownload, rdmDwnlUnInstallApp(_, _)).Times(testing::AnyNumber());
    EXPECT_CALL(*mockRdmDownload, findPFileAll(_, _, _, _, _)).Times(testing::AnyNumber());

    EXPECT_EQ(rdmDownloadVerApp(&appDet), RDM_SUCCESS);
}

TEST_F(RdmDownloadVerAppTest, Test_rdmDownloadVerApp_FailValidation) {
    RDMAPPDetails appDet;
    memset(&appDet, 0, sizeof(appDet));
    strcpy(appDet.app_name, "TestApp");
    strcpy(appDet.pkg_ver, "1.0.0 2.0.0");

    EXPECT_CALL(*mockRdmDownload, rdmDwnlValidation(_, _)).WillRepeatedly(Return(RDM_FAILURE));
    EXPECT_CALL(*mockRdmDownload, rdmDownloadMgr(_)).Times(0);
    EXPECT_CALL(*mockRdmDownload, rdmDwnlUnInstallApp(_, _)).Times(0);
    EXPECT_CALL(*mockRdmDownload, findPFileAll(_, _, _, _, _)).Times(testing::AnyNumber());

    EXPECT_EQ(rdmDownloadVerApp(&appDet), RDM_SUCCESS);
}
