#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c-lcd.h"
#include "unistd.h"
#include "esp_timer.h"
#include "Date.h"
#include <time.h>
//Moi them vao
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
     
static const char *TAG = "i2c-simple-example";

char buffer[50];

int counter = 0 ;
int tmp;
int hour,minute,second;
CDate date;
uint64_t start_time;
uint64_t end_time;
uint64_t elapsed_time;
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
void Print_Weather_Data(char* ssid,char* password,String URL,String ApiKey,String lat,String lon){

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
	    delay(500);
	    lcd_put_cur(0, 4);
	    lcd_send_string(".");
	}
	
	if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    //Set HTTP Request Final URL with Location and API key information
    http.begin(URL + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + ApiKey);

    // start connection and send HTTP Request
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {

	      //Read Data as a JSON string
	      String JSON_Data = http.getString();
	
	      //Retrieve some information about the weather from the JSON format
	      DynamicJsonDocument doc(2048);
	      deserializeJson(doc, JSON_Data);
	      JsonObject obj = doc.as<JsonObject>();
	
	      //Display the Current Weather Info
	      const char* description = obj["weather"][0]["description"].as<const char*>();
	      const float temp = obj["main"]["temp"].as<float>();
	      const float humidity = obj["main"]["humidity"].as<float>();
	
	      lcd.clear();
	      lcd_put_cur(0,1);
	      sprintf(buffer, "%s", description);	      
	      lcd.setCursor(1, 0);
	      sprintf(buffer, "%d", temp);
	      lcd_send_string(" C, ");
	      sprintf(buffer, "%d", humidity);
	      lcd_send_string(" %");
	
	    } else {
	      Serial.println("Error!");
	      lcd_send_string("Error!");
	      lcd.clear();
	      lcd_send_string("Can't Get DATA!");
	    }
	
	    http.end();

  	}
}
void Digital_Clock(void *arg);
void Digital_Clock(void *arg){
	start_time = esp_timer_get_time();
	counter++;
	tmp = counter;
	hour = tmp/3600;
	tmp = tmp%3600;
	minute = tmp/60;
	tmp = tmp%60;
	second = tmp;
	
    sprintf(buffer, "%02d", hour);
    lcd_put_cur(0, 4);
    lcd_send_string(buffer);

    sprintf(buffer, "%02d", minute);
    lcd_put_cur(0, 7);
    lcd_send_string(buffer);

    sprintf(buffer, "%02d", second);
    lcd_put_cur(0, 10);
    lcd_send_string(buffer);
    
        
	sprintf(buffer, "%02d", date.day);
    lcd_put_cur(1, 3);
    lcd_send_string(buffer);

	sprintf(buffer, "%02d", date.month);
    lcd_put_cur(1, 6);
    lcd_send_string(buffer);
       
	sprintf(buffer, "%04d", date.year);
    lcd_put_cur(1, 9);
    lcd_send_string(buffer);
	   
    if (counter == 86400 ){
    	counter = 0;
    	CDate_Increment(&date);
	}
	end_time = esp_timer_get_time();
    elapsed_time = end_time - start_time;
}	
void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
	
    lcd_init();
    
    // Cac bien de lay du lieu thoi tiet
	const char* ssid = "yourssid"; //sID Ten mang Wifi ket noi	
	const char* password = "yourpassword";	//Mat khau mang Wifi can ket noi
	
	String URL = "https://api.openweathermap.org/data/2.5/weather?lat=10.905124435118228&lon=106.76969263664785&appid=edb241abb24109d87f94bf90b89f38bf";
	String ApiKey = "edb241abb24109d87f94bf90b89f38bf";
	
	String lat = "10.905124435118228"; 
	String lon = "106.76969263664785";
    // In du lieu thoi tiet
	Print_Weather_Data(ssid,password,URL,ApiKey,lat,lon);    
    
    lcd_clear();
    
    lcd_put_cur(0, 4);
    lcd_send_string("00:00:00");
    lcd_put_cur(1, 3);
    lcd_send_string("00/00/0000");
    date.day = 28;
   	date.month = 2;
   	date.year = 2023;
   // CDate date; 
//    Khoi tao timer
    counter = 86399;

	
	const esp_timer_create_args_t periodic_timer_args ={
		.callback = &Digital_Clock,
		.name = "periodic"
	};
	esp_timer_handle_t periodic_timer;
	
	esp_timer_create(&periodic_timer_args,&periodic_timer);
	while (1){
	esp_timer_start_periodic(periodic_timer,1000000 - elapsed_time );
	}
	
}
