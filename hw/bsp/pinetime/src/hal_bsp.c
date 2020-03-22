#include <inttypes.h>
#include <assert.h>
#include "syscfg/syscfg.h"
#include "sysflash/sysflash.h"
#include "hal/hal_system.h"
#include "hal/hal_flash_int.h"
#include "hal/hal_timer.h"
#include "hal/hal_bsp.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "mcu/nrf52_hal.h"
#include "mcu/nrf52_periph.h"
#include "sgm4056/sgm4056.h"
#include "debounce/debounce.h"
#include "button/button.h"

static struct sgm4056_dev os_bsp_charger;
static struct sgm4056_dev_config os_bsp_charger_config = {
    .power_presence_pin = CHARGER_POWER_PRESENCE_PIN,
    .charge_indicator_pin = CHARGER_CHARGE_PIN,
};


/** What memory to include in coredump. */
static const struct hal_bsp_mem_dump dump_cfg[] = {
    [0] = {
        .hbmd_start = &_ram_start,
        .hbmd_size = RAM_SIZE,
    }
};

const struct hal_bsp_mem_dump *
hal_bsp_core_dump(int *area_cnt)
{
    *area_cnt = sizeof(dump_cfg) / sizeof(dump_cfg[0]);
    return dump_cfg;
}

/**
 * Retrieves the flash device with the specified ID.  Returns NULL if no such
 * device exists.
 */
const struct hal_flash *
hal_bsp_flash_dev(uint8_t id)
{
    switch (id) {
    case 0:
        /* MCU internal flash. */
        return &nrf52k_flash_dev;

    default:
        /* External flash.  Assume not present in this BSP. */
        return NULL;
    }
}

/**
 * Retrieves the configured priority for the given interrupt. If no priority
 * is configured, returns the priority passed in.
 *
 * @param irq_num               The IRQ being queried.
 * @param pri                   The default priority if none is configured.
 *
 * @return uint32_t             The specified IRQ's priority.
 */
uint32_t
hal_bsp_get_nvic_priority(int irq_num, uint32_t pri)
{
    return pri;
}

void hal_bsp_button_callback(button_id_t id, uint8_t type, uint8_t flags) {
    if (type == BUTTON_ACTION && id == 1) {
        pinetime_button_action_callback(flags);
    }
}

debounce_pin_t hal_bsp_debounce;
button_t hal_bsp_button = {   
    .id       = 1,
    .mode     = BUTTON_MODE_TOUCH,
};
 
void hal_bsp_button_debounce_handler(debounce_pin_t *d) {
    button_t *button = (button_t *) d->arg;
    bool pressed = debounce_state(d);
    button_set_low_level_state(button, pressed);
}

void hal_bsp_button_init(void)
{
    int rc;
    rc = hal_gpio_init_out(PUSH_BUTTON_OUT_PIN, 1);
    assert(rc == 0);

    rc = debounce_init(&hal_bsp_debounce, PUSH_BUTTON_IN_PIN, HAL_GPIO_PULL_NONE, 0);
    assert(rc == 0);
    rc = debounce_start(&hal_bsp_debounce, DEBOUNCE_CALLBACK_EVENT_ANY, hal_bsp_button_debounce_handler, &hal_bsp_button);
    assert(rc == 0);
    
    button_init(&hal_bsp_button, 1, hal_bsp_button_callback);
}

void
hal_bsp_init(void)
{
    int rc;

    (void)rc;

    /* Make sure system clocks have started. */
    hal_system_clock_start();

    /* Create all available nRF52840 peripherals */
    nrf52_periph_create();

    /* Create charge controller */
    rc = os_dev_create(&os_bsp_charger.dev, "charger",
                       OS_DEV_INIT_KERNEL, OS_DEV_INIT_PRIO_DEFAULT,
                       sgm4056_dev_init, &os_bsp_charger_config);
    assert(rc == 0);

    /* Create button */
    hal_bsp_button_init();

}
