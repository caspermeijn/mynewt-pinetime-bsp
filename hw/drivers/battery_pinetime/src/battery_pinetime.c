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

#include "battery_pinetime/battery_pinetime.h"

#include <adc/adc.h>
#include <nrf_saadc.h>
#include <nrfx_saadc.h>

nrfx_saadc_config_t adc_config = NRFX_SAADC_DEFAULT_CONFIG;

struct adc_dev *adc;

void *
battery_pinetime_init(void)
{
    nrf_saadc_channel_config_t cc = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);
    cc.gain = NRF_SAADC_GAIN1_4;
    cc.reference = NRF_SAADC_REFERENCE_INTERNAL;
    adc = (struct adc_dev *) os_dev_open("adc0", 0, &adc_config);
    assert(adc != NULL);
    adc_chan_config(adc, 0, &cc);
    return adc;
}

int
battery_pinetime_get_voltage(void)
{
    int measured_millivolt;
    int adc_result;
    int rc;

    // TODO: Make this async
    rc = adc_read_channel(adc, 0, &adc_result);
    assert(rc == 0);

    measured_millivolt = adc_result_mv(adc, 0, adc_result);

    // Compensate for the voltage divider between battery and adc pin
    return measured_millivolt * 2;
}
