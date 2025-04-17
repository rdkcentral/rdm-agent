#ifndef MOCK_RDM_RBUS_H
#define MOCK_RDM_RBUS_H

#define RBUS_ERROR_SUCCESS 0

extern "C" {
    #include "rdm_types.h"
    #include "rdm_utils.h"
    #include "rdm_rbus.h"
}

typedef enum
{
    RBUS_BOOLEAN  = 0x500,  /**< bool true or false */
    RBUS_CHAR,              /**< char of size 1 byte*/
    RBUS_BYTE,              /**< unsigned char */
    RBUS_INT8,              /**< 8 bit int */
    RBUS_UINT8,             /**< 8 bit unsigned int */
    RBUS_INT16,             /**< 16 bit int */
    RBUS_UINT16,            /**< 16 bit unsigned int */
    RBUS_INT32,             /**< 32 bit int */
    RBUS_UINT32,            /**< 32 bit unsigned int */
    RBUS_INT64,             /**< 64 bit int */
    RBUS_UINT64,            /**< 64 bit unsigned int */
    RBUS_SINGLE,            /**< 32 bit float */
    RBUS_DOUBLE,            /**< 64 bit float */
    RBUS_DATETIME,          /**< rbusDateTime_t structure for Date/Time */
    RBUS_STRING,            /**< null terminated C style string */
    RBUS_BYTES,             /**< byte array */
    RBUS_PROPERTY,          /**< property instance */
    RBUS_OBJECT,            /**< object instance */
    RBUS_NONE
} rbusValueType_t;

struct rbusValue_t
{
    union
    {
        bool                    b;
        char                    c;
        unsigned char           u;
        int8_t                  i8;
        uint8_t                 u8;
        int16_t                 i16;
        uint16_t                u16;
        int32_t                 i32;
        uint32_t                u32;
        int64_t                 i64;
        uint64_t                u64;
        float                   f32;
        double                  f64;
    } d;
    rbusValueType_t type;
};

struct rbusHandle_t
{
  char*                 componentName;
  int32_t               componentId;
};

class MockRdmRbus {
public:
    MOCK_METHOD(rbusValueType_t, rbusValue_GetType, (void*), ());
    MOCK_METHOD(INT32, rbus_get, (void*, INT8*, void*), ());
    MOCK_METHOD(INT8*, rbusValue_ToString, (void*, void*, int), ());
    MOCK_METHOD(bool, rbusValue_GetBoolean, (void*), ());
};

#endif // MOCK_RDM_RBUS_H

