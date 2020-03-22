/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>

#include "sysinit/sysinit.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "battery_pinetime/battery_pinetime.h"
#include "sgm4056/sgm4056.h"
#include "console/console.h"
#ifdef ARCH_sim
#include "mcu/mcu_sim.h"
#endif

static volatile int g_task1_loops;

/* For LED toggling */
int g_led_pin;

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

/**
 * main
 *
 * The main task for the project. This function initializes packages,
 * and then blinks the BSP LED in a loop.
 *
 * @return int NOTE: this function should never return!
 */
int
main(int argc, char **argv)
{
    int rc;
    charge_control_status_t charger_status;

#ifdef ARCH_sim
    mcu_sim_parse_args(argc, argv);
#endif

    sysinit();

    g_led_pin = LCD_BACKLIGHT_HIGH_PIN;
    hal_gpio_init_out(g_led_pin, 1);

    battery_pinetime_init();

    charger = (struct sgm4056_dev *) os_dev_open("charger", 0, 0);

    while (1) {
        ++g_task1_loops;

        /* Wait one second */
        os_time_delay(OS_TICKS_PER_SEC);

        /* Toggle the LED */
        hal_gpio_toggle(g_led_pin);

        /* Display charger status */
        rc = sgm4056_get_charger_status(charger, &charger_status);
        assert(rc == 0);

        console_printf("Charger state = %i = %s\n", 
            charger_status, get_charger_status_string(charger_status));
        
        /* Display battery voltage */
        int voltage = battery_pinetime_get_voltage();
        console_printf("Battery voltage = %i\n", voltage);

        console_flush();
    }
    assert(0);

    return rc;
}

