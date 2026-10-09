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
- Develop the classic Luna-inspired desktop appearance.
- Improve boot, installation, and hardware reliability.
- Validate builds and bootable ISO images before release.

These are project goals, not a claim that every item is already implemented or working.

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
