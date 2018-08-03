/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
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
#include <stdio.h>
#include "mbed.h"

#include "lorawan/lorastack/phy/LoRaPHYUS915Hybrid.h"
#include "lorawan/lorastack/phy/LoRaPHYUS915.h"
#include "lorawan/lorastack/phy/lora_phy_ds.h"
#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"

// Application helpers
#include "trace_helper.h"
#include "lora_radio_helper.h"

using namespace events;

uint8_t tx_buffer[LORAMAC_PHY_MAXPAYLOAD];
uint8_t rx_buffer[LORAMAC_PHY_MAXPAYLOAD];

/*
 * Sets up an application dependent transmission timer in ms. Used only when Duty Cycling is off for testing
 */
#define TX_TIMER                        10000

/**
 * Maximum number of events for the event queue.
 * 16 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS            16

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER     3
/**
* This event queue is the global event queue for both the
* application and stack. To conserve memory, the stack is designed to run
* in the same thread as the application and the application is responsible for
* providing an event queue to the stack that will be used for ISR deferment as
* well as application information event queuing.
*/
static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS * EVENTS_EVENT_SIZE);

/**
 * Event handler.
 *
 * This will be passed to the LoRaWAN stack to queue events for the
 * application which in turn drive the application.
 */
static void lora_event_handler(lorawan_event_t event);

/**
 * Constructing Mbed LoRaWANInterface and passing it down the radio object.
 */
//static LoRaWANInterface lorawan(radio);


LoRaWANTimeHandler lora_time;

LoRaPHYUS915 lora_phy(lora_time);
/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;

Ticker flipper;

DigitalOut led2(LED1);
int count = 0;

#define TX_POWER        0
#define PKT_LENGHT      25


void flip() {

    char tx_buffer[PKT_LENGHT];
    bool config_status;
    tx_config_params_t tx_config;
    int8_t tx_power = TX_POWER;
    lorawan_time_t tx_toa = 0;
//    rx_config_params_t rx_window1_config, rx_window2_config;

    led2 = !led2;
/*

    lora_phy.compute_rx_win_params(10, 6,10, &rx_window1_config);

    lora_phy.compute_rx_win_params(8, 6, 10, &rx_window2_config);
*/

    sprintf(tx_buffer, "ola mundo ------ --- %d", count++);


    tx_config.channel = 8;
    tx_config.datarate = 0;
    tx_config.tx_power = TX_POWER;
    tx_config.max_eirp = 0;
    tx_config.antenna_gain = 0;
    tx_config.pkt_len = PKT_LENGHT;

    config_status = lora_phy.tx_config(&tx_config, &tx_power, &tx_toa);

    if(config_status == true){
        printf("Send data with success\n");
    }else{
        printf("Problem to send data\n" );
    }
    lora_phy.handle_send((uint8_t *)tx_buffer, PKT_LENGHT);
    printf("\n\n\%s - retcode\n", tx_buffer);
/*
    lora_phy.rx_config(&rx_window1_config);
    lora_phy.setup_rx_window(rx_window1_config.is_rx_continuous, 3000);

    lora_phy.rx_config(&rx_window2_config);
    lora_phy.setup_rx_window(rx_window2_config.is_rx_continuous, 3000);*/

}


/**
 * Entry point for application
 */
int main (void)
{
    // setup tracing
    setup_trace();

    printf("\n\n\nInitializing the program\n");

    lora_phy.set_radio_instance(radio);

    lora_phy.setup_public_network_mode(MBED_CONF_LORA_PUBLIC_NETWORK);

    flipper.attach(&flip, 1.0);

    return 0;
}
