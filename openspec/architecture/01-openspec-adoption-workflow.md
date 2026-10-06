# OpenSpec Adoption Workflow

## Purpose

This document defines how the As-Is baseline in this repository is maintained as a stable reference and how future changes should be managed through the OpenSpec lifecycle without altering the verified baseline.

## How the As-Is baseline is maintained

The baseline is created from the verified production implementation and is intentionally limited to behavior that already exists in the repository. The baseline should be treated as the canonical As-Is description for the current runtime behavior of rdm-agent.

The baseline is maintained by:
- validating each capability against production source files and key functions
- excluding behavior that is speculative, external, or future-oriented
- keeping the baseline stable while product requirements or design changes are proposed separately
- updating only the OpenSpec baseline artifacts under `openspec/` when the implementation changes and the new behavior is confirmed in production code

This directory is not used for active development deltas. It is a baseline and reference package for the current implementation.

## Relationship between production code, baseline specs, and delta specs

### Production code
The production code in the repository root and under `src/`, `include/`, and the top-level service files is the source of truth for current runtime behavior.

### Baseline specs
The files under [specs](../specs/README.md) represent the verified As-Is implementation and describe the current behavior in a structured form. They are not design proposals and do not describe future targets.

### Delta specs
Active change work belongs in `openspec/changes/` as future delta specifications, created only when a new change is intentionally introduced. This directory is reserved for active work, not for the stable repository baseline.

## Future change workflow

When an intentional change is required, the expected OpenSpec workflow is:

1. Create a change proposal through the OpenSpec change workflow.
2. Capture the delta requirement in `openspec/changes/` as an active change artifact.
3. Apply the delta against the verified As-Is baseline while preserving the runtime evidence boundary.
4. Archive the change when completed and the delta is no longer active.

This process keeps the baseline stable and prevents future change artifacts from being mistaken for the current implementation state.

## How future changes should use propose, apply, and archive

### Propose
Use the propose workflow when a real change is identified. The proposal must be grounded in the current As-Is baseline and should describe the intended change in terms of product behavior, not repository assumptions.

### Apply
Use the apply workflow when the proposed change is approved and the change artifacts are being implemented. The change should be tracked relative to the baseline and should not rewrite the verified As-Is definition unless the underlying production implementation has legitimately changed and is confirmed.

### Archive
When the change has been completed and is no longer active, archive it to preserve the historical record. This keeps the baseline stable and the change history explicit.

## Baseline maintenance rule

The As-Is baseline is the repository’s current runtime truth. It remains unchanged unless the underlying repository code is verified to have changed. A future proposal must not be mixed into the baseline unless the production repository itself has been updated and confirmed.

## Practical guidance

- Use the baseline specs as evidence for what exists today.
- Use delta specs in `openspec/changes/` for active work only.
- Do not treat tests or platform stubs as product requirements.
- Do not add future behavior to the baseline documentation.
- Document only verified runtime behavior and external boundaries.
