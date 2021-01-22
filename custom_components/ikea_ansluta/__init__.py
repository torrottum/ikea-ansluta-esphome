import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import spi

DEPENDENCIES = ['spi']

ikea_ansluta_ns = cg.esphome_ns.namespace('ikea_ansluta')
IkeaAnsluta = ikea_ansluta_ns.class_('IkeaAnsluta', cg.Component, spi.SPIDevice)

CONF_IKEA_ANSLUTA_ID = 'ikea_ansluta_id'
CONF_SEND_COMMAND_NUM_TIMES = 'send_command_n_times'
CONF_SNIFF_AFTER_NUM_COMMANDS_SENT = 'sniff_after_num_commands_sent'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(IkeaAnsluta),
    cv.Optional(CONF_SEND_COMMAND_NUM_TIMES, default=75): cv.uint16_t,
    cv.Optional(CONF_SNIFF_AFTER_NUM_COMMANDS_SENT, default=5): cv.uint16_t,
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    if CONF_SEND_COMMAND_NUM_TIMES in config:
        cg.add(var.set_send_command_num_times(config[CONF_SEND_COMMAND_NUM_TIMES]))
    if CONF_SNIFF_AFTER_NUM_COMMANDS_SENT in config:
        cg.add(var.set_sniff_after_num_commands_sent(config[CONF_SNIFF_AFTER_NUM_COMMANDS_SENT]))
    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
