#ifndef MOCK_RDM_RBUS_H
#define MOCK_RDM_RBUS_H

#include <gmock/gmock.h>
#include "mock_rdm_rbustypes.h"

class MockRdmRbus {
public:
    MOCK_METHOD(rbusValueType_t, rbusValue_GetType, (void*), ());
    MOCK_METHOD(INT32, rbus_get, (void*, INT8*, void*), ());
    MOCK_METHOD(const INT8*, rbusValue_ToString, (void*, void*, int), ());
    MOCK_METHOD(bool, rbusValue_GetBoolean, (void*), ());
    MOCK_METHOD(void, rbusValue_Release, (void*), ());
};

#endif // MOCK_RDM_RBUS_H

