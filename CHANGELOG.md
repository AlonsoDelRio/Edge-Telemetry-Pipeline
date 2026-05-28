# Changelog
All notable changes to the **Edge-Telemetry-Pipeline** project will be documented in this file.
---
## [2.0.0](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/compare/v1.0.0...v2.0.0) (2026-05-28)


### ⚠ BREAKING CHANGES

* Complete integration of CI/CD pipeline and strict validation.

### Features

* add influxdb service and configure node-red healthcheck dependency ([17febf1](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/17febf1fc2c01fc745f81752aa22c5df0c7c5eba))
* **infra:** implement deterministic multi-bucket provisioning and env injection ([a30e3c3](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/a30e3c312221d4d39c7910a656c5066263f4773f))
* **infra:** implement isolated bridge network 'telemetry-net' for secure service communication ([b08f155](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/b08f155acfc51cc4630a776d3945af7dfe4e6242))
* **node-red:** add package.json with influxdb and dashboard dependencies ([46e7f4f](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/46e7f4fda72514f19caa54aeda3fd352341f807e))
* **node-red:** implement telemetry routing for HW and SIM to influxdb buckets ([80fd373](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/80fd373b1236f9f4aa6ea828c8c22f736bf3100e))
* **observability:** deploy grafana service with declarative provisioning, dashboards, and env schema ([8fc79dc](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/8fc79dc16cb42ec4f5cb80fabdc130fbc34fd7ed))
* official migration to 2.0.0 architecture ([bec35cb](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/bec35cb88d11534236f5de10b95022452e8b9f9a))


### Bug Fixes

* **ci:** remove special characters from dummy token to prevent interpolation errors ([dca6c86](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/dca6c8601ce9c8fd30d838cc82b7b75e87737559))
* **ci:** resolve integration errors and permissions in workflows ([a4fa367](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/a4fa3670c9541572c2ed256143d1ad57f6b14f30))
* **db:** quote variables in setup-buckets.sh to prevent word splitting ([785cec2](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/785cec2ff707b69b3c1e7314145c2aeb81bd5908))
* remove unsupported 'colorize' monitor filter in platformio and add 'esp32_exception_decoder' instead ([06b17c3](https://github.com/AlonsoDelRio/Edge-Telemetry-Pipeline/commit/06b17c34fca563559866936b3af96cd5a7391d21))

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
