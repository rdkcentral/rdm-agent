# Service Lifecycle and Startup

## Scope
This specification covers the verified startup, initialization, and shutdown behavior of the RDM service and CLI entry flow.

## Implementation evidence
- Systemd activation: [apps_rdm.path](../../../apps_rdm.path), [apps-rdm.service](../../../apps-rdm.service)
- Main process and lifecycle functions: [rdm_main.c](../../../rdm_main.c)
- Data model and handle initialization: [rdm.h](../../../rdm.h)
- Key functions: `rdmInit`, `rdmUnInit`, `rdmHelp`, `rdmParseBundleList`, `rdmIsValidInstallPackageToken`

## External boundaries
- Systemd activation and file-watch behavior occur outside the C runtime but are directly configured in repository files.
- rbus initialization is part of the runtime boundary and is implemented through the RDM service, not the host OS itself.
- Device-specific configuration values and platform state are environment-provided.

## Requirements
1. The service must activate when the XCONF download URL file changes.
2. The service must initialize an `RDMHandle` and allocate `pApp_det` when initialization succeeds.
3. The service must initialize rbus as part of startup and release it during shutdown.
4. The CLI help flow must expose the supported install modes.
5. The install package token validation must accept a single `name:version` pattern with numeric dot-version semantics and reject malformed tokens.

## GIVEN / WHEN / THEN scenarios
### Requirement 1
GIVEN the systemd path [apps_rdm.path](../../../apps_rdm.path) is configured to watch `/tmp/.xconfssrdownloadurl`
WHEN that file changes
THEN the `apps-rdm.service` unit starts `/usr/bin/rdm`.

### Requirement 2
GIVEN a valid `RDMHandle` pointer is provided to `rdmInit`
WHEN initialization executes successfully
THEN it allocates `pApp_det`, initializes the structure, and returns `RDM_SUCCESS`.

### Requirement 3
GIVEN a valid `RDMHandle` with an initialized rbus connection exists
WHEN `rdmUnInit` executes
THEN it calls `rdmRbusUnInit` and frees the app details structure.

### Requirement 4
GIVEN the help output is invoked through `rdmHelp`
WHEN the function runs
THEN it prints the supported manifest, single-app, USB, versioned-app, broadband, OSS, and help modes.

### Requirement 5
GIVEN a package token string such as `meminsight:1.0` or `package_name-1:0.8`
WHEN `rdmIsValidInstallPackageToken` validates it
THEN the token is accepted only when it uses a single `:` separator and a valid numeric `major.minor` version pattern.

GIVEN malformed values such as `meminsight:1`, `meminsight:1.1.1`, `meminsight:.1`, or `app:meminsight:1.0`
WHEN validation runs
THEN they are rejected.

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
