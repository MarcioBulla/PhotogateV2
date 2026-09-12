# Firmware license

Firmware source code and configuration developed specifically for PhotogateV2
are licensed under the [Apache License 2.0](../LICENSES/Apache-2.0.txt)
(`Apache-2.0`). Copyright (c) Marcio Bulla and PhotogateV2 contributors,
for their original contributions.

The project C/H files with SPDX headers, `main/CMakeLists.txt`,
`main/Kconfig.projbuild`, `main/idf_component.yml` and `sdkconfig.defaults`
are covered. `main/hardware/memory.c` is excluded pending review of the origin
of its ESP-IDF-style NVS initialization snippet. The top-level `CMakeLists.txt`
contains ESP-IDF template boilerplate and is not assigned a new license here.

ESP-IDF, FreeRTOS and downloaded components retain their own licenses and
notices. The project's Apache license does not cover every component linked
into a firmware binary. Do not apply project copyright headers to downloaded
components or generated build files.

`MarcioBulla/menu_manager` is an external Component Manager dependency;
its current upstream license is Apache-2.0. No copy was modified or added.
See [THIRD_PARTY.md](../THIRD_PARTY.md) for version-specific evidence and
[the audit](../LICENSE_AUDIT.md) for unresolved provenance and release checks.
