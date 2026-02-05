# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

ESPHome packages repository for Tuya-based smart switches and sensors. Devices use LibreTiny-compatible boards (BK72xx chips) and integrate with Home Assistant.

GitHub: `jbrandek/esphome-packages`

## File Naming Convention

Device files follow the pattern: `gel-{gangs}-{board}-{variant}.yaml`

- **gangs**: Number of switch channels (1, 2, or 3)
- **board**: Chip variant
  - `bk` - Generic BK7231T
  - `cb3s` - Tuya CB3S module
  - `wb3s` - Tuya WB3S module
- **variant**: `ns` or `ns2` = "no secrets" variants (wifi credentials in file, not secrets.yaml)

Special devices:
- `cortinas-*.yaml` - Curtain/blinds controller (3 buttons: up/stop/down)
- `athom-presence-sensor.yaml` - mmWave + PIR presence sensor (ESP8285)

## Architecture

- [common.yaml](common.yaml) - Base package imported by device files; includes wifi, api, ota, logging, and standard sensors (wifi signal, uptime, IP/MAC info)
- Device files use `packages:` directive to import common.yaml from GitHub
- Pin mappings differ between CB3S and WB3S boards (check comments in files)

## Key Device Features

- **Shabat Mode**: Disables physical button control (2-second hold to toggle; blue LED indicates active)
- **5-click restart**: Rapidly pressing button 5 times triggers device restart
- **State persistence**: Light states saved to flash via globals with `flash_write_interval: 30s`
- **LED indicators**: Green (relay state), Blue (status/shabat), WiFi LED

## GPIO Pin Mapping (CB3S vs WB3S)

Relay and button pins differ between boards - always verify pin assignments match the target hardware. CB3S typically uses P6/P8/P10 for relays, WB3S uses P6/P7/P9.

## Validation

```bash
esphome config <filename>.yaml  # Validate configuration
esphome compile <filename>.yaml # Compile firmware
```
