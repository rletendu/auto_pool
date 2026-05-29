# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

ESP32 swimming pool automation controller (PlatformIO + Arduino framework). Controls filter pump scheduling/speed, pH dosing (acid + optional base) and ORP/chlorine dosing via peristaltic pumps, with sensor reads (DS18B20 water temp, DHT22 air, ADS1115 pH/ORP/pressure, DS3231 RTC, level switches), a Nextion TFT UI, MQTT, web server, OTA, and a serial CLI.

A parallel ESPHome rewrite lives in `autopool_esphome.yaml` (see `2026-04-24-migration_esphome.txt`); the PlatformIO firmware in `src/` is still the authoritative implementation.

## Build / flash / run

PlatformIO is the only toolchain. Single environment `esp32dev` defined in `platformio.ini`. The Arduino-ESP32 core is pinned to `2.0.13` — don't bump it casually.

- `pio run` — build firmware
- `pio run -t upload` — flash via USB (default), or override on the CLI: `pio run -t upload --upload-port 192.168.x.x` for OTA (espota). Commented examples in `platformio.ini`.
- `pio run -t uploadfs` — flash the `data/` directory to SPIFFS. **Required** whenever web assets or `data/config.json` / `data/state.json` change; firmware reads them at boot via `parameters_read_file()` / `state_default_read_file()`.
- `pio device monitor` — serial monitor at 115200 baud.
- `pio run -t clean` — clean build.

There are no unit tests wired up; `test/autopool_tester.py` is a host-side helper, not a PlatformIO test runner.

## Architecture

### Entry point and main loop

`src/main.cpp` is the standard Arduino `setup()` / `loop()`. `setup()` calls each subsystem's `*_init()` in a fixed order (board → wifi → params → OTA → MQTT → CLI → RTC → state → filter/orp/ph control → measures → web/telnet) and reports each step to the Nextion boot page via `disp_boot_progress_message()`. `loop()` calls every subsystem's `*_loop()` plus `timer_pool.tick()` (a `SoftTimer` cooperative scheduler in `soft_timer.h`).

Subsystems are compile-time gated by `HAS_*` flags in `src/config.h` (`HAS_OTA`, `HAS_MQTT`, `HAS_CLI`, `HAS_WEB_SERVER`, `HAS_TELNET_SERVER`, `HAS_PH_CONTROL`, `HAS_ORP_CONTROL`, `HAS_FILTER_CONTROL`, `HAS_FILTER_PWR_CTRL`, `HAS_PH_PLUS_PUMP`, `HAS_QUIET_MEASURES`, `HAS_CUSTOM_MAC`). Touch this file to disable a feature wholesale rather than `#if 0`-ing call sites.

`src/autopool.h` is the umbrella header pulled in by every translation unit.

### Control loops (state machines)

Three independent regulators, each in its own pair of files with an `enum *_state_t` defined in `src/state.h`:

- `filter_control.{h,cpp}` — filter pump scheduling (`FILTER_OFF/ON/AUTO`) and power level (`FILTER_POWER_FULL/REG`). Auto mode uses `parameters.timer_prog` (24-bit hourly bitmask) or `timer_prog_temperature[]` (per-temperature schedule), selected by `parameters.filter_auto_mode`. States include warm-up, active, extended, periodic.
- `ph_control.{h,cpp}` — separate pH- and pH+ modes; injects on a `PH_REGULATION_CYCLE_S` cycle proportional to deviation from `parameters.target_ph` ± `parameters.delta_ph`.
- `orp_control.{h,cpp}` — chlorine dosing on `ORP_REGULATION_CYCLE_S`, with daily ml caps from `parameters.cl_max_day` / `phm_max_day`.

Daily injection counters (`measures.daily_ml_*`) reset at midnight inside `time_update()` in `main.cpp` and are mirrored to ESP32 RTC slow-memory (`RTC_NOINIT_ATTR`) so they survive soft resets but not power loss. `bootCount` and `boot_key` (compared against `BOOT_KEY_MAGIC`) detect cold vs warm boot.

### Hardware abstraction

`src/board.h` is the single source of truth for pin assignments and gives inline accessors (`pump_filtration_on/off/is_on`, `pump_ph_minus_*`, `level_water_is_ok`, etc.). Pump polarity is encoded in `PUMP_*_ACTIVE_VALUE` macros — the filter pump uses `PUMP_FILTER_ACTIVE_VALUE = true` because it's wired through an SSR (commit `f2c1aeb`), while dosing relays are active-low (`PUMP_ACTIVE_VALUE = false`). Always go through these helpers; don't `digitalWrite` raw pins from feature code.

Sensor reads are in `board.cpp` / `measures.cpp`: `water_get_temperature()` (DS18B20 via `DallasTemperature`), `water_get_ph()` / `water_get_orp()` (ADS1115 channels `ADS_CH_PH` / `ADS_CH_CL`, with `parameters.ph_offset` / `orp_offset` calibration applied), `pump_filtration_get_pressure()` (ADS channel + tare offset), DHT, DS3231.

### Persistence

Two SPIFFS JSON files, both in `data/`:

- `/config.json` ↔ `struct ParametersStruture` (see `parameters.h`) — user setpoints, MQTT credentials, calibration offsets, schedules. Read at boot, written by `parameters_write_file()` when WiFiManager portal saves new values (`is_should_save_config()`) or via web/MQTT updates.
- `/state.json` ↔ `struct StateStructure` — last-known mode of each regulator, restored at boot so reboots don't reset operator intent.

Buffer size constants (`PARAMETERS_JSON_MESSAGE_LEN`, `STATE_*_JSON_MESSAGE_LEN`) live in `config.h`. ArduinoJson is pinned to `<6.0` (v5 API) — keep using `JsonObject&` / `parseObject()` / `printTo()`, not v6's `JsonDocument`.

### Display

Nextion TFT over `Serial2` (`PIN_RX2`/`PIN_TX2`, `DISPLAY_BAUD = 115200`). The vendor `lib/Nextion` library is used. The HMI source (`hmi/auto_pool_ui.HMI`) is edited in the Nextion Editor; firmware references its widgets by name in `display_components.{h,cpp}` (e.g. `dis_sys_hour`, `disp_progress_hour`, `boot_version`, `disp_options_*`). When adding a widget, declare it there and bind any touch callback in `display.cpp`. `display_logger.{h,cpp}` maintains a circular log of `DISP_LOG_NB_LINES` lines on the LOG page.

OTA for the display itself goes through `ota_tft.cpp` using `lib/ESPNexUpload`.

### Networking

- `wifi_manager.{h,cpp}` wraps `lib/WiFiManager`; first-boot opens captive portal `PORTAL_NAME` (suffixed with `parameters.device_suffix` when set) for SSID + MQTT settings + the device suffix itself.
- `mqtt.{h,cpp}` uses PubSubClient. Topic root is `parameters.mqtt_base_topic`. `MQTT_MAX_PACKET_SIZE=1024` is set via `build_flags` in `platformio.ini` because state/measure/discovery JSON exceeds the library default of 256.
- `server.{h,cpp}` is the HTTP UI; static assets served from SPIFFS (`data/index.html`, `app.js`, `style.css`, etc.).
- `ota.{h,cpp}` is `ArduinoOTA` (espota), registered under mDNS as `autopool` or `autopool-<suffix>`; `telnet.{h,cpp}` uses `lib/RemoteDebug` (off by default).
- `cli.{h,cpp}` uses SimpleCLI on the USB serial.

### Multi-device support

`parameters.device_suffix` (lowercase `[a-z0-9_]`, max 12 chars, empty by default) is spliced into every shared identifier so two controllers can share a broker. Splice points live in `mqtt_reconnect()` (client_id), `publish_cfg()` + `add_device_block()` in `mqtt_discovery.cpp` (obj_id, dev["name"]), `portal_name()` in `wifi_manager.cpp` (AP SSID), and `ota_init()` (mDNS hostname). The validator + sanitizer are `device_suffix_is_valid()` / `device_suffix_sanitize()` in `parameters.cpp`. Setting the suffix via `<base>/CMD/SET/DEVICE_SUFFIX` also auto-rewrites `mqtt_base_topic` if it still has the legacy default.

### Vendored libraries

`lib/` contains in-tree forks of `Nextion`, `WiFiManager`, `RemoteDebug`, `SerialDebug`, and `ESPNexUpload`. Prefer editing these to swapping for upstream — pin maps and behavior have been customized. `SerialDebug.h` was modified locally (uncommitted change at session start).

## Conventions

- Logging: `printlnA(F("..."))` / `printA(...)` macros from SerialDebug — not `Serial.println`. Severity letters (`A`, `D`, `V`, `W`, `E`) gate output by configured level. `mqtt_publish_log()` and `log_append()` add MQTT/display-log entries.
- Time constants in `config.h` are in seconds and follow the `*_S` suffix convention (e.g. `PH_REGULATION_CYCLE_S`).
- Use `SoftTimer` (`timer_pool.every(ms, fn)`) for periodic work, not `delay()` in loops. Cancel with the `uintptr_t` handle returned at registration.
- Secrets belong in `secrets.yaml` (gitignored) — used by the ESPHome config; the PlatformIO firmware reads MQTT credentials from `data/config.json` populated via the WiFiManager portal.
