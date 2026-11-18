# BlastmasterOS Feedback

This directory contains the native Win32 feedback form for the BlastmasterOS client edition.

## Current implementation

- Bug Report and Feature Request categories.
- Summary, environment, reproduction steps, expected behavior, and actual behavior fields.
- Multi-file attachment selection.
- Native Windows controls and a standalone `feedback.exe` application target.

## Submission status

The form is currently a UI implementation only. The Submit Report action explicitly reports that delivery is not configured and does not claim to send anything.

Before enabling submission, configure a secure HTTPS service that:
- validates and rate-limits submissions;
- accepts the report fields and bounded attachment uploads;
- delivers messages to the configured feedback inbox;
- keeps mail-provider credentials on the service, never in this application.

The destination inbox is configured by the service, not embedded as a credential or secret in the client binary.
