# BlastmasterOS User Account Control (UAC)

## Status

**Design / implementation plan only. UAC is not implemented by this document.** Do not advertise UAC as working until the security, process-launch, UI, and test gates below pass.

BlastmasterOS is based on ReactOS and currently targets a legacy Windows NT compatibility baseline. UAC is a cross-cutting security feature; a consent dialog alone is not UAC. Implementation must be integrated with the existing account, access-token, service-control, process-creation, shell, and desktop infrastructure rather than creating a parallel security model.

## Goals

- Make elevation explicit and auditable.
- Preserve least privilege for routine desktop applications.
- Permit authorized users to start selected applications with elevated rights.
- Keep the consent UI separate from the requesting application's UI.
- Provide understandable policy choices and accessible keyboard navigation.
- Keep the Classic desktop usable when advanced desktop composition is unavailable.

## Architecture

### 1. Token and account groundwork

Audit the existing security subsystem before changing it. Document the current support for user/group SIDs, access tokens, privileges, administrator membership, impersonation, and process token assignment. Implement filtered and elevated administrator tokens only where the underlying token APIs can enforce the distinction correctly. A UI-only administrator badge or environment variable is not an authorization boundary.

### 2. Elevation broker

Audit the existing Secondary Logon service (`base/services/seclogon`) and `runas` utility (`base/applications/runas`) first. Reuse and extend the existing service and launch contract where suitable instead of introducing a competing broker. Determine whether a separate AppInfo-style service is needed only after documenting the current service's trust boundary and capabilities. Any elevation broker must authenticate and validate requests, bind them to the requesting process and target executable, reject unsafe caller-supplied token handles/paths/command lines, run with least privilege, and avoid logging secrets.

### 3. Consent UI

Provide a dedicated consent application with clear publisher/application identity, the requested operation, and explicit Allow/Cancel choices. Do not trust display strings supplied by the caller as proof of publisher identity. If publisher verification is unavailable, say so rather than displaying a fabricated trusted-publisher state.

A secure-desktop mode must only be offered once the window station/desktop access controls, input isolation, and desktop switching are verified. Until then, label the mode unavailable; do not describe an ordinary desktop dialog as secure.

### 4. Shell and launch integration

Integrate the broker with the shell's Run as administrator action and the supported runas/elevation launch contract. Ordinary launches must remain ordinary launches. Handle cancellation, invalid targets, broker failure, unavailable credentials, and child-process startup failure without silently retrying with broader privileges.

### 5. Policy and settings

Define policy only for behaviors that are implemented and testable. Initial policy design should cover prompting behavior, standard-user credential prompts where supported, administrator consent, and whether secure-desktop prompting is available. Defaults must fail safely and must not grant elevation because a setting is missing or unreadable.

## User experience

- Default application launch: no prompt unless elevation is requested and required by policy.
- Elevation request: show a clear consent or credential prompt appropriate to the account and policy.
- Cancel: do not launch the elevated process.
- Unknown or unverifiable publisher: clearly identify the publisher as unknown/unverified.
- Broker or security subsystem unavailable: fail closed for elevation and explain the failure.
- Keyboard: predictable focus, accessible labels, and keyboard-only Allow/Cancel operation.
- Classic theme: the prompt remains legible and functional without Aero/DWM composition.

## Implementation order

1. **Audit and tests:** map existing token, account, process creation, service, shell, and desktop-security capabilities. Record gaps before implementation.
2. **Security primitives:** implement or repair the required token and privilege behavior; add tests proving access checks distinguish standard and elevated contexts.
3. **Broker:** add the elevation service and a narrow, validated IPC contract.
4. **Consent UI:** implement the prompt and cancellation/error states; do not claim secure-desktop protection until it is verified.
5. **Shell integration:** wire supported elevation verbs and runas behavior to the broker.
6. **Policy and settings:** expose only tested policy controls.
7. **End-to-end validation:** build and boot the OS, test standard and administrator accounts, process tokens, denied operations, cancellation, service failure, malformed requests, and both Classic and Aero/fallback desktops.

## Release gates

UAC must remain marked **not ready** until all of the following pass:

- An unelevated process cannot gain administrator access merely by requesting it.
- Consent cancellation results in no elevated process.
- A successful elevation receives the intended token and no broader privileges than required.
- Standard-user and administrator flows are tested independently.
- Untrusted or malformed broker requests fail closed.
- The prompt cannot be trivially spoofed by the requesting application's window.
- Secure-desktop claims are backed by verified desktop and input isolation tests.
- No-prompt launches and legacy applications continue to work where expected.
- Debug and release builds compile, and the bootable image is tested in a disposable VM.

## Current limitation

This document defines the intended architecture and acceptance criteria. It does not itself add token filtering, an elevation broker, secure desktop isolation, or a working UAC prompt. Those changes must be implemented in the relevant operating-system subsystems and validated before UAC is described as functional.
