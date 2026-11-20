# Solar inverter Modbus monitor (ESP32 + ESPHome)

Reads a hybrid solar inverter **without touching its monitoring port logic**: an ESP32 with an RS485 adapter *passively listens* to the inverter's Modbus bus (custom ESPHome component `modbus_sniffer`) and publishes everything to Home Assistant: battery SOC, voltage and current, PV1/PV2 (V, A, W), loads, grid, temperatures and frequency.

| Path | What |
|---|---|
| `esphome/solarinverter.yaml` + `esphome/components/modbus_sniffer/` | The ESP32 firmware and the sniffer component |
| `homeassistant/packages/solar_inverter.yaml` | Template sensors (battery power, charge/discharge, grid, total solar, total load, battery runtime, "real solar state" classifier: night → dawn → transition → firm solar, forecast-based "will it charge today"), kWh integrations and the SOC/battery helpers |
| `docs/` | Inverter manual and the Modbus register table (English + original Chinese), hard to find outside China |
| `raspberry-original/read_inverter.py` | First version: a Raspberry Pi + USB‑RS485 reading Modbus RTU (9600 8N1) |
| `esphome/archive/` | Earlier attempts |

## Solar forecast (how the system "knows" how much sun is coming)
Besides reading the inverter, the package adds a **solar forecast** from Home Assistant's *Forecast.Solar* integration, which uses the location of the house, the weather and the orientation of each panel string (here two strings of 2.3 kWp, tilted 15°, facing 162° and 202°, on a 6.5 kW inverter; one integration entry per string). The sensors:
- `Solar Forecast Remaining Today Total`: kWh of sun still expected today, both strings added.
- `Solar Forecast Power Now Total`: expected power right now, to compare with what the panels really produce.
- `kWh faltantes para SOC objetivo`: energy the battery still needs to reach the target SOC.
- `Alcanza a cargar hoy`: true when the forecast remaining today covers the missing energy plus a safety buffer.
The A/C controller uses these to decide whether it is worth pre-cooling now. Set up Forecast.Solar with your own coordinates and panel data (not included here).

## Works with
- **[solar-ac-controller](https://github.com/KleverBatista-max/solar-ac-controller)** uses these sensors to run the air conditioner on solar surplus.
- **[bluetti-solar-charger](https://github.com/KleverBatista-max/bluetti-solar-charger)** charges a power station when the battery shows real surplus.
- Overview of the whole house: [solar-home-overview](https://github.com/KleverBatista-max/solar-home-overview).

## History
Started as a Raspberry Pi script, became an ESP32 sniffer once the manufacturer's supplier shared the register table, and then moved into Home Assistant.

Created by **John Batista** ([@KleverBatista-max](https://github.com/KleverBatista-max)). Built with AI assistants (ChatGPT, Claude) as tools. MIT licensed. Secrets are not included; replace `!secret` values and regenerate ESPHome keys.
