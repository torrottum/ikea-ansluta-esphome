from esphome.components import light
import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.automation as auto
from esphome.const import CONF_OUTPUT_ID, CONF_THRESHOLD, CONF_ADDRESS, CONF_TRIGGER_ID
from .. import ikea_ansluta_ns, CONF_IKEA_ANSLUTA_ID, IkeaAnsluta

DEPENDENCIES = ['ikea_ansluta']

CONF_PAIRING_MODE = 'pairing_mode'
CONF_ON_CHANGE = 'on_change'

IkeaAnslutaLight = ikea_ansluta_ns.class_('IkeaAnslutaLight', light.LightOutput, cg.Component)
LightOnChangeTrigger = ikea_ansluta_ns.class_('IkeaAnslutaLightOnChangeTrigger',
                                              auto.Trigger.template(cg.uint8))

CONFIG_SCHEMA = cv.All(light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(IkeaAnslutaLight),
    cv.GenerateID(CONF_IKEA_ANSLUTA_ID): cv.use_id(IkeaAnsluta),
    cv.Required(CONF_ADDRESS): cv.hex_uint16_t,
    cv.Optional(CONF_PAIRING_MODE, default=False): cv.boolean,
    cv.Optional(CONF_THRESHOLD, default=0.5): cv.zero_to_one_float,
    cv.Optional(CONF_ON_CHANGE): auto.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(LightOnChangeTrigger),
    }),

}).extend(cv.COMPONENT_SCHEMA))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield light.register_light(var, config)

    cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_PAIRING_MODE in config:
        cg.add(var.set_pairing_mode(config[CONF_PAIRING_MODE]))

    if CONF_THRESHOLD in config:
        cg.add(var.set_threshold(config[CONF_THRESHOLD]))

    for conf in config.get(CONF_ON_CHANGE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield auto.build_automation(trigger, [(cg.uint8, 'state')], conf)

    paren = yield cg.get_variable(config[CONF_IKEA_ANSLUTA_ID])
    cg.add(var.set_parent(paren))
