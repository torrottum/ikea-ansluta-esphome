from esphome.components import light
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_OUTPUT_ID
from .. import ikea_ansluta_ns, CONF_IKEA_ANSLUTA_ID, IkeaAnsluta

DEPENDENCIES = ['ikea_ansluta']

CONF_REMOTE_ADDRESS = 'remote_address'
CONF_ADDRESS = 'address'
CONF_PAIRING_ENABLED = 'pairing_enabled'

IkeaAnslutaLight = ikea_ansluta_ns.class_('IkeaAnslutaLight', light.LightOutput, cg.Component)

CONFIG_SCHEMA = cv.All(light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(IkeaAnslutaLight),
    cv.GenerateID(CONF_IKEA_ANSLUTA_ID): cv.use_id(IkeaAnsluta),
    cv.Required(CONF_REMOTE_ADDRESS): cv.hex_uint16_t,
    cv.Optional(CONF_ADDRESS): cv.hex_uint16_t,
    cv.Optional(CONF_PAIRING_ENABLED, default=False): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield light.register_light(var, config)

    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_PAIRING_ENABLED in config:
        cg.add(var.set_pairing_enabled(config[CONF_PAIRING_ENABLED]))

    cg.add(var.set_remote_address(config[CONF_REMOTE_ADDRESS]))

    paren = yield cg.get_variable(config[CONF_IKEA_ANSLUTA_ID])
    cg.add(var.set_radio(paren))
