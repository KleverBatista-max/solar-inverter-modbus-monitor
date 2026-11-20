# Solar inverter Modbus monitor (ESP32 + ESPHome)

Reads a hybrid solar inverter **without touching its monitoring port logic**: an ESP32 with an RS485 adapter *passively listens* to the inverter's Modbus bus (custom ESPHome component `modbus_sniffer`) and publishes everything to Home Assistant: battery SOC, voltage and current, PV1/PV2 (V, A, W), loads, grid, temperatures and frequency.

| Path | What |
|---|---|
| `esphome/solarinverter.yaml` + `esphome/components/modbus_sniffer/` | The ESP32 firmware and the sniffer component |
| `homeassistant/packages/solar_inverter.yaml` | Template sensors (battery power, charge/discharge, grid, total solar, total load, battery runtime, "real solar state" classifier: night → dawn → transition → firm solar, forecast-based "will it charge today"), kWh integrations and the SOC/battery helpers |
| `docs/` | Inverter manual and the Modbus register table (English + original Chinese), hard to find outside China |
| `raspberry-original/read_inverter.py` | First version: a Raspberry Pi + USB‑RS485 reading Modbus RTU (9600 8N1) |
| `esphome/archive/` | Earlier attempts |

## Works with
- **[solar-ac-controller](https://github.com/KleverBatista-max/solar-ac-controller)** uses these sensors to run the air conditioner on solar surplus.
- **[bluetti-solar-charger](https://github.com/KleverBatista-max/bluetti-solar-charger)** charges a power station when the battery shows real surplus.
- Overview of the whole house: [solar-home-overview](https://github.com/KleverBatista-max/solar-home-overview).

## History
Started as a Raspberry Pi script, became an ESP32 sniffer once the manufacturer's supplier shared the register table, and then moved into Home Assistant.

Created by **John Batista** ([@KleverBatista-max](https://github.com/KleverBatista-max)). Built with AI assistants (ChatGPT, Claude) as tools. MIT licensed. Secrets are not included; replace `!secret` values and regenerate ESPHome keys.
