from esphome.components import sensor
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_OUTPUT_ID
from .. import ikea_ansluta_ns, CONF_IKEA_ANSLUTA_ID, IkeaAnsluta

DEPENDENCIES = ['ikea_ansluta']

CONF_REMOTE_ADDRESS = 'remote_address'

IkeaAnslutaSensor = ikea_ansluta_ns.class_('IkeaAnslutaSensor', sensor.Sensor, cg.Component)

CONFIG_SCHEMA = cv.All(sensor.SENSOR_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(IkeaAnslutaSensor),
    cv.GenerateID(CONF_IKEA_ANSLUTA_ID): cv.use_id(IkeaAnsluta),
    cv.Required(CONF_REMOTE_ADDRESS): cv.hex_uint16_t,
}).extend(cv.COMPONENT_SCHEMA))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)

    cg.add(var.set_remote_address(config[CONF_REMOTE_ADDRESS]))

    paren = yield cg.get_variable(config[CONF_IKEA_ANSLUTA_ID])
    cg.add(var.set_radio(paren))
