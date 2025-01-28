#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "rdk_fwdl_utils.h"
#include "codebigUtils.h"
#include "rdm_types.h"
#include "json_parse.h"
#include <fstream>
#include <string>
#include <cstring>

using ::testing::Return;
using ::testing::StrEq;

// Mock dependencies
class MockCodeBigUtils {
public:
    MOCK_METHOD(int, read_RFCProperty, (char* type, const char* key, char* out_value, size_t datasize), ());
};

MockCodeBigUtils mockUtils;

// Helper function to create temporary test files
void createTestFile(const char* filename, const char* content) {
    std::ofstream outfile(filename);
    outfile << content;
    outfile.close();
}

void deleteTestFile(const char* filename) {
    std::remove(filename);
}

// Test cases
TEST(CodeBigUtilsTest, DoCodeBigSigning_ValidInputs) {
    char signUrl[256] = {0};
    char outHeader[256] = {0};
    
    int result = doCodeBigSigning(1, "InputString", signUrl, sizeof(signUrl), outHeader, sizeof(outHeader));
    
    EXPECT_EQ(result, 0);
    EXPECT_STREQ(signUrl, "");
    EXPECT_STREQ(outHeader, "");
}

TEST(CodeBigUtilsTest, GetXconfCodebigDetails_ValidInputs) {
    char pIn[256] = "Sample Input";
    char plocation[256] = {0};

    int result = GetXconfCodebigDetails(pIn, plocation, sizeof(plocation));

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(plocation, "");
}

TEST(CodeBigUtilsTest, GetSSRCodebigDetails_ValidInputs) {
    char pIn[256] = "Sample Input";
    char plocation[256] = {0};
    char pauthHeader[256] = {0};

    int result = GetSSRCodebigDetails(pIn, plocation, sizeof(plocation), pauthHeader, sizeof(pauthHeader));

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(plocation, "");
    EXPECT_STREQ(pauthHeader, "");
}

TEST(CodeBigUtilsTest, GetXconfRequestType_ValidInput) {
    char data[256] = "type=1";

    int result = getXconfRequestType(data, strlen(data));

    EXPECT_EQ(result, 0);
}

TEST(CodeBigUtilsTest, GetXconfRequestType_InvalidInput) {
    int result = getXconfRequestType(nullptr, 0);
    EXPECT_EQ(result, 0);
}

TEST(CodeBigUtilsTest, ReadRFCProperty_Success) {
    char out_value[256] = {0};
    EXPECT_CALL(mockUtils, read_RFCProperty(StrEq("Type"), StrEq("Key"), _, 256))
        .WillOnce(Return(1));

    int result = read_RFCProperty((char*)"Type", "Key", out_value, sizeof(out_value));

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(out_value, "");
}

TEST(CodeBigUtilsTest, GetServerUrlFile_ValidFile) {
    const char* testFile = "/tmp/testfile.txt";
    createTestFile(testFile, "http://example.com/test");

    char buffer[256] = {0};
    size_t result = GetServerUrlFile(buffer, sizeof(buffer), (char*)testFile);

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(buffer, "");

    deleteTestFile(testFile);
}

TEST(CodeBigUtilsTest, GetServerUrlFile_FileNotFound) {
    char buffer[256] = {0};

    size_t result = GetServerUrlFile(buffer, sizeof(buffer), (char*)"/tmp/nonexistentfile.txt");

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(buffer, "");
}

TEST(CodeBigUtilsTest, GetTR181Url_ValidInput) {
    char buffer[256] = {0};
    size_t result = GetTR181Url(TR181URL::URL_XCONF, buffer, sizeof(buffer));

    EXPECT_EQ(result, 0);
    EXPECT_STREQ(buffer, "");
}
