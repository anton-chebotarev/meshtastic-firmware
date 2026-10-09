<div align="center" markdown="1">

<img src=".github/meshtastic_logo.png" alt="Meshtastic Logo" width="80"/>
<h1>Meshtastic Firmware</h1>

![GitHub release downloads](https://img.shields.io/github/downloads/meshtastic/firmware/total)
[![CI](https://img.shields.io/github/actions/workflow/status/meshtastic/firmware/main_matrix.yml?branch=master&label=actions&logo=github&color=yellow)](https://github.com/meshtastic/firmware/actions/workflows/ci.yml)
[![CLA assistant](https://cla-assistant.io/readme/badge/meshtastic/firmware)](https://cla-assistant.io/meshtastic/firmware)
[![Fiscal Contributors](https://opencollective.com/meshtastic/tiers/badge.svg?label=Fiscal%20Contributors&color=deeppink)](https://opencollective.com/meshtastic/)
[![Vercel](https://img.shields.io/static/v1?label=Powered%20by&message=Vercel&style=flat&logo=vercel&color=000000)](https://vercel.com?utm_source=meshtastic&utm_campaign=oss)

<a href="https://trendshift.io/repositories/5524" target="_blank"><img src="https://trendshift.io/api/badge/repositories/5524" alt="meshtastic%2Ffirmware | Trendshift" style="width: 250px; height: 55px;" width="250" height="55"/></a>

</div>

</div>

<div align="center">
	<a href="https://meshtastic.org">Website</a>
	-
	<a href="https://meshtastic.org/docs/">Documentation</a>
</div>

## Ukrainian fork

> This is an unofficial fork of [meshtastic/firmware](https://github.com/meshtastic/firmware) for use in Ukraine. It is not published or supported by the Meshtastic project. Everything from "Overview" down is the upstream README.

The `v2.7.26.54e0d8d-ua` branch is upstream `v2.7.26.54e0d8d` plus these changes:

- **Ukrainian (Cyrillic) text on the device screen.** Messages and node names are drawn correctly instead of as `¿`, including Є, І, Ї and Ґ. Enabled for every board: OLED/TFT/E-Ink via `-D OLED_UA=1` in `platformio.ini`, InkHUD via Windows-1251 fonts.
- **Radio defaults for Ukraine.** A device with factory settings starts on region `UA_433`, LongFast preset, frequency slot 1 (433.125 MHz). Set in `userPrefs.jsonc`.
- **TX power: 10 dBm by default, up to 22 dBm on request.** The `UA_433` limit is raised from 10 to 22 dBm, but an unset power still resolves to 10 dBm; going higher means entering the value explicitly. You are responsible for staying within the rules that apply to you.

No channels or keys are built in.

### Downloads

Prebuilt firmware is on the [Releases](https://github.com/anton-chebotarev/meshtastic-firmware/releases) page, for:

| Device                          | PlatformIO environment |
| ------------------------------- | ---------------------- |
| LILYGO T-Beam (v1.x, incl. 1.2) | `tbeam`                |
| Heltec V3                       | `heltec-v3`            |
| LILYGO T-Echo, classic screen   | `t-echo`               |
| LILYGO T-Echo, InkHUD screen    | `t-echo-inkhud`        |

Only these four have been built. Any other board can be built from this branch with `pio run -e <env>`, but has not been built or tested here.

### Things to know

- The radio defaults apply only to a device with factory settings. An update keeps the device's saved region, slot and TX power; factory-reset it to pick up the new defaults.
- Western European accented letters (é, ü, ß, ...) no longer display on the screen, and InkHUD boards that used Greek fonts lose Greek. Their place in the fonts is taken by Cyrillic.
- There is no Ukrainian keyboard on the device. Type Ukrainian in the phone app.
- Boards running the colour-TFT MUI already support Ukrainian upstream: choose it under Settings → Language.
- On Windows, ESP32-S3 builds can fail with `CreateProcess: No such file or directory` because the compiler command exceeds the Windows length limit. Pointing `PLATFORMIO_CORE_DIR` at a short path (for example `C:\pio`) works around it for most boards.

## Overview

This repository contains the official device firmware for Meshtastic, an open-source LoRa mesh networking project designed for long-range, low-power communication without relying on internet or cellular infrastructure. The firmware supports various hardware platforms, including ESP32, nRF52, RP2040/RP2350, and Linux-based devices.

Meshtastic enables text messaging, location sharing, and telemetry over a decentralized mesh network, making it ideal for outdoor adventures, emergency preparedness, and remote operations.

### Get Started

- 🔧 **[Building Instructions](https://meshtastic.org/docs/development/firmware/build)** – Learn how to compile the firmware from source.
- ⚡ **[Flashing Instructions](https://meshtastic.org/docs/getting-started/flashing-firmware/)** – Install or update the firmware on your device.

Join our community and help improve Meshtastic! 🚀

## Stats

![Alt](https://repobeats.axiom.co/api/embed/8025e56c482ec63541593cc5bd322c19d5c0bdcf.svg "Repobeats analytics image")
