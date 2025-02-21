// iarm_mock.h
#ifndef IARM_MOCK_H  // Include guard
#define IARM_MOCK_H

#include <gmock/gmock.h>
#include "unittest/mocks/libIBus.h"
class MockIARM {
public:
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Init, (const char*), ());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Connect, (), ());
    MOCK_METHOD0(IARM_Bus_Disconnect, IARM_Result_t());
    MOCK_METHOD0(IARM_Bus_Term, IARM_Result_t());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_BroadcastEvent, (const char*, IARM_EventId_t, void*, size_t), ());
};

#endif // IARM_MOCK_H
