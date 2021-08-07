import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components.light.types import LightState
import esphome.automation as automation
from .types import EnablePairingModeAction, DisableParingModeAction


@automation.register_action('ikea_ansluta.enable_pairing_mode', EnablePairingModeAction, automation.maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(LightState),
}))
def enable_pairing_mode_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    yield var


@automation.register_action('ikea_ansluta.disable_pairing_mode', DisableParingModeAction, automation.maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(LightState),
}))
def disable_pairing_mode_to_code(config, action_id, template_arg, args):
    paren = yield cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    yield var
