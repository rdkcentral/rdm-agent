# Architecture Overview

This directory captures the verified As-Is architecture of the rdm-agent repository. It reflects the current runtime structure and implementation boundaries without adding future or speculative behavior.

## Architecture summary

The repository implements a native C service that starts from a systemd path trigger and then performs manifest-driven app discovery, download, validation, and installation. The runtime is organized around a central service entry point plus modular capabilities for metadata lookup, package handling, version selection, USB install, and platform integration.

## Major runtime components

- Entry and lifecycle: [rdm_main.c](../../rdm_main.c), [rdm.h](../../rdm.h)
- JSON manifest discovery: [src/rdm_jsonquery.c](../../src/rdm_jsonquery.c)
- Direct package download and transfer: [src/rdm_download.c](../../src/rdm_download.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c), [src/rdm_curldownload.c](../../src/rdm_curldownload.c)
- Legacy package install pipeline: [src/rdm_downloadmgr.c](../../src/rdm_downloadmgr.c)
- Versioned app management: [src/rdm_downloadverapp.c](../../src/rdm_downloadverapp.c)
- Plugin package manager flow: [src/rdm_packagemgr.c](../../src/rdm_packagemgr.c)
- USB install flow: [src/rdm_usbinstall.c](../../src/rdm_usbinstall.c)
- Security validation: [src/rdm_openssl.c](../../src/rdm_openssl.c)
- rbus and status: [src/rdm_rbus.c](../../src/rdm_rbus.c), [src/rdm_utils.c](../../src/rdm_utils.c)
- Persistence and cleanup: [src/rdm_download.c](../../src/rdm_download.c), [src/rdm_downloadutils.c](../../src/rdm_downloadutils.c)

## Verified runtime flows

- Startup and activation: [00-baseline-architecture.md](00-baseline-architecture.md)
- Package download and installation: [00-baseline-architecture.md](00-baseline-architecture.md)
- Versioned app lifecycle: [00-baseline-architecture.md](00-baseline-architecture.md)
- Plugin package manager flow: [00-baseline-architecture.md](00-baseline-architecture.md)
- USB installation flow: [00-baseline-architecture.md](00-baseline-architecture.md)
- Security validation boundaries: [00-baseline-architecture.md](00-baseline-architecture.md)

## External dependency boundaries

The repository clearly relies on external runtime services and platform resources, including:
- systemd service activation
- rbus and RFC values
- IARM event bus
- OpenSSL key and signature material
- WPE packager / JSON-RPC integration
- device-specific mount and storage paths

These are treated as boundaries, not as repo-owned product requirements.

## Relationship to OpenSpec

The OpenSpec baseline mirrors the repository’s verified runtime implementation. The architecture documentation describes the system shape; the capability specs describe concrete behaviors; the index and parity matrix tie the architecture to the production source evidence.
