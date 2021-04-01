import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.automation as auto
from esphome.const import CONF_ID, CONF_TRIGGER_ID, CONF_COMMAND
from esphome.components import spi
from esphome.components.light.types import LightState

DEPENDENCIES = ['spi']

ikea_ansluta_ns = cg.esphome_ns.namespace('ikea_ansluta')
IkeaAnsluta = ikea_ansluta_ns.class_('IkeaAnsluta', cg.Component, spi.SPIDevice)

CONF_IKEA_ANSLUTA_ID = 'ikea_ansluta_id'
CONF_SEND_COMMAND_TIMES = 'send_command_times'
CONF_SNIFF_AFTER_COMMANDS_SENT = 'sniff_after_commands_sent'
CONF_ON_REMOTE_CLICK = 'on_remote_click'

OnRemoteClickTrigger = ikea_ansluta_ns.class_('OnRemoteClickTrigger',
                                              auto.Trigger.template(cg.uint16, cg.uint8))
# Actions
EnablePairingModeAction = ikea_ansluta_ns.class_('EnablePairingModeAction', auto.Action)
DisableParingModeAction = ikea_ansluta_ns.class_('DisablePairingModeAction', auto.Action)
SendCommandAction = ikea_ansluta_ns.class_('SendCommandAction', auto.Action)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(IkeaAnsluta),
    cv.Optional(CONF_SEND_COMMAND_TIMES, default=75): cv.uint16_t,
    cv.Optional(CONF_SNIFF_AFTER_COMMANDS_SENT, default=5): cv.uint16_t,
    cv.Optional(CONF_ON_REMOTE_CLICK): auto.validate_automation({
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnRemoteClickTrigger),
    }),
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())

@auto.register_action('ikea_ansluta.enable_pairing_mode', EnablePairingModeAction, auto.maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(LightState),
}))
def enable_pairing_mode_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    yield var

@auto.register_action('ikea_ansluta.disable_pairing_mode', DisableParingModeAction, auto.maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(LightState),
}))
def disable_pairing_mode_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    yield var

@auto.register_action('ikea_ansluta.send_command', SendCommandAction, auto.maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(LightState),
    cv.Required(CONF_COMMAND): cv.templatable(cv.uint8_t),
}))
def send_command_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    templ = yield cg.templatable(config[CONF_COMMAND], args, cg.uint8)
    cg.add(var.set_command(templ))
    yield var

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    if CONF_SEND_COMMAND_TIMES in config:
        cg.add(var.set_send_command_times(config[CONF_SEND_COMMAND_TIMES]))
    if CONF_SNIFF_AFTER_COMMANDS_SENT in config:
        cg.add(var.set_sniff_after_commands_sent(config[CONF_SNIFF_AFTER_COMMANDS_SENT]))

    for conf in config.get(CONF_ON_REMOTE_CLICK, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield auto.build_automation(trigger, [(cg.uint16, 'address'), (cg.uint8, 'command')], conf)
    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)