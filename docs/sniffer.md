# The Modbus sniffer, line by line

An ESP32 listens (**RX only, it never transmits**) to the RS485 bus between the inverter and its own monitoring master, and turns the traffic it overhears into Home Assistant sensors. Because it never talks, it cannot disturb the inverter or its original monitor.

## Hardware
- ESP32 (`esp32dev`, esp-idf) + RS485 adapter, **RX on GPIO34**, Modbus RTU **9600 8N1**, UART buffer 256 bytes.
- Wired in parallel to the A/B lines of the inverter's monitoring port.

## How the component works (`components/modbus_sniffer/`)
1. **Collect bytes.** Each loop iteration reads every available byte into a 256-byte buffer and stamps the time of the last byte.
2. **Frame by silence.** Modbus RTU has no start/stop markers: a frame ends when the line is quiet for `frame_gap_ms` (default **8 ms**, tunable 2–50). When that gap passes, the buffer is treated as one frame.
3. **Validate.** The frame must be at least 5 bytes, pass the **CRC-16/Modbus** check (polynomial 0xA001) and carry the configured `slave_id` (default 1). Anything else is dropped silently.
4. **Pair request and response.** A response does *not* say which registers it holds, only how many bytes. So the sniffer remembers the last **request** (function 0x03 or 0x04, exactly 8 bytes: start address and quantity) and, when the matching **response** arrives (same function, byte count consistent), decodes it starting from that address. After a response it forgets the request, so a later frame can't be attributed to the wrong registers.
5. **Decode.** Each 16-bit register is published to the sensor mapped to its address.

## Register map used
| Register | Meaning | Scale |
|---|---|---|
| 0x0100 | Battery SOC | % |
| 0x0101 | Battery voltage | × 0.1 V |
| 0x0102 | Battery current (**signed**, negative = charging) | × 0.1 A |
| 0x0107 / 0x0108 / 0x0109 | PV1 voltage / current / power | × 0.1 V / × 0.1 A / W |
| 0x010F / 0x0110 / 0x0111 | PV2 voltage / current / power | × 0.1 V / × 0.1 A / W |
| 0x021B, 0x0232 | Load 1 and Load 2 (summed in Home Assistant) | W |
| 0x0213 / 0x0214 / 0x0215 | Grid A voltage / current / frequency | × 0.1 V / × 0.1 A / × 0.01 Hz |
| 0x022A / 0x0238 | Grid B voltage / current | × 0.1 V / × 0.1 A |
| 0x0037 / 0x0038 | Inverter and radiator temperature | °C |

The addresses come from the manufacturer's register table in `docs/` (English and Chinese). Any register not in the list is ignored, so adding one is a new `else if` in `publish_reg_` plus a sensor option in `__init__.py`.

## What I would fix when it is running again
- Remove the leftover debug banner in `setup()` (`BUILD_TEST_MPPT_V9999`).
- `publish_reg_` logs **every** register at WARN level and the YAML sets the logger to `VERY_VERBOSE`: lower both once it works.
- It publishes on every frame even if the value did not change; a `delta` or `throttle` filter would cut Home Assistant traffic.
- It only trusts one slave id; if the inverter's monitor changes address the sensors go silent without a warning.
