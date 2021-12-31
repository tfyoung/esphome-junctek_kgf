import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (
    CONF_RAW,
    CONF_ID,
    CONF_ADDRESS,
    CONF_INPUT,
    CONF_NUMBER,
    CONF_HARDWARE_UART,
    CONF_TEMPERATURE,
    CONF_VOLTAGE,
    CONF_CURRENT,
    CONF_BATTERY_LEVEL,
    
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    UNIT_CELSIUS,
    UNIT_AMPERE,
    CONF_UPDATE_INTERVAL,
    UNIT_EMPTY,
    UNIT_PERCENT,
    ICON_EMPTY,
    ICON_THERMOMETER,
    ICON_FLASH,
    ICON_PERCENT,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_TEMPERATURE,
)


DEPENDENCIES = ["uart"]

AUTO_LOAD = ["sensor"]

TYPES = [
    CONF_VOLTAGE,
    CONF_CURRENT,
    CONF_BATTERY_LEVEL,
    CONF_TEMPERATURE,
]

JuncTekKGF = cg.global_ns.class_(
    "JuncTekKGF", cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(JuncTekKGF),
            cv.Optional(CONF_ADDRESS, default=1): cv.int_range(1, 255),
            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                UNIT_VOLT,
                ICON_FLASH,
                1,
                DEVICE_CLASS_VOLTAGE,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CURRENT): sensor.sensor_schema(
                UNIT_AMPERE,
                "mdi:current-dc",
                1,
                DEVICE_CLASS_CURRENT,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
                UNIT_PERCENT,
                ICON_PERCENT,
                1,
                DEVICE_CLASS_BATTERY,
                STATE_CLASS_MEASUREMENT,
            ),
             cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                UNIT_CELSIUS,
                ICON_THERMOMETER,
                0,
                DEVICE_CLASS_TEMPERATURE,
                STATE_CLASS_MEASUREMENT,
            ),
        }
    ).extend(uart.UART_DEVICE_SCHEMA)
    )

async def setup_conf(config, key, hub):
    if key in config:
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(hub, f"set_{key}_sensor")(sens))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_ADDRESS])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    for key in TYPES:
        await setup_conf(config, key, var)
