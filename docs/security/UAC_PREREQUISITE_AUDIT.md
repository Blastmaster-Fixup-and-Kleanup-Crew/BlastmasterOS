# BlastmasterOS UAC Prerequisite Audit

Audit date: 2026-10-14
Branch inspected: `master`
Repository: `Blastmaster-Fixup-and-Kleanup-Crew/BlastmasterOS`

## Scope and confidence

This is a source-level audit of selected files, not a successful build, runtime security assessment, or proof of exploitability. Findings describe the inspected source paths. End-to-end UAC behavior still requires tracing callers and validating behavior in a booted VM.

## Findings

### Critical — Secondary Logon logs the supplied password

File: `base/services/seclogon/rpcserver.c`

The request tracing block in `SeclCreateProcessWithLogonW` includes a `TRACE` statement that formats `pRequest->Password`. This can disclose credentials in debug/trace logs when the relevant logging is enabled.

**Required fix:** remove password-value logging completely; log only a non-sensitive event and result code. Review all credential-handling logs for passwords, reusable secrets, or sensitive environment data. Add a regression check to ensure the password field is never formatted into logs.

### High — Token filtering exists, but UAC token semantics are not established

Files: `ntoskrnl/se/token.c`, `modules/rostests/kmtests/ntos_se/SeTokenFiltering.c`, `modules/rostests/apitests/ntdll/NtFilterToken.c`

The kernel has token implementation infrastructure, and existing tests exercise token filtering, including disabled/deleted privileges and restricted groups. Those tests do not by themselves prove Windows-style split administrator tokens, linked-token semantics, integrity levels, or correct consent policy. The precise kernel-side filter implementation and token-information behavior must be traced before relying on them for UAC.

**Required investigation:** trace `NtFilterToken` into the kernel service and `SeFilterToken` implementation; inspect token group attributes, privilege handling, token type/level information, and whether linked-token/elevation information is implemented. Add tests for each supported behavior and explicit expected results for unsupported ones.

### High — Process creation accepts a supplied token

File: `dll/win32/kernelbase/wine/process.c`

`CreateProcessAsUserW` delegates to `CreateProcessInternalW`, and `create_nt_process` passes a non-null token as `PS_ATTRIBUTE_TOKEN`. This is an expected process-creation mechanism, but its security depends on the native kernel's handle access checks, token validation, caller privileges, and broker authorization.

**Required investigation:** trace native process creation and verify the required access rights/privileges are enforced. Test that an unprivileged caller cannot acquire or substitute a privileged token and cannot bypass authorization by invoking token-based process APIs directly.

### High — Shell `runas` is not proof of UAC

File: `dll/win32/shell32/shlexec.cpp`

The shell recognizes the `runas` verb and routes it to `PromptAndRunProcessAs`. The inspected implementation includes a separate-user credential dialog and uses `CreateProcessWithLogonW`. That supports alternate-credential launching, but it is not equivalent to an administrator-consent flow using a filtered/linked administrator token, nor does it prove a protected consent desktop.

**Required investigation:** trace the full `runas` call path and supported shell verbs. Keep alternate-user credentials separate from administrator consent; define explicit behavior for standard users, administrators, cancel, and service failure.

### High — Secondary Logon is a credential-based launch service

Files: `base/services/seclogon/seclogon.c`, `base/services/seclogon/rpcserver.c`

The service starts an RPC server. Its `SeclCreateProcessWithLogonW` path calls `LogonUser`, optionally loads a profile, then calls `CreateProcessAsUserW`. This is a useful existing component to audit and potentially reuse, but the inspected path does not establish a complete consent broker, secure desktop, or split-token UAC model.

**Required investigation:** validate RPC caller authentication and authorization, request marshalling, target path/command handling, environment handling, profile cleanup, process-creation flags, and all failure paths. Ensure no secret is logged.

## Suggested implementation sequence

1. Remove credential logging from Secondary Logon and scan nearby code for additional secret leakage.
2. Trace the kernel implementation of `NtFilterToken` / `SeFilterToken` and document the token information classes and semantics currently supported.
3. Trace native process creation, token handle validation, and privilege checks; add negative tests for unauthorized token use.
4. Audit the full Secondary Logon RPC trust boundary and its cleanup/error paths.
5. Audit shell `runas` and alternate-credential behavior, then specify the distinct administrator-consent flow.
6. Only after the prerequisites are demonstrated, implement the consent broker/UI and secure-desktop isolation.

## Release status

**UAC is not ready and must not be represented as implemented.** This audit found reusable components and at least one credential-logging defect. It does not make code changes to the security implementation and does not claim a build or runtime test has passed.