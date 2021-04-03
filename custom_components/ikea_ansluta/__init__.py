import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.automation as auto
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_ADDRESS, CONF_DEBOUNCE
from esphome.components import spi
from .automation import disable_pairing_mode_to_code # noqa
from .types import ikea_ansluta_ns, IkeaAnsluta, OnRemoteClickTrigger

DEPENDENCIES = ['spi']

CONF_IKEA_ANSLUTA_ID = 'ikea_ansluta_id'
CONF_SEND_COMMAND_TIMES = 'send_command_times'
CONF_SNIFF_AFTER_COMMAND_SENT_X_TIMES = 'sniff_after_command_sent_x_times'
CONF_ON_REMOTE_CLICK = 'on_remote_click'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(IkeaAnsluta),
    cv.Optional(CONF_SEND_COMMAND_TIMES, default=75): cv.uint16_t,
    cv.Optional(CONF_SNIFF_AFTER_COMMAND_SENT_X_TIMES, default=5): cv.uint16_t,
    cv.Optional(CONF_ON_REMOTE_CLICK): auto.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnRemoteClickTrigger),
        cv.Optional(CONF_ADDRESS): cv.hex_uint16_t,
        cv.Optional(CONF_DEBOUNCE, default=200): cv.uint16_t,
    }),
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    if CONF_SEND_COMMAND_TIMES in config:
        cg.add(var.set_send_command_times(config[CONF_SEND_COMMAND_TIMES]))
    if CONF_SNIFF_AFTER_COMMAND_SENT_X_TIMES in config:
        cg.add(var.set_sniff_after_command_sent_x_times(config[CONF_SNIFF_AFTER_COMMAND_SENT_X_TIMES]))

    for conf in config.get(CONF_ON_REMOTE_CLICK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        if CONF_ADDRESS in conf:
            cg.add(trigger.set_address(conf[CONF_ADDRESS]))
        if CONF_DEBOUNCE in conf:
            cg.add(trigger.set_debounce(conf[CONF_DEBOUNCE]))
        yield auto.build_automation(trigger, [(cg.uint16, 'address'), (cg.uint8, 'command')], conf)

    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
