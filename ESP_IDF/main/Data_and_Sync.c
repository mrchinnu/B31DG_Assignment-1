#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_timer.h"


#define PRODUCTION
#ifdef PRODUCTION
#define DEBUG 1
#else
#define DEBUG 1000
#endif

#define Alt_Mode_LED 16
#define Alt_Mode_Btn 22
#define Data_Wave_LED 32
#define Data_Wave_Btn 26
#define SYN_Alt_Mode_LED 15
#define ALPHA 1300
#define BETA 600
#define CIS 13
#define DELTA 500
#define T_S 50
#define FORMULA_50 50

static volatile bool mode = false;
static volatile bool mode1 = false;
static volatile unsigned long last_Enable_Interrupt_Time = 0;
static volatile unsigned long last_Select_Interrupt_Time = 0;
const unsigned long debounce_Delay_Interrupt = 250;

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *arg) {
    int pin = (int)arg;
    unsigned long now = esp_timer_get_time() / 1000;  // Convert to milliseconds
    if (pin == Alt_Mode_Btn && (now - last_Enable_Interrupt_Time >= debounce_Delay_Interrupt)) {
        mode = !mode;
        last_Enable_Interrupt_Time = now;
    } else if (pin == Data_Wave_Btn && (now - last_Select_Interrupt_Time >= debounce_Delay_Interrupt)) {
        mode1 = !mode1;
        last_Select_Interrupt_Time = now;
    }
    xQueueSendFromISR(gpio_evt_queue, &pin, NULL);
}

void gpio_task(void *arg) {
    int io_num;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI("GPIO", "Interrupt on GPIO %d", io_num);
        }
    }
}

void led_task(void *arg) {
    int al = ALPHA, be = BETA, ci, de = DELTA, x;
    while (1) {
        if (mode) {
            ci = CIS - 3;
        } else {
            ci = CIS;
        }
        gpio_set_level(Alt_Mode_LED, mode);
        x = 0;
        if (mode1) {
            gpio_set_level(SYN_Alt_Mode_LED, 1);
           esp_rom_delay_us(T_S * DEBUG);
            gpio_set_level(SYN_Alt_Mode_LED, 0);
            while (x < ci) {
                gpio_set_level(Data_Wave_LED, 1);
               esp_rom_delay_us(al * DEBUG);
                gpio_set_level(Data_Wave_LED, 0);
               esp_rom_delay_us(be * DEBUG);
                al = ALPHA + (x + 1) * FORMULA_50;
                if (x == ci - 1) {
                   esp_rom_delay_us(de * DEBUG);
                }
                x++;
                taskYIELD();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pin_bit_mask = (1ULL << Alt_Mode_Btn) | (1ULL << Data_Wave_Btn)
    };
    gpio_config(&io_conf);

    gpio_config_t output_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << Alt_Mode_LED) | (1ULL << Data_Wave_LED) | (1ULL << SYN_Alt_Mode_LED)
    };
    gpio_config(&output_conf);

    gpio_evt_queue = xQueueCreate(10, sizeof(int));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(Alt_Mode_Btn, gpio_isr_handler, (void *)Alt_Mode_Btn);
    gpio_isr_handler_add(Data_Wave_Btn, gpio_isr_handler, (void *)Data_Wave_Btn);

    ESP_LOGI("MAIN", "Started");
}