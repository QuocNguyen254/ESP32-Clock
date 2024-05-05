/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c-lcd.h"
#include "unistd.h"
#include "esp_timer.h"
#include "Date.h"
static const char *TAG = "i2c-simple-example";

char buffer[10];

int counter = 0 ;
int tmp;
int hour,minute,second;
CDate date;
/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

void Delay(int n,int m){
	for (int i =0;i<n;++i){
		for (int j = 0 ; j <m;++j){
			Delay(i,j);
		}
	}
}
void Digital_Clock(void *arg);
void Digital_Clock(void *arg){
	counter++;
	tmp = counter;
	hour = tmp/3600;
	tmp = tmp%3600;
	minute = tmp/60;
	tmp = tmp%60;
	second = tmp;
	
    sprintf(buffer, "%02d", hour);
    lcd_put_cur(0, 0);
    lcd_send_string(buffer);

    sprintf(buffer, "%02d", minute);
    lcd_put_cur(0, 3);
    lcd_send_string(buffer);

    sprintf(buffer, "%02d", second);
    lcd_put_cur(0, 6);
    lcd_send_string(buffer);
    
        
    sprintf(buffer, "%d", date.day);
    lcd_put_cur(0, 8);
    lcd_send_string(buffer);
    lcd_send_string("/");
    
    sprintf(buffer, "%d", date.month);
    lcd_send_string(buffer);
    lcd_send_string("/");
	   
    sprintf(buffer, "%d", date.year);
    lcd_send_string(buffer);
	   
    if (counter == 86400 ){
    	counter = 0;
    	CDate_Increment(&date);
	}
}	
void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
	
    lcd_init();
    lcd_clear();
	
//    Khoi tao timer
	const esp_timer_create_args_t periodic_timer_args ={
		.callback = &Digital_Clock,
		.name = "periodic"
	};
	esp_timer_handle_t periodic_timer;
	
	esp_timer_create(&periodic_timer_args,&periodic_timer);
	while (1){
	esp_timer_start_periodic(periodic_timer,1000000);
	}
	
}
