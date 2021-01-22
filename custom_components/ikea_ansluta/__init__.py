import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import spi

DEPENDENCIES = ['spi']

ikea_ansluta_ns = cg.esphome_ns.namespace('ikea_ansluta')
IkeaAnsluta = ikea_ansluta_ns.class_('IkeaAnsluta', cg.Component, spi.SPIDevice)

CONF_IKEA_ANSLUTA_ID = 'ikea_ansluta_id'
CONF_SEND_COMMAND_N_TIMES = 'send_command_n_times'
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(IkeaAnsluta),
    cv.Optional(CONF_SEND_COMMAND_N_TIMES, default=50): cv.uint16_t,
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    if CONF_SEND_COMMAND_N_TIMES in config:
        cg.add(var.set_send_command_n_times(config[CONF_SEND_COMMAND_N_TIMES]));
    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
