# Third-party material

Audit date: 2026-09-12. These are observations about identified versions, not a
license grant over third-party material. Existing notices were preserved.

## Firmware dependencies

The tracked dependency declarations are in
[`firmware/main/idf_component.yml`](firmware/main/idf_component.yml).
The CMake files register the application and include ESP-IDF's build system;
there is no vendored library in the tracked firmware tree.

| Dependency | Declared / observed version | Verified license and evidence |
| --- | --- | --- |
| ESP-IDF | Manifest `>=4.1.0`; README mentions 5.4.2; local materials build resolved 5.4.4 | Apache-2.0 for the framework's own code: [v5.4.2 LICENSE](https://github.com/espressif/esp-idf/blob/v5.4.2/LICENSE). Components with different terms remain exceptions. |
| FreeRTOS kernel | Supplied by ESP-IDF | MIT: [kernel license shipped with ESP-IDF v5.4.2](https://github.com/espressif/esp-idf/blob/v5.4.2/components/freertos/FreeRTOS-Kernel/LICENSE.md). This does not blanket-license Espressif additions or every port. |
| `espressif/button` | `^4.1.3`; local materials resolve 4.1.6 | Apache-2.0: local `espressif__button/license.txt` and [registry 4.1.3](https://components.espressif.com/components/espressif/button/versions/4.1.3). |
| `espressif/knob` | `^1.0.0`; local materials resolve 1.0.2 | Apache-2.0: local `espressif__knob/license.txt`. |
| `espressif/cmake_utilities` | Transitive dependency of button and knob; local materials resolve 0.5.3 | Apache-2.0: local `espressif__cmake_utilities/license.txt` and the parent component manifests. |
| `iamflinks/i2c_lcd_pcf8574` | Git `main`; local materials resolve `7c29391e90379700cc4d6416a02550066d46c3b4`, metadata 1.0.1 | Apache-2.0: local `iamflinks__i2c_lcd_pcf8574/LICENSE.txt` and [upstream declaration](https://github.com/iamflinks/i2c_lcd_pcf8574). |
| `MarcioBulla/menu_manager` | Git `main`; local materials resolve `fa6d547bcf370c687cc6094c8bced6c190ab907f` | Apache-2.0 for current upstream, confirmed by its [LICENSE](https://github.com/MarcioBulla/menu_manager/blob/main/LICENSE) and [manifest](https://github.com/MarcioBulla/menu_manager/blob/main/idf_component.yml), consistent with the owner's instruction. See the historical-version caveat below. |

The local evidence above is in
`PhotogateV2_materials/firmware/managed_components/` and its sibling
`dependencies.lock`. That tree is a separate, untracked local checkout, not the
root project's resolved dependency set. It was inspected read-only and was not
copied, edited or included in the new grants.

The old local `menu_manager` copy identifies Marcio Bulla as maintainer but
contains no standalone license or manifest license field. Current upstream
Apache-2.0 metadata does not establish the license of every historical snapshot.
The registry's 1.0.0 README still displayed an MIT statement during this audit;
the current Git `main` files were checked directly and declare Apache-2.0.
Reconcile the exact version and accompanying notices before distributing that
old copy; do not silently replace its files. The current manifest uses Git,
not the registry release.

Application includes also use ESP-IDF drivers (PCNT, GPIO, I2C, LEDC), NVS,
timers, logging, HAL/SOC headers and C runtime headers. These do not establish a
complete binary dependency bill of materials. Inspect the resolved SDK, C runtime,
transitive components and any binary libraries for the actual release build.
The root `.gitignore` excludes `dependencies.lock`, `managed_components` and
build output, and the two Git dependencies follow mutable `main` branches.
No version or build setting was changed in this licensing pass.

The README also credits `encoder_menu.py`, `rotary.py` and `rotary_irq_esp.py`.
Those files are absent from the tracked tree and the current dependency manifest.
Their exact origins/licenses and any inherited code require manual confirmation;
no license has been inferred from their names.

## Component models and embedded libraries

All 52 STEP/WRL files in `boards/packages3D/` remain outside the project's
CERN-OHL-W-2.0 grant. The audit inventory lists each file separately.

- Some files contain CC-BY-SA-4.0 notices with an explicit KiCad library
  exception and credits to Ray Benitez or kicad StepUp.
- Some contain GPL-3.0-or-later notices with a special model-use/linking
  exception and kicad StepUp credits. Preserve the exact exception printed in
  each file; it is not replaced by the current KiCad library license.
- Others have exporter/author clues but no explicit license. Names such as
  `easyeda2kicad`, FreeCAD or StepUp identify a tool, not permission to reuse
  the input geometry.

KiCad's official libraries currently use
[CC-BY-SA-4.0 with the KiCad libraries exception](https://www.kicad.org/libraries/license/).
That exception permits independent licensing of designs using the library data;
the data itself retains its terms. This does not resolve unidentified historical
copies or EasyEDA imports. The schematic and PCB embed library definitions;
their original PhotogateV2 circuitry/layout is licensed separately from those
definitions. `jlcpcb/project.db` contains supplier-related fields including
LCSC identifiers and stock data and is excluded pending provenance review.

## Website and generated assets

`docs/ibom.html` contains third-party implementation code. Its existing notices
identify Split.js as MIT, lz-string as WTFPL (Pieroxy, 2013), and jQuery PEP
v0.4.3 with jQuery Foundation/contributor copyright and a license link.
InteractiveHtmlBom's upstream [LICENSE](https://github.com/openscopeproject/InteractiveHtmlBom/blob/f42ee38fc93ecb7d619eb63db897cb2477d98423/LICENSE)
is MIT, copyright (c) 2018 qu1ck. That revision is referenced by the website's
external IBOM icon; it is not proof of the generator version used for the HTML.
The bundled subcomponent revisions and complete attribution/license notices
need reconciliation before distributing a cleared IBOM package. No blanket
MIT or CC-BY-SA declaration was applied to the generated HTML.

The website also loads these external resources, which are outside its CC grant:

| External resource | Reference in project | Audit status |
| --- | --- | --- |
| Mermaid | CDN version 10.9.1 in hardware/software pages | Exact distributed asset and notices not audited; no license assigned here. |
| Google model-viewer | Unversioned unpkg URL in hardware/modeling pages | Version and notices need to be recorded. |
| Nerd Font webfonts | `mshaugh/nerdfont-webfonts@v3.3.0` CSS | Font/icon families can have separate licenses; verify actual assets. |
| JetBrains Mono | Google Fonts import in `styles.css` | Verify served font files and notices. |
| Espressif, FreeRTOS, KiCad, FreeCAD, Linux/Tux and OSHW logos | Remote images in `index.html` | Image copyright and trademark terms are not the programs' licenses; review individually. |
| InteractiveHtmlBom icon | Upstream SVG at the revision linked above in `hardware.html` | Retains upstream terms; not PhotogateV2 artwork. |

Local images, the seal, favicon and mixed 3D exports are itemized as pending
in [LICENSE_AUDIT.md](LICENSE_AUDIT.md). Original inline Mermaid diagram text is
distinct from the externally loaded rendering library.

## Development tools

FreeCAD and KiCad are CAD tools; their software licenses do not license the
models, schematics or boards created with them. ESP-IDF's own Apache-2.0 terms
likewise do not replace the licenses of all components used in an application.

## License text sources

The following complete texts were downloaded without editing their contents:

- `LICENSES/Apache-2.0.txt`: [Apache Software Foundation](https://www.apache.org/licenses/LICENSE-2.0.txt).
- `LICENSES/CERN-OHL-W-2.0.txt`: [SPDX License List text](https://raw.githubusercontent.com/spdx/license-list-data/main/text/CERN-OHL-W-2.0.txt).
- `LICENSES/CC-BY-SA-4.0.txt`: [SPDX License List text](https://raw.githubusercontent.com/spdx/license-list-data/main/text/CC-BY-SA-4.0.txt).

These are the three project licenses. They do not replace the license texts,
exceptions or notices required for third-party redistribution.
