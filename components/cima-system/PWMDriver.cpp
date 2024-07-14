#include <system/PWMDriver.h>
#include <system/Log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace cima::system {

    static const std::string PWM_LOGGER_NAME("PWMDriver");
    Log PWMDriver::LOG(PWM_LOGGER_NAME);
    bool PWMDriver::fade_func_installed = false;

    PWMDriver::PWMDriver(gpio_num_t pwmGpioPin, ledc_channel_t channel, bool inverted) 
            : pwmGpioPin(pwmGpioPin), channel(channel), resolution(LEDC_TIMER_13_BIT), inverted(inverted) {

        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE; //TODO unused
        ledc_timer.duty_resolution = resolution; //TODO unused
        ledc_timer.timer_num = LEDC_TIMER_0; //TODO unused
        ledc_timer.freq_hz = 1000;

        ledc_channel.gpio_num = this->pwmGpioPin;
        ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_channel.channel = this->channel;
        ledc_channel.intr_type = LEDC_INTR_DISABLE;
        ledc_channel.timer_sel = LEDC_TIMER_0;
        ledc_channel.duty = 0;

        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    }

    void PWMDriver::init(){
        if( ! fade_func_installed){
            ledc_fade_func_install(0);
            fade_func_installed = true;
        } else {
            LOG.warning("Calling PWMDriver init despite it has been initialized");
        }
        
        /* //A testing code to spot if PWM works
        //Start with the half of full duty
        int tmpRes = (resolution - 1);
        while (tmpRes) {
            update(0x01 << tmpRes);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            tmpRes -= 1;
        }
        */
    }

    void PWMDriver::update(uint32_t dutyCycle) {

        //TODO use different constants for ESP32 than tghoes ebelow for esp8266
        ledc_set_duty(LEDC_LOW_SPEED_MODE, this->channel, inverted ? (0x01 << resolution) - 1 - dutyCycle : dutyCycle);
	    ledc_update_duty(LEDC_LOW_SPEED_MODE, this->channel);
    }
}