# Specification Organization

This directory contains the verified As-Is specification baseline for the rdm-agent repository. The organization is intentionally aligned with the repository’s actual runtime implementation, not with future design abstractions.

## Navigation

- [00-capability-index.md](00-capability-index.md) — index of all verified production capabilities
- [implementation-parity-matrix.md](implementation-parity-matrix.md) — 1:1 parity mapping between implementation and specs
- [service-lifecycle/spec.md](service-lifecycle/spec.md) — startup, lifecycle, and CLI validation
- [manifest-discovery/spec.md](manifest-discovery/spec.md) — JSON manifest reading and metadata query behavior
- [package-download/spec.md](package-download/spec.md) — download URL resolution and transfer flow
- [package-installation/spec.md](package-installation/spec.md) — legacy package extraction and install path
- [versioned-app-management/spec.md](versioned-app-management/spec.md) — version selection and cleanup behavior
- [plugin-package-installation/spec.md](plugin-package-installation/spec.md) — plugin install and packager validation path
- [usb-installation/spec.md](usb-installation/spec.md) — USB scan and matching flow
- [security-validation/spec.md](security-validation/spec.md) — signature verification and package integrity checks
- [rbus-and-status/spec.md](rbus-and-status/spec.md) — rbus/RFC status and IARM notifications
- [state-and-cleanup/spec.md](state-and-cleanup/spec.md) — persistent state, retries, cleanup, and blocking

## Baseline conventions

Each spec follows the same As-Is structure:
- scope
- implementation evidence
- external boundaries
- requirements
- GIVEN/WHEN/THEN scenarios
- runtime flow diagram where applicable

## What is intentionally excluded

The baseline intentionally excludes:
- future or planned behavior
- proposed change requirements
- test-only functionality presented as runtime capability
- placeholder or environment-dependent logic not implemented in the repo itself
- assumed behaviors that cannot be traced to production code

## Source of truth

The repository code under the project root is the authoritative source of truth for the baseline. The OpenSpec documents are a structured representation of that verified implementation.
