#include "led_color.h"

void RGB_LED_INIT()  {
	    //red
	    // Prepare and then apply the LEDC PWM timer configuration
	    ledc_timer_config_t ledc_timer1 = {
	        .speed_mode       = LEDC_LOW_SPEED_MODE,
	        .timer_num        = LEDC_TIMER_0,
	        .duty_resolution  = LEDC_TIMER_13_BIT,
	        .freq_hz          = 5000,  // Set output frequency at 5 kHz
	        .clk_cfg          = LEDC_AUTO_CLK
	    };
	    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer1));
	    // Prepare and then apply the LEDC PWM channel configuration
	    ledc_channel_config_t ledc_channel1 = {
	        .speed_mode     = LEDC_LOW_SPEED_MODE,
	        .channel        = LEDC_CHANNEL_0,
	        .timer_sel      = LEDC_TIMER_0,
	        .intr_type      = LEDC_INTR_DISABLE,
	        .gpio_num       = 13,
	        .duty           = 0, // Set duty to 0%
	        .hpoint         = 0
	    };
	    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));
	    //green
	    // Prepare and then apply the LEDC PWM timer configuration
		ledc_timer_config_t ledc_timer2 = {
			.speed_mode       = LEDC_LOW_SPEED_MODE,
			.timer_num        = LEDC_TIMER_0,
			.duty_resolution  = LEDC_TIMER_13_BIT,
			.freq_hz          = 5000,  // Set output frequency at 5 kHz
			.clk_cfg          = LEDC_AUTO_CLK
		};
		ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer2));

		// Prepare and then apply the LEDC PWM channel configuration
		ledc_channel_config_t ledc_channel2 = {
			.speed_mode     = LEDC_LOW_SPEED_MODE,
			.channel        = LEDC_CHANNEL_1,
			.timer_sel      = LEDC_TIMER_0,
			.intr_type      = LEDC_INTR_DISABLE,
			.gpio_num       = 12,
			.duty           = 0, // Set duty to 0%
			.hpoint         = 0
		};
		ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));

		//blue
		// Prepare and then apply the LEDC PWM timer configuration
		ledc_timer_config_t ledc_timer3 = {
			.speed_mode       = LEDC_LOW_SPEED_MODE,
			.timer_num        = LEDC_TIMER_0,
			.duty_resolution  = LEDC_TIMER_13_BIT,
			.freq_hz          = 5000,  // Set output frequency at 5 kHz
			.clk_cfg          = LEDC_AUTO_CLK
		};
		ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer3));

		// Prepare and then apply the LEDC PWM channel configuration
		ledc_channel_config_t ledc_channel3 = {
			.speed_mode     = LEDC_LOW_SPEED_MODE,
			.channel        = LEDC_CHANNEL_2,
			.timer_sel      = LEDC_TIMER_0,
			.intr_type      = LEDC_INTR_DISABLE,
			.gpio_num       = 14,
			.duty           = 0, // Set duty to 0%
			.hpoint         = 0
		};
	    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel3));

}

void LED_COLOR(int Color)  {
	if(Color == 0)  {
		//WHITE
		// Set the LEDC peripheral configuration
		// Set duty to 50% red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
		// Set the LEDC peripheral configuration
		//  example_ledc_init2();
		// Set duty to 50%  green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));
		// Set the LEDC peripheral configuration
		//  example_ledc_init3();
		// Set duty to 50% blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
	}
	else if(Color == 1)  {
		//red
		// Set the LEDC peripheral configuration

		// Set duty to 100% red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

		// Set duty to 0% green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

		// Set duty to 0% blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));

	}
	else if(Color == 2)  {
		//green
		// Set the LEDC peripheral configuration

		// Set duty to 50%  red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

		// Set the LEDC peripheral configuration
		//  example_ledc_init2();
		// Set duty to 50%  green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

		// Set the LEDC peripheral configuration
		//  example_ledc_init3();
		// Set duty to 50%  blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
	}
	else if(Color == 3)  {
		//blue
		// Set the LEDC peripheral configuration

		// Set duty to 50%  red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

		// Set the LEDC peripheral configuration
		//  example_ledc_init2();
		// Set duty to 50%  green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

		// Set the LEDC peripheral configuration
		//  example_ledc_init3();
		// Set duty to 50%  blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
	}
	else if(Color == 4)  {
		//orange
		// Set the LEDC peripheral configuration
		// Set duty to 50%  red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
		// Set the LEDC peripheral configuration
		//  example_ledc_init2();
		// Set duty to 50%  green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 5333));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));
		// Set the LEDC peripheral configuration
		//  example_ledc_init3();
		// Set duty to 50%  blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
	}
	else if(Color == 5)  {
		//pink
		// Set the LEDC peripheral configuration
		// Set duty to 50%  red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

		// Set the LEDC peripheral configuration
		//  example_ledc_init2();
		// Set duty to 50%  green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

		// Set the LEDC peripheral configuration
		//  example_ledc_init3();
		// Set duty to 50%  blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 5333));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
	}
	else if(Color == 6)  {
		//yellow
		// Set the LEDC peripheral configuration

		// Set duty to 50%  red
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

		// Set the LEDC peripheral configuration
		//  example_ledc_init2();
		// Set duty to 50%  green
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 0));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

		// Set the LEDC peripheral configuration
		//  example_ledc_init3();
		// Set duty to 50%  blue
		ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 8000));
		// Update duty to apply the new value
		ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
	}
}



