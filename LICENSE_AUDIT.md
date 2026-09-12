# Licensing audit

Audit date: 2026-09-12. Baseline: `d2b286c`. This report records the local
inspection and the limited licensing changes; it is not a certification.

## Scope and evidence

All 136 tracked paths were inventoried before edits: 70 in `boards/`, 1 in
`3DModels/`, 32 in `firmware/`, 30 in `docs/`, and 3 at the root. Inspection
included Git history, dependency manifests/CMake files, source/header notices,
STEP/WRL headers, embedded KiCad library identifiers, the FreeCAD archive's
Document.xml, the supplier database schema, generated HTML notices, and the
image library's source note. Git history credits Marcio Bulla for the application;
project-specific application structure and the owner's licensing instruction
support the original-code classification. History alone is not proof of authorship
of imported assets, and this is not an exhaustive plagiarism or binary provenance
analysis.

The full directory structure was inspected, including ignored backups and the
separate `PhotogateV2_materials/` checkout. That untracked tree includes duplicate
project files, `3D_Model/`, `Codigo_fonte/`, `placa/`, a nested `.git`, build output,
a lockfile, caches and five managed components. Its manifests, lockfile and
component licenses supplied supplemental evidence; its generated binaries and
all historical/source assets have not been individually cleared. It remains
outside the new grants and was not modified. No graph existed and no graph
artifacts were added.

Pre-existing working-tree state was `M 3DModels/All_Models.FCStd` and
`?? PhotogateV2_materials/`. The model's pre-edit bytes were preserved, as were
all existing board, website asset and third-party files. No existing license or
copyright notice was removed. No code behavior, dependency version, build
setting, schematic, PCB or model was changed.

## License decisions

- Original electronic circuitry, PCB design and manufacturing contributions:
  CERN-OHL-W-2.0, subject to embedded-library/artwork exclusions.
- Original mechanical design contributions: CERN-OHL-W-2.0, subject to
  unresolved imported-object/artwork exclusions within the assembly.
- Identified original firmware: Apache-2.0; 26 C/H files received SPDX headers.
- Original documentation text/diagrams and website implementation:
  CC-BY-SA-4.0; images and mixed generated exports are explicitly excluded.
- Third-party files retain their existing terms. A pending designation does not
  assert that a file is proprietary; it means permission was not established.

## Manual review before publication

1. **24 component-model files lack an explicit license:** the inventory below
   lists all of them. Exporter names or similarity to KiCad library names are
   insufficient evidence. The other 28 files have explicit notices: 14
   CC-BY-SA-4.0 plus exception and 14 GPL-3.0-or-later plus exception. Preserve
   their exact notices and obtain/ship any additional texts required by their
   licenses when preparing a distribution.
2. **Embedded PCB/schematic material:** resolve EasyEDA footprint provenance,
   `LOGO`, the `kibuzzard-*` font-derived artwork, and library versions. Original
   circuit/layout rights are granted; imported definitions and their copies in
   fabrication exports are not relicensed. `boards/jlcpcb/project.db` contains
   supplier fields (including stock), so no new license is assigned to it.
3. **FreeCAD assembly:** `All_Models.FCStd` has blank creator fields and the
   document property `License = All rights reserved`. Reconcile it manually
   with the original-contributions CERN grant without losing the existing model
   edits. Verify `Screw`, `Nut`, sensor representations, `Sketch050`/`LorxiSymbol`
   and font-derived shapes before licensing the assembly as a whole. The same
   issue affects `docs/3dmodels/board.glb`, `case.gltf` and `case.bin`.
4. **Images:** `docs/icon.png`, `docs/images/FabianoYokaichiyaSeal.png` and all
   `docs/images/project/` image files remain unlicensed by this change.
   The project image README documents copying/renaming from the materials
   folder, but does not identify each original creator/license. Confirm photos,
   screenshots, timing diagrams, schematic exports and any incorporated artwork
   individually; a screenshot's tool is not its copyright owner by default.
5. **Firmware snippet/template:** `firmware/main/hardware/memory.c` follows the
   common ESP-IDF NVS initialization/recovery pattern. Its authorship or precise
   example origin remains uncertain; no new header/license was assigned. This
   is a conservative provenance flag, not a finding that copying occurred.
   `firmware/CMakeLists.txt` visibly contains ESP-IDF template instructions;
   `.gitignore` includes SDK-wide template rules. Their exact upstream origins
   were not established. `.gitattributes` was left outside the scoped grants.
6. **Generated website and external assets:** reconcile the IBOM generator and
   bundled Split.js/lz-string/PEP versions and complete notices. Verify CDN
   scripts, fonts and remote logo terms. See THIRD_PARTY.md; the viewer and
   external assets were not assigned the documentation license.
7. **Historical references/dependencies:** resolve the README's three Python
   library credits and any inherited implementation. `menu_manager` is
   Apache-2.0 in current Git upstream, but the old managed snapshot has no
   license file and a registry README contains a conflicting historical MIT
   statement. Align the exact distributed version with its license evidence.

## OSHWA / HardwareX readiness

The three intended project licenses are now documented. CERN-OHL-W-2.0 is among
[OSHWA's recommended hardware licenses](https://certification.oshwa.org/process/hardware.html).
Licensing files alone do not establish certification: resolve material essential
to reproducing the device, provide accessible editable source and clearly identify
excluded material. See the [OSHWA requirements](https://certification.oshwa.org/requirements.html)
and [documentation guidance](https://certification.oshwa.org/process/documentation.html).
No certification status or certification identifier is claimed.

For HardwareX, check that the final deposit includes the editable sources,
manufacturing files, BOM, assembly/build instructions and evidence needed to
reproduce the reported hardware, with licenses/attribution matching those files.
The [journal's manuscript template](https://zenodo.org/records/5078227/files/HardwareXTemplate_20210707.pdf)
emphasizes reproducibility and open licensing. Its current
[Guide for Authors](https://www.sciencedirect.com/journal/hardwarex/publish/guide-for-authors)
could not be retrieved during this audit; confirm current submission requirements
before submission. This audit does not certify manuscript or experimental adequacy.

The root has no resolved lockfile; Git dependencies follow `main`. The existing
README references ESP-IDF 5.4.2, its requirement section says 5.4, the manifest
permits >=4.1.0, and the separate materials lock resolves 5.4.4. Record the actual
release build versions and dependency notices. The README build command also
omits entering `firmware/`; this existing reproducibility issue was not changed
in a licensing-only pass. The firmware component CMake list names `core/cronos`
without `.c`; build correctness was not assessed or changed.

The supplied DOI was added to the README. Zenodo metadata and deposited content
were not changed or verified against the working tree; ensure the eventual
archived version includes these notices. Commit, tag, push and release operations
remain with the owner.

REUSE annotations are deliberately partial: unresolved/mixed files were not
blanket-annotated. The `reuse` executable is unavailable in this environment,
so no `reuse lint` result or full REUSE compliance is claimed. Resolve the listed
provenance gaps and add accurate per-file/sidecar metadata and third-party license
texts before expecting a clean full-tree REUSE check.

## Complete tracked-file inventory before edits

“Original” below describes project contributions, not a transfer of embedded
third-party rights. Existing third-party notices always remain effective.

| File | Classification / licensing disposition |
| --- | --- |
| `.gitattributes` | Repository configuration/template: no new license assigned; unchanged |
| `.gitignore` | Repository configuration/template: no new license assigned; unchanged |
| `3DModels/All_Models.FCStd` | Mixed assembly: original contributions CERN-OHL-W-2.0; imported objects/metadata pending; unchanged |
| `README.md` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `boards/board.kicad_pcb` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/board.kicad_pro` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/board.kicad_sch` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-CuBottom.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-CuTop.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-EdgeCuts.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-MaskBottom.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-MaskTop.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-NPTH-drl_map.pdf` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-NPTH.drl` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-PTH-drl_map.pdf` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-PTH.drl` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-SilkBottom.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/gerber/board-SilkTop.gbr` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/production_files/BOM-board.csv` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/production_files/CPL-board.csv` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/production_files/GERBER-board.zip` | Original electronic design/manufacturing contribution: CERN-OHL-W-2.0; imported material excluded |
| `boards/jlcpcb/project.db` | Pending: project/supplier database; excluded |
| `boards/packages3D/BOURNS-3296Y.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/BOURNS-3296Y.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/CAP-TH_D4.0-H7.0-P1.5.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/CAP-TH_D4.0-H7.0-P1.5.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/CAP-TH_D5.0×H7.0×P2.0.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/CAP-TH_D5.0×H7.0×P2.0.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/CP_Radial_D5.0mm_P2.50mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/CP_Radial_D5.0mm_P2.50mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/CP_Radial_Tantal_D5.5mm_P2.50mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/CP_Radial_Tantal_D5.5mm_P2.50mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/CP_Radial_Tantal_D6.0mm_P2.50mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/CP_Radial_Tantal_D6.0mm_P2.50mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/C_Disc_D5.0mm_W2.5mm_P5.00mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/C_Disc_D5.0mm_W2.5mm_P5.00mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/C_Disc_D6.0mm_W2.5mm_P5.00mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/C_Disc_D6.0mm_W2.5mm_P5.00mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/C_Rect_L7.0mm_W2.5mm_P5.00mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/C_Rect_L7.0mm_W2.5mm_P5.00mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/DIP-8_W7.62mm.step` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/DIP-8_W7.62mm.wrl` | Third party: existing GPL-3.0-or-later + embedded special exception; unchanged |
| `boards/packages3D/DO-41_D2.4-L4.7-P8.70.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/DO-41_D2.4-L4.7-P8.70.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/D_DO-35_SOD27_P7.62mm_Horizontal.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/D_DO-35_SOD27_P7.62mm_Horizontal.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/D_DO-41_SOD81_P10.16mm_Horizontal.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/D_DO-41_SOD81_P10.16mm_Horizontal.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/ESP32-WROOM-32.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/ESP32-WROOM-32.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/IDC-Header_2x03_P2.54mm_Vertical.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/IDC-Header_2x03_P2.54mm_Vertical.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/IDC-Header_2x05_P2.54mm_Vertical.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/IDC-Header_2x05_P2.54mm_Vertical.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-02A_1x02_P2.54mm_Vertical.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-03A_1x03_P2.54mm_Vertical.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-03A_1x03_P2.54mm_Vertical.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-05A_1x05_P2.54mm_Vertical.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-05A_1x05_P2.54mm_Vertical.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-06A_1x06_P2.54mm_Vertical.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/Molex_KK-254_AE-6410-06A_1x06_P2.54mm_Vertical.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/RES-TH_BD3.9-L9.0-P16.00-D0.6.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/RES-TH_BD3.9-L9.0-P16.00-D0.6.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/RES-TH_D2.3-L6.5-P10.50.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/RES-TH_D2.3-L6.5-P10.50.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal.step` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal.wrl` | Third party: existing CC-BY-SA-4.0 + KiCad library exception; unchanged |
| `boards/packages3D/TO-220-3_Vertical.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/TO-220-3_Vertical.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/TO-92-3_L4.8-W3.7-H7.7-P2.54-R.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/TO-92-3_L4.8-W3.7-H7.7-P2.54-R.wrl` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/TO-92_Inline.step` | Pending: component geometry with no explicit license; excluded |
| `boards/packages3D/TO-92_Inline.wrl` | Pending: component geometry with no explicit license; excluded |
| `docs/3dmodels/board.glb` | Mixed design export: original design rights CERN-OHL-W-2.0; imported geometry/artwork pending |
| `docs/3dmodels/case.bin` | Mixed design export: original design rights CERN-OHL-W-2.0; imported geometry/artwork pending |
| `docs/3dmodels/case.gltf` | Mixed design export: original design rights CERN-OHL-W-2.0; imported geometry/artwork pending |
| `docs/CNAME` | Deployment configuration: outside content grant; unchanged |
| `docs/app.js` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `docs/hardware.html` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `docs/ibom.html` | Mixed generated hardware data and third-party viewer; no whole-file license assigned |
| `docs/icon.png` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/FabianoYokaichiyaSeal.png` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/README.md` | Original documentation: CC-BY-SA-4.0 |
| `docs/images/project/board/pcb-back.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/board/pcb-front.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/board/pcb-layout-back.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/board/pcb-layout-front.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/gate-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/generic-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/hollow-cylinder-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/pendulum-centripetal-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/pendulum-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/solid-cylinder-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/experiments/spring-pulse.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/firmware/menuconfig-experiments.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/firmware/menuconfig-main.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/mechanical/freecad-assembly.jpeg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/schematics/complete-schematic.svg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/images/project/signals/sensor-hysteresis.jpg` | Pending image authorship/incorporated material; excluded, unchanged |
| `docs/index.html` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `docs/modeling.html` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `docs/software.html` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `docs/styles.css` | Original text/diagrams or website implementation: CC-BY-SA-4.0; external assets excluded |
| `firmware/CMakeLists.txt` | ESP-IDF template boilerplate: upstream provenance/license review; unchanged |
| `firmware/main/CMakeLists.txt` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/Kconfig.projbuild` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/cronos.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/cronos.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/history.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/history.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/menus.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/menus.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/misc.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/core/misc.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/centripetal.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/centripetal.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/classic.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/classic.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/mechanical.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/mechanical.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/pendulum.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/pendulum.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/spring.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/experiments/spring.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/counter.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/counter.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/display.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/display.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/encoder.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/encoder.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/hardware/memory.c` | Pending: ESP-IDF-style NVS snippet provenance; excluded, unchanged |
| `firmware/main/hardware/memory.h` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/idf_component.yml` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/main/main.c` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |
| `firmware/sdkconfig.defaults` | Original firmware: Apache-2.0; SPDX header for C/H, REUSE annotation for configuration |

## Files created by this change

- `LICENSES/Apache-2.0.txt`
- `LICENSES/CERN-OHL-W-2.0.txt`
- `LICENSES/CC-BY-SA-4.0.txt`
- `LICENSES.md`
- `LICENSE_AUDIT.md`
- `THIRD_PARTY.md`
- `REUSE.toml`
- `boards/LICENSE.md`
- `3DModels/LICENSE.md`
- `firmware/LICENSE.md`
- `docs/LICENSE.md`

## Existing files modified by this change

- `README.md`
- `firmware/main/core/cronos.c`
- `firmware/main/core/cronos.h`
- `firmware/main/core/history.c`
- `firmware/main/core/history.h`
- `firmware/main/core/menus.c`
- `firmware/main/core/menus.h`
- `firmware/main/core/misc.c`
- `firmware/main/core/misc.h`
- `firmware/main/experiments/centripetal.c`
- `firmware/main/experiments/centripetal.h`
- `firmware/main/experiments/classic.c`
- `firmware/main/experiments/classic.h`
- `firmware/main/experiments/mechanical.c`
- `firmware/main/experiments/mechanical.h`
- `firmware/main/experiments/pendulum.c`
- `firmware/main/experiments/pendulum.h`
- `firmware/main/experiments/spring.c`
- `firmware/main/experiments/spring.h`
- `firmware/main/hardware/counter.c`
- `firmware/main/hardware/counter.h`
- `firmware/main/hardware/display.c`
- `firmware/main/hardware/display.h`
- `firmware/main/hardware/encoder.c`
- `firmware/main/hardware/encoder.h`
- `firmware/main/hardware/memory.h`
- `firmware/main/main.c`

## Verification

- All 26 source/header edits consist only of a three-line SPDX comment prefix;
  the remaining bytes match Git HEAD exactly.
- Pre-edit SHA-256 checks of all 136 tracked files establish that every other
  pre-existing file except README is unchanged, including the already-modified
  FreeCAD assembly. Existing copyright and license notices are intact.
- The three license files match their downloaded source texts byte for byte.
- REUSE.toml parses as TOML and all annotation paths exist; local Markdown links
  in the newly written licensing documents resolve.
- `git diff --check` passes. No firmware build was run because code behavior and
  build inputs were unchanged; this makes no claim about existing build defects.
- No commit, tag, push, release or Zenodo modification was performed.

Suggested commit message (not executed):
`docs: define PhotogateV2 licenses and document third-party exceptions`
