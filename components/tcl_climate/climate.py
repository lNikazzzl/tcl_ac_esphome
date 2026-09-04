import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, uart, switch
from esphome.const import CONF_ID

DEPENDENCIES = ['uart']
AUTO_LOAD = ['climate', 'switch']

tcl_climate_ns = cg.esphome_ns.namespace('tcl_climate')
TCLClimate = tcl_climate_ns.class_('TCLClimate', climate.Climate, uart.UARTDevice, cg.PollingComponent)
TCLDisplaySwitch = tcl_climate_ns.class_('TCLDisplaySwitch', switch.Switch, cg.Component)
TCLBeepSwitch = tcl_climate_ns.class_('TCLBeepSwitch', switch.Switch, cg.Component)

CONF_DISPLAY_SWITCH = 'display_switch'
CONF_BEEP_SWITCH = 'beep_switch'

CONFIG_SCHEMA = climate.climate_schema(TCLClimate).extend({
    cv.GenerateID(): cv.declare_id(TCLClimate),
    cv.Optional(CONF_DISPLAY_SWITCH): switch.switch_schema(TCLDisplaySwitch).extend({
        cv.GenerateID(): cv.declare_id(TCLDisplaySwitch),
    }),
    cv.Optional(CONF_BEEP_SWITCH): switch.switch_schema(TCLBeepSwitch).extend({
        cv.GenerateID(): cv.declare_id(TCLBeepSwitch),
    }),
}).extend(uart.UART_DEVICE_SCHEMA).extend(cv.polling_component_schema('450ms'))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    await uart.register_uart_device(var, config)

    if CONF_DISPLAY_SWITCH in config:
        conf = config[CONF_DISPLAY_SWITCH]
        sw = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(sw, conf)
        await switch.register_switch(sw, conf)
        cg.add(sw.set_parent(var))

    if CONF_BEEP_SWITCH in config:
        conf = config[CONF_BEEP_SWITCH]
        sw = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(sw, conf)
        await switch.register_switch(sw, conf)
        cg.add(sw.set_parent(var))
