import esphome.codegen as cg
import esphome.automation as automation
from esphome.components import light
from ..types import ikea_ansluta_ns

LightOutput = ikea_ansluta_ns.class_('Light', light.LightOutput, cg.Component)
LightOnChangeTrigger = ikea_ansluta_ns.class_('LightOnChangeTrigger',
                                              automation.Trigger.template(cg.uint8))








