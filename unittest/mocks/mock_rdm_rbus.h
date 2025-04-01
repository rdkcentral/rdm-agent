// mock_rdm_rbus.h
#ifndef MOCK_RDM_RBUS_H
#define MOCK_RDM_RBUS_H

#include <gmock/gmock.h>

// Mock class for rdm_rbus functions
class MockRdmRbus {
public:
    MOCK_METHOD(INT32, rdmRbusInit, (VOID** pRDMRbusHandle, INT8* rbusName), ());
    MOCK_METHOD(VOID, rdmRbusUnInit, (VOID* pRDMbusHandle), ());
    MOCK_METHOD(INT32, rdmRbusGetRfc, (VOID* plRDMRbusHandle, INT8* rdmRFCName, VOID* pValue), ());
};

extern MockRdmRbus* g_mockRdmRbus;

INT32 rdmRbusInit(VOID** pRDMRbusHandle, INT8* rbusName) {
    return g_mockRdmRbus->rdmRbusInit(pRDMRbusHandle, rbusName);
}

VOID rdmRbusUnInit(VOID* pRDMbusHandle) {
    return g_mockRdmRbus->rdmRbusUnInit(pRDMbusHandle);
}

INT32 rdmRbusGetRfc(VOID* plRDMRbusHandle, INT8* rdmRFCName, VOID* pValue) {
    return g_mockRdmRbus->rdmRbusGetRfc(plRDMRbusHandle, rdmRFCName, pValue);
}

#endif // MOCK_RDM_RBUS_H
