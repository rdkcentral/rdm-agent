# rdm-agent As-Is OpenSpec Baseline

This directory contains the verified As-Is OpenSpec baseline for the rdm-agent repository. It documents only behavior that is directly supported by the production implementation in this codebase and excludes future, proposed, or platform-specific assumptions.

## Purpose

The repository implements a native C service for manifest-driven download, validation, and installation of RDK applications and packages. The As-Is baseline captures the current runtime responsibilities and boundaries without modifying production code.

## Baseline scope

This baseline includes:
- service startup and lifecycle
- manifest discovery and metadata lookup
- direct package download and transfer
- legacy package installation
- versioned app management
- plugin package installation
- USB installation
- signature validation and integrity checks
- rbus/RFC integration and status reporting
- persistent state, cleanup, retry, and block handling

## Documentation structure

- [architecture/README.md](architecture/README.md) — architecture overview and flow references
- [architecture/00-baseline-architecture.md](architecture/00-baseline-architecture.md) — verified component architecture and runtime flows
- [architecture/01-openspec-adoption-workflow.md](architecture/01-openspec-adoption-workflow.md) — baseline maintenance and future change workflow
- [specs/README.md](specs/README.md) — specification organization and navigation
- [specs/00-capability-index.md](specs/00-capability-index.md) — capability-to-spec mapping
- [specs/implementation-parity-matrix.md](specs/implementation-parity-matrix.md) — 1:1 parity between implementation and specs
- [specs/service-lifecycle/spec.md](specs/service-lifecycle/spec.md)
- [specs/manifest-discovery/spec.md](specs/manifest-discovery/spec.md)
- [specs/package-download/spec.md](specs/package-download/spec.md)
- [specs/package-installation/spec.md](specs/package-installation/spec.md)
- [specs/versioned-app-management/spec.md](specs/versioned-app-management/spec.md)
- [specs/plugin-package-installation/spec.md](specs/plugin-package-installation/spec.md)
- [specs/usb-installation/spec.md](specs/usb-installation/spec.md)
- [specs/security-validation/spec.md](specs/security-validation/spec.md)
- [specs/rbus-and-status/spec.md](specs/rbus-and-status/spec.md)
- [specs/state-and-cleanup/spec.md](specs/state-and-cleanup/spec.md)

## Boundary rules

The documented baseline intentionally excludes:
- future or recommended requirements
- unimplemented behavior
- test-only behavior
- assumptions not directly reflected in production code
- platform-dependent behavior that cannot be verified from the repository alone

## Source of truth

The source-of-truth implementation evidence is the production code in the repository root and its `src/`, `include/`, and `apps*` files. The OpenSpec baseline is a repository-scoped documentation of those verified runtime capabilities.
