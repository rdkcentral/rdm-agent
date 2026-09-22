# Capability Index

This index maps each verified runtime capability in the rdm-agent repository to the corresponding OpenSpec baseline specification, source files, key functions, and the major architecture flow it participates in.

## Capability map

| Capability | Specification | Source files | Key functions | Architecture flow |
|---|---|---|---|---|
| Service activation and startup lifecycle | [service-lifecycle/spec.md](service-lifecycle/spec.md) | [apps_rdm.path](../../apps_rdm.path), [apps-rdm.service](../../apps-rdm.service), [rdm_main.c](../../rdm_main.c) | `rdmInit`, `rdmUnInit`, `rdmHelp` | Startup and activation |
| Manifest discovery and metadata lookup | [manifest-discovery/spec.md](manifest-discovery/spec.md) | [src/rdm_jsonquery.c](../../src/rdm_jsonquery.c), [rdm-manifest.json](../../rdm-manifest.json) | `cJSON_SearchFile`, `rdmJSONGetLen`, `rdmJSONQuery` | Manifest-to-download flow |
| Direct package download and transfer | [package-download/spec.md](package-download/spec.md) | [src/rdm_download.c](../../src/rdm_download.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c), [src/rdm_curldownload.c](../../src/rdm_curldownload.c) | `rdmDownloadApp`, `rdmDownloadCheckFs`, `rdmDwnlDirect` | Package download and installation |
| Legacy install pipeline | [package-installation/spec.md](package-installation/spec.md) | [src/rdm_downloadmgr.c](../../src/rdm_downloadmgr.c) | `rdmDownloadMgr`, `rdmDwnlExtract` | Package installation flow |
| Versioned app lifecycle | [versioned-app-management/spec.md](versioned-app-management/spec.md) | [src/rdm_downloadverapp.c](../../src/rdm_downloadverapp.c) | `rdmDownloadVerApp`, `rdmDwnlVAGetFinalVer`, `rdmDwnlVAInstall` | Versioned application flow |
| Plugin package installation | [plugin-package-installation/spec.md](plugin-package-installation/spec.md) | [src/rdm_packagemgr.c](../../src/rdm_packagemgr.c) | `rdmPackageMgr`, `rdmInvokePackage`, `rdmPkgDwnlValidation` | Plugin package manager flow |
| USB installation | [usb-installation/spec.md](usb-installation/spec.md) | [src/rdm_usbinstall.c](../../src/rdm_usbinstall.c) | `rdmUSBInstall` | USB installation flow |
| Security validation | [security-validation/spec.md](security-validation/spec.md) | [src/rdm_openssl.c](../../src/rdm_openssl.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c) | `rdmDwnlValidation`, `rdmOpensslRsafileSignatureVerify` | Security validation boundary |
| rbus/RFC integration and status reporting | [rbus-and-status/spec.md](rbus-and-status/spec.md) | [src/rdm_rbus.c](../../src/rdm_rbus.c), [src/rdm_utils.c](../../src/rdm_utils.c) | `rdmRbusInit`, `rdmRbusGetRfc`, `rdmRbusSetDownloadStatus`, `rdmIARMEvntSendPayload` | Status and rbus integration |
| Persistent state, cleanup, retry and blocked re-download handling | [state-and-cleanup/spec.md](state-and-cleanup/spec.md) | [src/rdm_download.c](../../src/rdm_download.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c) | `rdmDwnlUnInstallApp`, `rdmDwnlCleanUp`, `rdmDwnlIsBlocked`, `rdmRemvDwnlAppInfo` | Persistent state and cleanup flow |

## Coverage note

Each verified production capability has one corresponding specification in this baseline. The mapping is intentionally limited to confirmed repository behavior and excludes specification coverage for tests, future recommendations, and external platform integration not implemented in-repo.
