#pragma once
#ifdef __cplusplus
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#endif    // Only included in C++ mode




#ifdef IARMBUS_SUPPORT
/* ----------------- RDMMgr ---------- */
#define IARM_BUS_RDMMGR_NAME "RDMMgr"
#define RDM_PKG_NAME_MAX_SIZE 128
#define RDM_PKG_VERSION_MAX_SIZE 8
#define RDM_PKG_INST_PATH_MAX_SIZE 256
#endif

typedef int INT32;
typedef void VOID;
typedef char CHAR;

// Forward declare RDMHandle instead of including rdm.h
typedef struct _rdmHandle RDMHandle;



// Function declarations
INT32 rdmInit(RDMHandle* handle);
VOID rdmUnInit(RDMHandle* handle);



typedef enum _RDMMgr_EventId_t
{
    IARM_BUS_RDMMGR_EVENT_APPDOWNLOADS_CHANGED = 0,
    IARM_BUS_RDMMGR_EVENT_APP_INSTALLATION_STATUS,
    IARM_BUS_RDMMGR_EVENT_MAX
} IARM_Bus_RDMMgr_EventId_t;

typedef enum _RDMMgr_Status_t
{
    RDM_PKG_INSTALL_COMPLETE = 0,
    RDM_PKG_INSTALL_ERROR,
    RDM_PKG_DOWNLOAD_COMPLETE,
    RDM_PKG_DOWNLOAD_ERROR,
    RDM_PKG_EXTRACT_COMPLETE,
    RDM_PKG_EXTRACT_ERROR,
    RDM_PKG_VALIDATE_COMPLETE,
    RDM_PKG_VALIDATE_ERROR,
    RDM_PKG_POSTINSTALL_COMPLETE,
    RDM_PKG_POSTINSTALL_ERROR,
    RDM_PKG_UNINSTALL,
    RDM_PKG_INVALID_INPUT
} IARM_RDMMgr_Status_t;
