# Changelog
All notable changes to the **Edge-Telemetry-Pipeline** project will be documented in this file.
---
## [1.0.0] - 2026-03-31

### Added
- Initial MVP for ESP32 firmware with Wokwi and PlatformIO support.
- MQTT connectivity using `AsyncMqttClient`.
- Telemetry integration for DHT22 (Temperature and Humidity), soil moisture and light.
- Fully functional Node-RED dashboards for real-time monitoring.
- Manual water pump control interface in Node-RED.
- Docker stack configuration for Mosquitto Broker and Node-RED.
- Modular GitHub Actions CI/CD workflows for both firmware and Docker infrastructure.

### Changed
- Refactored build system and directory structure for better scalability.
- Cleaned up `platformio.ini` by removing unnecessary flags.
- Migrated from `#define` to `static const` for improved type safety in hardware configuration.

### Fixed
- Serial output formatting issues (alignment and line endings).

### Notes
- Initial MVP release of the Edge Telemetry Pipeline.