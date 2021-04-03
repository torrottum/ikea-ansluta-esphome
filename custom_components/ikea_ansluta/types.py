import esphome.codegen as cg
from esphome.components import spi
import esphome.automation as automation

ikea_ansluta_ns = cg.esphome_ns.namespace('ikea_ansluta')
IkeaAnsluta = ikea_ansluta_ns.class_('IkeaAnsluta', cg.Component, spi.SPIDevice)

OnRemoteClickTrigger = ikea_ansluta_ns.class_('OnRemoteClickTrigger',
                                              automation.Trigger.template(cg.uint16, cg.uint8))


EnablePairingModeAction = ikea_ansluta_ns.class_('EnablePairingModeAction', automation.Action)
DisableParingModeAction = ikea_ansluta_ns.class_('DisablePairingModeAction', automation.Action)