<!--
#
#   Copyright 2020 Casper Meijn <casper@meijn.net>
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
-->

# mynewt bsp for PineTime

## Overview

This is a board support package for the Pine64 PineTime smartwatch. 

## Status

Currently the status is: incomplete.

Available packages:

- hw/bsp/pinetime
- hw/drivers/battery_pinetime

Once completed my intention is to merge into apache-mynewt-core.

## Usage

This repository contains some targets to test the board support. These can be build and run on the PineTime device.

It should also be possible to include this repo into other projects and just use the bsp part.
