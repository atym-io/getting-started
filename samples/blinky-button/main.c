#include <stdio.h>
#include <stdbool.h>
#include "ocre_api.h"

// Red LED: P0_14
#define PORT_LED_RED        0
#define PIN_LED_RED         14

// Green LED: P2_12
#define PORT_LED_GREEN      2
#define PIN_LED_GREEN       12

// Button: P4_0
#define PORT_BTN            4
#define PIN_BTN             0
#define BTN_PIN_ID          64

// Timers
#define TIMER_ID_RED        1
#define TIMER_ID_GREEN      2
#define INTERVAL_RED        500
#define INTERVAL_GREEN      100

volatile bool button_event = false;

void set_red_led(bool on) {
    ocre_gpio_pin_set(PORT_LED_RED, PIN_LED_RED, on ? OCRE_GPIO_PIN_SET : OCRE_GPIO_PIN_RESET);
}

void set_green_led(bool on) {
    ocre_gpio_pin_set(PORT_LED_GREEN, PIN_LED_GREEN, on ? OCRE_GPIO_PIN_SET : OCRE_GPIO_PIN_RESET);
}

OCRE_EXPORT("timer_callback")
void exported_timer_callback(int timer_id) {
    static bool red_state = false;
    static bool green_state = false;

    if (timer_id == TIMER_ID_RED) {
        red_state = !red_state;
        green_state = false;
        set_red_led(red_state);
        printf("R %c\r", red_state ? '+' : '.');
    } else if (timer_id == TIMER_ID_GREEN) {
        green_state = !green_state;
        red_state = false;
        set_green_led(green_state);
        printf("G %c\r", green_state ? '+' : '.');
    }
    fflush(stdout);
}

OCRE_EXPORT("gpio_callback")
void exported_gpio_callback(int pin, int state) {
    if (pin == BTN_PIN_ID) {
        static int prev_state = -1;
        if (state != prev_state && state == 0) {
            button_event = true;
        }
        prev_state = state;
    }
}

int main() {
    printf("Dual-timer blink demo starting...\n");

    ocre_gpio_init();

    ocre_gpio_configure(PORT_LED_RED, PIN_LED_RED, OCRE_GPIO_DIR_OUTPUT);
    ocre_gpio_configure(PORT_LED_GREEN, PIN_LED_GREEN, OCRE_GPIO_DIR_OUTPUT);
    
    ocre_gpio_configure(PORT_BTN, PIN_BTN, OCRE_GPIO_DIR_INPUT);
    ocre_gpio_register_callback(PORT_BTN, PIN_BTN);

    ocre_timer_create(TIMER_ID_RED);
    ocre_timer_create(TIMER_ID_GREEN);

    // Start with red blinking
    ocre_timer_start(TIMER_ID_RED, INTERVAL_RED, true);
    ocre_timer_stop(TIMER_ID_GREEN);
    set_green_led(false);

    while (true) {
        if (button_event) {
            button_event = false;
            
            static bool red_running = true;
            red_running = !red_running;

            if (red_running) {
                ocre_timer_stop(TIMER_ID_RED);
                set_red_led(false);
                ocre_timer_start(TIMER_ID_GREEN, INTERVAL_GREEN, true);
                printf("Switched to GREEN (100ms)\n");
            } else {
                ocre_timer_stop(TIMER_ID_GREEN);
                set_green_led(false);
                ocre_timer_start(TIMER_ID_RED, INTERVAL_RED, true);
                printf("Switched to RED (500ms)\n");
            }
            fflush(stdout);
        }

        ocre_sleep(10);
    }

    return 0;
}
