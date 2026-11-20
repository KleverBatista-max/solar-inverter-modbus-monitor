import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID

DEPENDENCIES = ["uart", "sensor"]

modbus_sniffer_ns = cg.esphome_ns.namespace("modbus_sniffer")
ModbusSniffer = modbus_sniffer_ns.class_("ModbusSniffer", cg.Component)

CONF_UART_ID = "uart_id"
CONF_SLAVE_ID = "slave_id"
CONF_FRAME_GAP_MS = "frame_gap_ms"

CONF_SOC = "soc"
CONF_BAT_V = "battery_voltage"
CONF_BAT_I = "battery_current"
CONF_MPPT1 = "mppt1_power"
CONF_MPPT2 = "mppt2_power"
CONF_LOAD_21B = "load_21b"
CONF_LOAD_232 = "load_232"

CONF_PV1_V = "pv1_voltage"
CONF_PV1_I = "pv1_current"
CONF_PV2_V = "pv2_voltage"
CONF_PV2_I = "pv2_current"
CONF_GRID_V = "grid_voltage"
CONF_GRID_P = "grid_power"
CONF_INV_TEMP = "inverter_temperature"
CONF_RAD_TEMP = "radiator_temperature"
CONF_GRID_A_V = "grid_a_voltage"
CONF_GRID_A_I = "grid_a_current"
CONF_GRID_FREQ = "grid_frequency"
CONF_GRID_B_V = "grid_b_voltage"
CONF_GRID_B_I = "grid_b_current"

SENSOR_SCHEMA = sensor.sensor_schema()

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ModbusSniffer),
        cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
        cv.Optional(CONF_SLAVE_ID, default=1): cv.int_range(min=1, max=247),
        cv.Optional(CONF_FRAME_GAP_MS, default=8): cv.int_range(min=2, max=50),

        cv.Optional(CONF_SOC): SENSOR_SCHEMA,
        cv.Optional(CONF_BAT_V): SENSOR_SCHEMA,
        cv.Optional(CONF_BAT_I): SENSOR_SCHEMA,
        cv.Optional(CONF_MPPT1): SENSOR_SCHEMA,
        cv.Optional(CONF_MPPT2): SENSOR_SCHEMA,
        cv.Optional(CONF_LOAD_21B): SENSOR_SCHEMA,
        cv.Optional(CONF_LOAD_232): SENSOR_SCHEMA,
        
        cv.Optional(CONF_GRID_A_V): SENSOR_SCHEMA,
        cv.Optional(CONF_GRID_A_I): SENSOR_SCHEMA,
        cv.Optional(CONF_GRID_FREQ): SENSOR_SCHEMA,
        cv.Optional(CONF_GRID_B_V): SENSOR_SCHEMA,
        cv.Optional(CONF_GRID_B_I): SENSOR_SCHEMA,

        cv.Optional(CONF_PV1_V): SENSOR_SCHEMA,
        cv.Optional(CONF_PV1_I): SENSOR_SCHEMA,
        cv.Optional(CONF_PV2_V): SENSOR_SCHEMA,
        cv.Optional(CONF_PV2_I): SENSOR_SCHEMA,
        cv.Optional(CONF_GRID_V): SENSOR_SCHEMA,
        cv.Optional(CONF_GRID_P): SENSOR_SCHEMA,
        cv.Optional(CONF_INV_TEMP): SENSOR_SCHEMA,
        cv.Optional(CONF_RAD_TEMP): SENSOR_SCHEMA,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    uart_comp = await cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart(uart_comp))
    cg.add(var.set_slave_id(config[CONF_SLAVE_ID]))
    cg.add(var.set_frame_gap_ms(config[CONF_FRAME_GAP_MS]))

    if CONF_SOC in config:
        s = await sensor.new_sensor(config[CONF_SOC])
        cg.add(var.set_soc_sensor(s))
    if CONF_BAT_V in config:
        s = await sensor.new_sensor(config[CONF_BAT_V])
        cg.add(var.set_bat_v_sensor(s))
    if CONF_BAT_I in config:
        s = await sensor.new_sensor(config[CONF_BAT_I])
        cg.add(var.set_bat_i_sensor(s))
    if CONF_MPPT1 in config:
        s = await sensor.new_sensor(config[CONF_MPPT1])
        cg.add(var.set_mppt1_sensor(s))
    if CONF_MPPT2 in config:
        s = await sensor.new_sensor(config[CONF_MPPT2])
        cg.add(var.set_mppt2_sensor(s))
    if CONF_LOAD_21B in config:
        s = await sensor.new_sensor(config[CONF_LOAD_21B])
        cg.add(var.set_load21b_sensor(s))
    if CONF_LOAD_232 in config:
        s = await sensor.new_sensor(config[CONF_LOAD_232])
        cg.add(var.set_load232_sensor(s))

    if CONF_PV1_V in config:
        s = await sensor.new_sensor(config[CONF_PV1_V])
        cg.add(var.set_pv1_v_sensor(s))

    if CONF_PV1_I in config:
        s = await sensor.new_sensor(config[CONF_PV1_I])
        cg.add(var.set_pv1_i_sensor(s))

    if CONF_PV2_V in config:
        s = await sensor.new_sensor(config[CONF_PV2_V])
        cg.add(var.set_pv2_v_sensor(s))

    if CONF_PV2_I in config:
        s = await sensor.new_sensor(config[CONF_PV2_I])
        cg.add(var.set_pv2_i_sensor(s))

    if CONF_GRID_V in config:
        s = await sensor.new_sensor(config[CONF_GRID_V])
        cg.add(var.set_grid_v_sensor(s))

    if CONF_GRID_P in config:
        s = await sensor.new_sensor(config[CONF_GRID_P])
        cg.add(var.set_grid_p_sensor(s))

    if CONF_INV_TEMP in config:
        s = await sensor.new_sensor(config[CONF_INV_TEMP])
        cg.add(var.set_inv_temp_sensor(s))

    if CONF_RAD_TEMP in config:
        s = await sensor.new_sensor(config[CONF_RAD_TEMP])
        cg.add(var.set_rad_temp_sensor(s))

    if CONF_GRID_A_V in config:
        s = await sensor.new_sensor(config[CONF_GRID_A_V])
        cg.add(var.set_grid_a_v_sensor(s))

    if CONF_GRID_A_I in config:
        s = await sensor.new_sensor(config[CONF_GRID_A_I])
        cg.add(var.set_grid_a_i_sensor(s))

    if CONF_GRID_FREQ in config:
        s = await sensor.new_sensor(config[CONF_GRID_FREQ])
        cg.add(var.set_grid_freq_sensor(s))

    if CONF_GRID_B_V in config:
        s = await sensor.new_sensor(config[CONF_GRID_B_V])
        cg.add(var.set_grid_b_v_sensor(s))

    if CONF_GRID_B_I in config:
        s = await sensor.new_sensor(config[CONF_GRID_B_I])
        cg.add(var.set_grid_b_i_sensor(s))