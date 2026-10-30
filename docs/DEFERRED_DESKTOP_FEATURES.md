# Deferred desktop features

## Release decision

The current BlastmasterOS release ships with the built-in Classic, non-composited desktop only.

The following work is deferred and must not be presented as an available or supported feature in the current final OS image:

- **DWM compositor:** no compositor process, redirected-window rendering pipeline, or composition effects are included as a release feature.
- **Aero:** no Aero theme, glass frames, composited shadows, or Aero shell presentation is enabled or packaged.
- **Luna:** the Luna visual-style target remains excluded from the build and final image.

## Source preservation

Do not delete, rename, or move the existing source trees, design documents, tests, or resource files for these features. Keep them available for future development.

The existing `dwmapi.dll` compatibility module is distinct from the unfinished DWM compositor. Keep the API compatibility module in the OS system image unless a separate compatibility audit proves it can safely be removed. It must continue to report composition as unavailable; it must not claim that a compositor or Aero rendering is active.

The Luna theme source remains under `media/themes/Luna`, but `media/themes/CMakeLists.txt` must not add that directory to the current build.

Aero design work and future compositor implementation remain documented under `docs/design/BLASTMASTER_AERO_THEME.md` and `docs/design/DWM_COMPOSITOR_ARCHITECTURE.md`.

## Release acceptance checks

- A clean install starts in Classic mode.
- No Aero or Luna theme target is built into the release image.
- No DWM compositor process or composition feature is advertised as available.
- DWM API compatibility calls report composition disabled while preserving ordinary application compatibility.
- The source trees and design documentation for deferred work remain present in the repository.
