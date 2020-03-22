/*
 * Copyright 2020 Casper Meijn <casper@meijn.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <string.h>

#include "sysinit/sysinit.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "battery_pinetime/battery_pinetime.h"
#include "sgm4056/sgm4056.h"
#include "button/button.h"
#include "console/console.h"
#ifdef ARCH_sim
#include "mcu/mcu_sim.h"
#endif

struct sgm4056_dev *charger;

char * get_charger_status_string(charge_control_status_t status) {
    static char * no_source_string = "no source detected";
    static char * charging_string = "charging";
    static char * complete_string = "charge completed";
    switch (status)
    {   
    case CHARGE_CONTROL_STATUS_NO_SOURCE:
        return no_source_string;

    case CHARGE_CONTROL_STATUS_CHARGING:
        return charging_string;

    case CHARGE_CONTROL_STATUS_CHARGE_COMPLETE:
        return complete_string;

    default:
        return NULL;
    }
}

void pinetime_button_action_callback(uint8_t flags) {
    if (flags & BUTTON_FLG_MISSED) {
        console_printf("some action was lost");
    }
    console_printf("BUTTON ");
    if (flags & BUTTON_FLG_PRESSED) {
        console_printf("ACTION=");
        if (flags & BUTTON_FLG_LONG) {
            console_printf("long ");
        }
        if (flags & BUTTON_FLG_DOUBLED) {
            console_printf("double ");   
        }
        console_printf("click");
        console_printf("\n");
    }
}

static void periodic_callback(struct os_event *ev);
static struct os_callout periodic_callout;

static void periodic_init()
{
    int rc;

    rc = hal_gpio_init_out(LCD_BACKLIGHT_HIGH_PIN, 1);
    assert(rc == 0);

    charger = (struct sgm4056_dev *) os_dev_open("charger", 0, 0);
    assert(charger);

    os_callout_init(&periodic_callout, os_eventq_dflt_get(),
                    periodic_callback, NULL);

    os_callout_reset(&periodic_callout, 0);
}

static void periodic_callback(struct os_event *ev)
{
    int rc;
    charge_control_status_t charger_status;

    /* Toggle the LED */
    hal_gpio_toggle(LCD_BACKLIGHT_HIGH_PIN);

    /* Display charger status */
    rc = sgm4056_get_charger_status(charger, &charger_status);
    assert(rc == 0);

    console_printf("Charger state = %i = %s\n", 
        charger_status, get_charger_status_string(charger_status));
    
    /* Display battery voltage */
    int voltage = battery_pinetime_get_voltage();
    console_printf("Battery voltage = %i\n", voltage);

    console_flush();

    /* Trigger in another second */
    os_callout_reset(&periodic_callout, OS_TICKS_PER_SEC);
}

int
main(int argc, char **argv)
{
    int rc;

#ifdef ARCH_sim
    mcu_sim_parse_args(argc, argv);
#endif

    sysinit();

    battery_pinetime_init();

    periodic_init();

    while (1) {
       os_eventq_run(os_eventq_dflt_get());
    }
    
    assert(0);

    return rc;
}

