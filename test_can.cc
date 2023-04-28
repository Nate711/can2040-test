// source: https://gitea.predevolution-technologies.de/anme/CAN2040_Test

#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
extern "C" {
#include "can2040.h"
}
#include "RP2040.h"

#include <iostream>

static struct can2040 cbus;

static void can2040_cb(struct can2040 *cd, uint32_t notify,
                       struct can2040_msg *msg)
{
    // Add message processing code here...
}

static void PIOx_IRQHandler(void) { can2040_pio_irq_handler(&cbus); }

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 125000;
    uint32_t gpio_rx = 14, gpio_tx = 15;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

int main(void)
{

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    int32_t ledState = 0;
    stdio_init_all();

    canbus_setup();

    while (1)
    {
        // printf("bla\n");
        std::cout << "blah\n";
        gpio_put(LED_PIN, ledState);
        if (ledState == 0)
        {
            ledState = 1;
        }
        else
        {
            ledState = 0;
        }
        sleep_ms(1000);
    }
}

// #include <iostream>

// int main()
// {
//     std::cout<<"hi"<<"\n";
// }