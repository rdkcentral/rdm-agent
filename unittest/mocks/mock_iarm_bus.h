#ifndef MOCK_IARM_BUS_H
#define MOCK_IARM_BUS_H

#include <gmock/gmock.h>
#include "rdm_types.h" // Ensure this header includes the definition for IARM_Result_t and other types

extern "C" {
    #include "mocks/libIBus.h"
}

class MockIARM {
public:
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Init, (const char*), ());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_Connect, (), ());
    MOCK_METHOD0(IARM_Bus_Disconnect, IARM_Result_t());
    MOCK_METHOD0(IARM_Bus_Term, IARM_Result_t());
    MOCK_METHOD(IARM_Result_t, IARM_Bus_BroadcastEvent, (const char*, IARM_EventId_t, void*, size_t), ());
};

#endif // MOCK_IARM_BUS_H
