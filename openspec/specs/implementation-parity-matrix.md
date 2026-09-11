# Implementation Parity Matrix

This matrix demonstrates the 1:1 relationship between the verified runtime implementation and the As-Is OpenSpec baseline. It maps each production capability directly to one specification and one or more source locations.

## Parity matrix

| Implementation capability | Verified source evidence | Baseline spec |
|---|---|---|
| Service activation and application lifecycle | [apps_rdm.path](../../apps_rdm.path), [apps-rdm.service](../../apps-rdm.service), [rdm_main.c](../../rdm_main.c) | [service-lifecycle/spec.md](service-lifecycle/spec.md) |
| Manifest parsing and metadata lookup | [src/rdm_jsonquery.c](../../src/rdm_jsonquery.c), [rdm-manifest.json](../../rdm-manifest.json) | [manifest-discovery/spec.md](manifest-discovery/spec.md) |
| Direct file download and transfer | [src/rdm_download.c](../../src/rdm_download.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c), [src/rdm_curldownload.c](../../src/rdm_curldownload.c) | [package-download/spec.md](package-download/spec.md) |
| Legacy extract-and-install flow | [src/rdm_downloadmgr.c](../../src/rdm_downloadmgr.c) | [package-installation/spec.md](package-installation/spec.md) |
| Versioned app handling | [src/rdm_downloadverapp.c](../../src/rdm_downloadverapp.c) | [versioned-app-management/spec.md](versioned-app-management/spec.md) |
| Plugin package manager path | [src/rdm_packagemgr.c](../../src/rdm_packagemgr.c) | [plugin-package-installation/spec.md](plugin-package-installation/spec.md) |
| USB installation path | [src/rdm_usbinstall.c](../../src/rdm_usbinstall.c) | [usb-installation/spec.md](usb-installation/spec.md) |
| Security verification | [src/rdm_openssl.c](../../src/rdm_openssl.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c) | [security-validation/spec.md](security-validation/spec.md) |
| rbus and RFC status integration | [src/rdm_rbus.c](../../src/rdm_rbus.c), [src/rdm_utils.c](../../src/rdm_utils.c) | [rbus-and-status/spec.md](rbus-and-status/spec.md) |
| Persistent state, cleanup, retry, and block handling | [src/rdm_download.c](../../src/rdm_download.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c), [rdm.h](../../rdm.h) | [state-and-cleanup/spec.md](state-and-cleanup/spec.md) |

## Parity principle

For this baseline, each production capability has a corresponding spec and a single, clearly identified architecture flow. No capability is documented in multiple unrelated sections, and no runtime capability is left undocumented.

## Exclusion note

The parity matrix excludes behaviors that are not fully implemented in the repository or are platform external, such as:
- external packager internals
- vendor trust-provider semantics
- non-repo key storage and secure provisioning
- future or unimplemented features
- test-defined behavior not present as runtime capability
