# Maintaining the Ukrainian fork

This is the maintainer's guide for the Ukrainian fork of the Meshtastic firmware: what the fork changes, how to move it onto a newer official release, and how to build, package and publish a release. For what the fork is and where to download it, see the "Ukrainian fork" section of [README.md](README.md).

Commands are written for Windows PowerShell, run from the repository root.

## What the fork changes

Each fork branch is named `v<upstream version>-ua` and sits on top of the matching upstream tag. The first is `v2.7.26.54e0d8d-ua`, on upstream `v2.7.26.54e0d8d`.

| #   | Change                                                                                                                                  | Where                                                                                    | Commit      |
| --- | --------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------- | ----------- |
| 1   | `UA_433` power limit raised from 10 to 22 dBm                                                                                           | `src/mesh/RadioInterface.cpp` (`RDEF(UA_433, ...)`)                                      | `3103af903` |
| 2   | Unset TX power on `UA_433` resolves to 10 dBm, so 22 dBm is opt-in                                                                      | `src/mesh/RadioInterface.cpp`                                                            | `d497522d4` |
| 3   | Cyrillic screen fonts on every board: `-D OLED_UA=1` in the shared build flags                                                          | `platformio.ini` (`[env]`)                                                               | `f095fba87` |
| 4   | `0xD2` treated as a UTF-8 prefix byte, so Ґ/ґ are not preceded by `¿`                                                                   | `src/graphics/Screen.h`                                                                  | `5a9aa8494` |
| 5   | InkHUD boards use Windows-1251 fonts; new 18 pt and 24 pt Cyrillic fonts for the T5-S3 e-paper                                          | `variants/**/nicheGraphics.h`, `src/graphics/niche/Fonts/`, `AppletFont.h`               | `f095fba87` |
| 6   | Cyrillic version of the 30 px monospaced e-ink font (classic UI on T5-S3 e-paper and CrowPanel 5.79")                                   | `src/graphics/fonts/EinkDisplayFonts.*`, `src/graphics/ScreenFonts.h`                    | `f095fba87` |
| 7   | Radio defaults: region `UA_433`, LongFast, frequency slot 1                                                                             | `userPrefs.jsonc`                                                                        | `e25f394c0` |
| 8   | Fork section in the README                                                                                                              | `README.md`                                                                              | `dcf62bbff` |

Deliberately not built in:

- **Channel keys.** The fork is public, so a committed key is a published key, and it is readable in every binary. Channels are shared by link or QR code.
- **Exact position precision and MQTT uplink/downlink defaults.** These are per-user safety choices.
- **Hop limit above the default of 3.**

Known trade-offs: Western European accented letters no longer display on the screen, and InkHUD boards that used Greek fonts lose Greek. MUI (colour-TFT) boards need no change, because the `device-ui` library already ships Cyrillic glyphs and a Ukrainian translation.

## Moving to a newer official release

1. Fetch upstream and start a new branch from the new tag:

   ```powershell
   git fetch upstream --tags
   git switch -c v<new version>-ua <new upstream tag>
   ```

2. List the fork's commits on the previous branch, then cherry-pick them in order, resolving conflicts. The list includes the ones in the table above and anything added since, such as this guide:

   ```powershell
   git log --oneline --reverse <previous upstream tag>..<previous -ua branch>
   git cherry-pick <first fork commit>^..<last fork commit>
   ```

   Expect conflicts in `platformio.ini`, `userPrefs.jsonc` and the `nicheGraphics.h` files, which upstream edits often.

3. Check for leftovers. Upstream adds boards between releases, and each new InkHUD board defaults to Latin fonts again. Run in Git Bash:

   ```bash
   # InkHUD font assignments that are not Windows-1251 (expect no output)
   grep -rnE "font(Small|Medium|Large)\s*=" variants src --include=*.h --include=*.cpp | grep -v WIN1251 | grep -v "docs/"

   # Fonts used outside the FONT_* macros and InkHUD's own font variables (expect only those)
   grep -rhoE "setFont\([^)]*\)" src --include=*.cpp --include=*.h | sort | uniq -c

   # Font overrides in ScreenFonts.h that bypass the *_UA fonts
   grep -nE "define FONT_(SMALL|MEDIUM|LARGE) " src/graphics/ScreenFonts.h
   ```

   Also confirm `-D OLED_UA=1` is still in the shared `[env]` build flags and that no environment sets another language flag (`OLED_RU`, `OLED_PL`, `OLED_CS`, `OLED_GR`); `OLED_UA` loses to all of them.

4. Update the branch name in the README's fork section, then set the new branch as the fork's default branch on GitHub so its README shows on the front page.

### If a new font size is needed

The existing fonts were generated with FreeType, monochrome rendering, and can be reproduced:

- **InkHUD 6, 9 and 12 pt:** GNU FreeSans.
- **InkHUD 18 and 24 pt:** DejaVu Sans 2.37 at 141 DPI, despite the `FreeSans` file names. Glyphs `0x01`–`0x1F` are InkHUD's emoji and are copied from an existing font of the same size.
- **`Monospaced_plain_30`:** DejaVu Sans Mono at 30 px, baseline on row 28 of the 36-row cell.

Cyrillic glyphs go at their Windows-1251 positions. Check a new generator by regenerating an existing font first and comparing it with the file in the repo.

## Building

Releases are built locally for four boards only:

| Device                          | PlatformIO environment |
| ------------------------------- | ---------------------- |
| LILYGO T-Beam (v1.x, incl. 1.2) | `tbeam`                |
| Heltec V3                       | `heltec-v3`            |
| LILYGO T-Echo, classic screen   | `t-echo`               |
| LILYGO T-Echo, InkHUD screen    | `t-echo-inkhud`        |

```powershell
$env:PLATFORMIO_CORE_DIR = 'C:\pio'
python -m platformio run -e tbeam -e heltec-v3 -e t-echo -e t-echo-inkhud
```

Outputs land in `.pio\build\<env>\`, named with the version and the short hash of the commit that was built, for example `firmware-tbeam-2.7.26.e25f394.bin`. Commit before building, so the hash in the file names matches the release.

Windows notes:

- **Short PlatformIO path.** ESP32-S3 builds pass so many include paths that the compiler command exceeds the Windows length limit and fails with `CreateProcess: No such file or directory`. Pointing `PLATFORMIO_CORE_DIR` at a short path fixes it for `heltec-v3`. `C:\pio` can be a junction to the real folder: `New-Item -ItemType Junction -Path C:\pio -Target $env:USERPROFILE\.platformio`.
- **Some boards still fail to link on Windows** for the same reason (`t5s3_epaper_inkhud` did). Build those on Linux or in CI.

State in the commit message and release notes exactly which environments were built and which were checked on hardware. The fork changes all environments, but only these four are ever built.

## Packaging

A release contains, for version `<ver>`:

| File                                 | Source                                 | Purpose                                             |
| ------------------------------------ | -------------------------------------- | --------------------------------------------------- |
| `firmware-t-echo-<ver>.uf2`          | `.pio\build\t-echo\`                   | T-Echo, classic screen                              |
| `firmware-t-echo-inkhud-<ver>.uf2`   | `.pio\build\t-echo-inkhud\`            | T-Echo, InkHUD                                      |
| `firmware-<env>-<ver>.factory.bin`   | `.pio\build\<env>\`                    | ESP32 full image for esptool, written once at `0x0` |
| `firmware-<env>-<ver>.bin`           | `.pio\build\<env>\`                    | ESP32 update image, written at `0x10000`            |
| `webflasher-<env>-<ver>-ua.zip`      | built as below                         | ESP32 bundle for the Meshtastic web flasher         |
| `SHA256SUMS.txt`                     | generated                              | Checksums                                           |

### The web flasher zip

When a user uploads their own firmware to the [Meshtastic web flasher](https://flasher.meshtastic.org), the flasher looks for files by an older naming scheme than current builds produce. The zip holds the normal build outputs under those names:

| Name in the zip                         | Content                                              | The flasher writes it at                          |
| --------------------------------------- | ---------------------------------------------------- | ------------------------------------------------- |
| `firmware-<env>-<ver>.factory.bin`      | the factory image                                    | `0x0` (full install)                              |
| `firmware-<env>-<ver>-update.bin`       | the update image, renamed                            | `0x10000` (update)                                |
| `bleota.bin` or `bleota-s3.bin`         | the upstream OTA image, renamed (ESP32 / ESP32-S3)   | OTA slot: `0x260000` on 4 MB, `0x340000` on 8 MB  |
| `littlefs-<env>-<ver>.bin`              | the filesystem image                                 | filesystem slot: `0x300000` on 4 MB, `0x670000` on 8 MB |

Rules that follow from how the flasher matches names:

- The zip must contain no other `firmware-<env>-*.bin` that does not end in `-update.bin`, or a full install may pick the wrong file.
- The offsets are fixed in the flasher. Compare them with the `part` list in `.pio\build\<env>\firmware-<env>-<ver>.mt.json` whenever upstream changes a partition table.

To build both zips into `.pio\release`:

```powershell
$gh = 'C:\Program Files\GitHub CLI\gh.exe'
$v = '<ver>'          # for example 2.7.26.e25f394
$out = '.pio\release'
New-Item -ItemType Directory -Force "$out\ota" | Out-Null
& $gh release download -R meshtastic/esp32-unified-ota -p 'mt-esp32-ota.bin' -p 'mt-esp32s3-ota.bin' -D "$out\ota" --clobber
$boards = @(
  @{ env = 'tbeam';     ota = 'mt-esp32-ota.bin';   otaName = 'bleota.bin' },
  @{ env = 'heltec-v3'; ota = 'mt-esp32s3-ota.bin'; otaName = 'bleota-s3.bin' }
)
foreach ($b in $boards) {
  $e = $b.env; $src = ".pio\build\$e"; $stage = "$out\stage-$e"
  New-Item -ItemType Directory -Force $stage | Out-Null
  Copy-Item "$src\firmware-$e-$v.factory.bin" $stage
  Copy-Item "$src\firmware-$e-$v.bin" "$stage\firmware-$e-$v-update.bin"
  Copy-Item "$src\littlefs-$e-$v.bin" $stage
  Copy-Item "$out\ota\$($b.ota)" "$stage\$($b.otaName)"
  Compress-Archive -Path "$stage\*" -DestinationPath "$out\webflasher-$e-$v-ua.zip" -Force
}
```

Status as of the first release: the zip layout was derived from the web flasher's source (`stores/firmwareStore.ts` and `components/targets/Esp32.vue`, October 2026) and checked against its matching rules, but had not been confirmed by a real flash. The flasher is a live website; re-read those files if a zip stops working.

### Why a single file is not enough

Given one uploaded file, the web flasher uses it for every image it writes. This is the failure to warn users about in every release:

| Uploaded file  | Full Erase and Install | What the flasher does                                         | Result                                                                                                              |
| -------------- | ---------------------- | ------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| `.bin`         | not offered            | Writes it at `0x10000`                                        | Fine on a board already running Meshtastic; an erased board has no bootloader and does not start                    |
| `.factory.bin` | on                     | Writes the same file at `0x0`, the OTA slot and the filesystem slot | Boot loop: the later copies run past the end of the flash, wrap to `0x0` and overwrite the bootloader. Seen on a T-Beam |
| `.factory.bin` | off                    | Writes the full image at `0x10000`                            | Does not start                                                                                                      |

A board in this state prints `invalid header: 0x...` repeatedly on its serial port. It is not damaged. To recover it:

```powershell
python C:\pio\packages\tool-esptoolpy\esptool.py --port COM8 erase_flash
python C:\pio\packages\tool-esptoolpy\esptool.py --port COM8 write_flash 0x0 firmware-tbeam-<ver>.factory.bin
```

If it will not connect while looping, hold BOOT, tap RST, then release BOOT. Close any browser flasher tab first; it holds the serial port.

## Publishing

1. Push the branch: `git push -u origin v<version>-ua`.
2. Write the release notes. Reuse the structure of the previous release: what differs from stock, the file table, one flashing method per kind of file, the single-file warning above, and a "Tested" section that says which boards were verified on hardware.
3. Create the release as a draft, pinned to the commit the binaries were built from:

   ```powershell
   & $gh release create v<ver>-ua <files> -R anton-chebotarev/meshtastic-firmware --target <built commit SHA> --title "<title>" --notes-file RELEASE_NOTES.md --draft
   ```

4. Review the draft on GitHub, then publish it there: open Releases, click the pencil on the draft, and press "Publish release". The publish button is only on the edit screen.

To change a published release, update it in place so the tag and download links survive:

```powershell
& $gh release upload v<ver>-ua <files> --clobber -R anton-chebotarev/meshtastic-firmware
& $gh release edit v<ver>-ua --notes-file RELEASE_NOTES.md -R anton-chebotarev/meshtastic-firmware
```

Do not pass `--draft` to `gh release edit` on a published release. It unpublishes it.

## Checking a release on hardware

After a full install, the first seconds of the serial log (115200 baud) should show the fork's defaults:

```text
Wanted region 14, using UA_433
Set radio: region=UA_433, name=LongFast, config=0, ch=0, power=10
channel_num: 1
frequency: 433.125000
Final Tx power: 10 dBm
```

These defaults apply only when a device installs factory settings. An update keeps the device's saved region, slot and TX power, so check them after a full install or a factory reset.
