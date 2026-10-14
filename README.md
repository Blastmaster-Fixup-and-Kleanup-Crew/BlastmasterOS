<p align="center">
  <strong>BlastmasterOS</strong><br>
  <em>A work-in-progress desktop operating system</em>
</p>

---

## About BlastmasterOS

**BlastmasterOS is an experimental operating system project based on the ReactOS codebase.** Its goal is to provide a familiar desktop environment with compatibility for software and drivers designed for the Windows NT family.

BlastmasterOS is under active development. Features, device support, installation, and application compatibility may be incomplete or unreliable.

> [!WARNING]
> **ALPHA-QUALITY SOFTWARE — USE AT YOUR OWN RISK.** Test BlastmasterOS in a virtual machine with disposable storage. Do not install it on a computer or disk containing important data. Crashes, data loss, missing drivers, and non-working features are possible.

## Current project direction

- Establish consistent BlastmasterOS product branding.
- Develop the desktop appearance, including the planned Aero-inspired theme and a reliable Classic/opaque fallback.
- Improve boot, installation, and hardware reliability.
- Validate builds and bootable ISO images before release.

These are project goals, not a claim that every item is already implemented or working.

## Runtime system requirements (provisional)

BlastmasterOS does not yet have a tested, officially guaranteed minimum configuration. The figures below are **development targets for planning and virtual-machine testing**, not proof that every configuration boots or that Aero composition is implemented.

| Component | Baseline / fallback desktop test target | Aero development target |
|---|---|---|
| Processor | x86-compatible CPU; 1 virtual CPU for basic smoke tests | 2 virtual CPU cores recommended |
| Memory | 1 GB for basic boot experiments (may be insufficient) | **2 GB minimum target; 4 GB recommended** |
| Storage | 10 GB virtual disk for initial experiments | **20 GB minimum target; 32 GB recommended** to leave room for updates, applications, and test snapshots |
| Display | VGA-compatible virtual adapter | **1024 × 768 or higher**; test 1280 × 720 as well |
| Graphics | Adapter supported by the current build | A working display driver and a future verified composition-capable rendering path; hardware acceleration is not currently guaranteed |
| Firmware / boot | Legacy BIOS-compatible virtual machine is the conservative starting point | Same baseline until UEFI boot is validated for the target build |
| Input | Standard keyboard and mouse | Same, with keyboard-only navigation included in theme testing |

### Recommended VM profile for Aero development

- 2 virtual CPU cores
- 4 GB RAM
- 32 GB disposable virtual disk
- VGA-compatible virtual graphics adapter, using the VM's supported display settings
- 1024 × 768 baseline resolution, followed by 1280 × 720
- A snapshot before installation and before theme/compositor changes

**Important:** Raising memory and disk allocations does not enable Aero by itself. The current DWM code is an API shim and the real compositor, redirected-window surfaces, and glass-frame rendering still need implementation and validation. Until then, use the Classic/opaque fallback. Do not interpret these targets as confirmed minimum requirements or a promise of hardware compatibility. See the [Aero theme design and release gates](docs/design/BLASTMASTER_AERO_THEME.md).

---

## Build

BlastmasterOS uses the CMake-based ReactOS build system and supports Ninja-based build configurations. The repository includes CMake presets and toolchain files.

### Prerequisites

Use a supported compiler toolchain for the target platform, along with CMake, Ninja, and the additional build tools required by the project. The upstream ReactOS build documentation is a useful reference for toolchain setup.

- [CMake](https://cmake.org/)
- [Ninja](https://ninja-build.org/)
- [ReactOS build documentation](https://reactos.org/wiki/Building_ReactOS)

### Configure and build

From the repository root, inspect the available presets first:

```sh
cmake --list-presets
```

Choose an appropriate preset listed by that command, then configure and build it. For example, a preset named `mingw-i386-debug` can be used as follows:

```sh
cmake --preset mingw-i386-debug
cmake --build output-MinGW-i386
```

Preset names and build directories can change; use the names actually listed in the checked-out `CMakePresets.json`. A full build may require platform-specific dependencies and considerable disk space.

### Build a bootable ISO

After a successful configuration, generate the bootable CD image with the project's `bootcd` target. For a build directory named `output-MinGW-i386`, for example:

```sh
cmake --build output-MinGW-i386 --target bootcd
```

The build output location and ISO filename depend on the selected configuration. Confirm the actual artifact path in the build output before distributing it. A generated ISO should be boot-tested in a virtual machine before being treated as release-ready.

## Installation and testing

Installation and hardware support are experimental. Use a virtual machine first, take snapshots where possible, and use a virtual disk that contains no important data. Never assume an installer operation is safe for a physical disk without carefully verifying the target.

When reporting a problem, include:

- The BlastmasterOS commit or build identifier.
- The target architecture and virtual-machine or hardware configuration.
- The exact steps that reproduce the issue.
- Relevant build output, logs, or screenshots.

## Branding and upstream acknowledgement

BlastmasterOS is based on the [ReactOS project](https://github.com/reactos/reactos). The upstream project's code and third-party components remain subject to their respective licenses and notices. Consult the repository's `COPYING`, `CREDITS`, and component-level license files before redistributing modified binaries or source.

BlastmasterOS is an independent project; it is not an official ReactOS release. Product branding does not change the licenses or attribution obligations of the underlying code.

## Contributing

Contributions that improve reliability, compatibility, documentation, accessibility, and the BlastmasterOS experience are welcome. Keep changes focused, describe how they were tested, and clearly identify features that have not yet been validated.

For upstream project background and build-system details, see the [ReactOS project](https://reactos.org/) and its [build guide](https://reactos.org/wiki/Building_ReactOS).
