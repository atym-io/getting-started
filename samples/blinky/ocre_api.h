#ifndef OCRE_API_H
#define OCRE_API_H

// --- GPIO API ---
#define OCRE_GPIO_DIR_INPUT  0
#define OCRE_GPIO_DIR_OUTPUT 1

#define OCRE_GPIO_PIN_RESET  0
#define OCRE_GPIO_PIN_SET    1

int ocre_gpio_init ();
int ocre_gpio_configure(int port, int pin, int direction);
int ocre_gpio_pin_set(int port, int pin, int state);
int ocre_gpio_pin_get(int port, int pin);
int ocre_gpio_pin_toggle(int port, int pin);
int ocre_gpio_register_callback( int port, int pin);
int ocre_gpio_unregister_callback(int port, int pin);

// --- Timer API ---
int ocre_timer_create(int id);
int ocre_timer_delete(int id);
int ocre_timer_start(int id, int interval, int is_periodic);
int ocre_timer_stop(int id);
int ocre_timer_get_remaining(int id);

// --- Sleep API ---
int ocre_sleep(int milliseconds);

// --- Sensor API ---
typedef int ocre_sensor_handle_t;
int ocre_sensors_init(void);
int ocre_sensors_discover(void);
int ocre_sensors_open(ocre_sensor_handle_t handle);
int ocre_sensors_get_handle(int sensor_id);
int ocre_sensors_get_channel_count(int sensor_id);
int ocre_sensors_get_channel_type(int sensor_id, int channel_index);
int ocre_sensors_read(int sensor_id, int channel_type);

#endif // OCRE_API_H