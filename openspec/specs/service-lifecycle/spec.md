# Service Lifecycle and Startup

## Purpose
This specification covers the verified startup, initialization, and shutdown behavior of the RDM service and CLI entry flow.

## Requirements

### Requirement: Service activation and lifecycle
The service SHALL activate when the XCONF download URL file changes.
The service SHALL initialize an `RDMHandle` and allocate `pApp_det` when initialization succeeds.
The service SHALL initialize rbus as part of startup and release it during shutdown.
The CLI help flow SHALL expose the supported install modes.
The install package token validation SHALL accept a single `name:version` pattern with numeric dot-version semantics and reject malformed tokens.

#### Scenario: Systemd activation on URL file change
- **WHEN** the systemd path [apps_rdm.path](../../../apps_rdm.path) is configured to watch `/tmp/.xconfssrdownloadurl` and that file changes
- **THEN** the `apps-rdm.service` unit starts `/usr/bin/rdm`.

#### Scenario: Valid service initialization
- **WHEN** a valid `RDMHandle` pointer is provided to `rdmInit` and initialization executes successfully
- **THEN** it allocates `pApp_det`, initializes the structure, and returns `RDM_SUCCESS`.

#### Scenario: Shutdown cleanup
- **WHEN** a valid `RDMHandle` with an initialized rbus connection exists and `rdmUnInit` executes
- **THEN** it calls `rdmRbusUnInit` and frees the app details structure.

#### Scenario: CLI help mode listing
- **WHEN** the help output is invoked through `rdmHelp`
- **THEN** it prints the supported manifest, single-app, USB, versioned-app, broadband, OSS, and help modes.

#### Scenario: Package token validation
- **WHEN** a package token string such as `meminsight:1.0` or `package_name-1:0.8` is validated
- **THEN** the token is accepted only when it uses a single `:` separator and a valid numeric `major.minor` version pattern.
- **WHEN** malformed values such as `meminsight:1`, `meminsight:1.1.1`, `meminsight:.1`, or `app:meminsight:1.0` are validated
- **THEN** they are rejected.

## Implementation evidence
- Systemd activation: [apps_rdm.path](../../../apps_rdm.path), [apps-rdm.service](../../../apps-rdm.service)
- Main process and lifecycle functions: [rdm_main.c](../../../rdm_main.c)
- Data model and handle initialization: [rdm.h](../../../rdm.h)
- Key functions: `rdmInit`, `rdmUnInit`, `rdmHelp`, `rdmParseBundleList`, `rdmIsValidInstallPackageToken`

## External boundaries
- Systemd activation and file-watch behavior occur outside the C runtime but are directly configured in repository files.
- rbus initialization is part of the runtime boundary and is implemented through the RDM service, not the host OS itself.
- Device-specific configuration values and platform state are environment-provided.

## Runtime flow
```mermaid
flowchart TD
    A[/tmp/.xconfssrdownloadurl changed/] --> B[apps_rdm.path]
    B --> C[apps-rdm.service]
    C --> D[/usr/bin/rdm]
    D --> E[rdmInit]
    E --> F[Start rbus + app details]
    F --> G[Service ready for manifest/app workflow]
```
