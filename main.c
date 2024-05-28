#include <stdio.h>
#include "liquid_crystal.h"
#include "Date.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unistd.h"
#include "DHT.h"
char *hello = "Hello World!";
char buffer[10];
const uint8_t rs = 5, en = 18, d4 = 19, d5 = 21, d6 = 22, d7 = 23;

int cursor[2] = {0, 0};
bool cursor_flag = 0;

int counter = 0;
int tmp_counter = 0;
int tmp;
int tmp1; //For Stopwatch
int tmp_hour = 0;
int tmp_minute = 0;
int tmp_second = 0;
int tmp_day  ;
int tmp_month ;
int tmp_year ;
int hour, minute, second;

int change_flag = 0;
int press_flag = 1;
int press_flag1 = 1;

int alarm_flag= 0;
int alarm_flag_before = 0;
CDate date;
uint64_t start_time;
uint64_t end_time;
uint64_t elapsed_time;

#define ADD_BUT GPIO_NUM_25
#define SUB_BUT GPIO_NUM_26
// #define CUR_BUT GPIO_NUM_27
#define CHANGE_BUT GPIO_NUM_14
#define START_STOP_BUT GPIO_NUM_12
#define RESET_BUT GPIO_NUM_13

// bool gpio_value = 0;


void Digital_Clock(void *arg);
void Print_Date();
void Print_Number(int write_value,int collumn,int row){
  sprintf(buffer, "%02d", write_value);
  lcd_set_cursor(collumn, row);
  lcd_write_string(buffer);
}
void Print_String(char* str,int collumn,int row){
  lcd_set_cursor(collumn, row);
  lcd_write_string(str);
}
static void IRAM_ATTR gpio_isr_handler_1(void *arg)
{
  start_time = esp_timer_get_time();
  gpio_intr_disable(ADD_BUT);
  gpio_isr_handler_remove(ADD_BUT);

  date.year+=3;
  Print_Date(hour,minute,second,date.day,date.month,date.year);

  gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_1, NULL);
  gpio_intr_enable(ADD_BUT);

  end_time = esp_timer_get_time();
  elapsed_time = (double)(end_time - start_time);
}

static void IRAM_ATTR gpio_isr_handler_2(void *arg)
{
  start_time = esp_timer_get_time();
  gpio_intr_disable(SUB_BUT);
  gpio_isr_handler_remove(SUB_BUT);

  date.year-=3;
  Print_Date(hour,minute,second,date.day,date.month,date.year);

  gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_2, NULL);
  gpio_intr_enable(SUB_BUT);

  end_time = esp_timer_get_time();
  elapsed_time = (double)(end_time - start_time);
}

// static void IRAM_ATTR gpio_isr_handler_3(void *arg)
// {
//   start_time = esp_timer_get_time();
//   gpio_intr_disable(ADD_BUT);
//   gpio_isr_handler_remove(ADD_BUT);

//   if (cursor_flag == 0)
//   {
//     cursor_flag = 1;
//     cursor[0] = 12;
//     cursor[1] = 0;
//   }
//   else (cursor_flag == 1 && ())
//   {
//     cursor_flag = 0;
//     cursor[0] = 4;
//     cursor[1] = 0;
//   }
//   {
//     cursor[0] += 1
//     cursor[1] += 1;
//   }
  
//   lcd_cursor();
  
//   gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_3, NULL);
//   gpio_intr_enable(ADD_BUT);

//   end_time = esp_timer_get_time();
//   elapsed_time = (double)(end_time - start_time);
// }

static void IRAM_ATTR gpio_isr_handler_4(void *arg)
{
  start_time = esp_timer_get_time();
  gpio_intr_disable(CHANGE_BUT);
  gpio_isr_handler_remove(CHANGE_BUT);
  // while(1){
  //   if (press_flag==1){
  //   press_flag = 0;
  //   if (change_flag==2) change_flag = 0; 
  //   else change_flag+=1;

  //   if (change_flag==1){ //Chuc nang bam gio
  //       while(1){
  //         Print_String(30,4,0);
  //         Print_String(30,7,0);
  //         Print_String(30,10,0); 
  //       }

  //   }else if (change_flag==2){ // Chuc nang bao thuc 
  

  //   }else{
      
  //   }
 
  //   }else{
  //     press_flag=1;
  //     break;
  //   }
  // }
  while (1) {
        if (press_flag == 1) {
            press_flag = 0;
 
                while (1) {
                    Print_Number(30, 4, 0);
                    Print_Number(30, 7, 0);
                    Print_Number(30, 10, 0);
                    
                    // Thực hiện việc kiểm tra và thoát khỏi vòng lặp khi cần thiết
                    if (press_flag == 1) {
                        press_flag = 0;
                        break;
                    }
                }
 
        } else {
            press_flag = 1;
            break; // Thoát khỏi vòng lặp while(1)
        }
    }

  gpio_isr_handler_add(CHANGE_BUT, gpio_isr_handler_4, NULL);
  gpio_intr_enable(CHANGE_BUT);
  end_time = esp_timer_get_time();
  elapsed_time = (double)(end_time - start_time);

}
static void IRAM_ATTR gpio_isr_handler_5(void *arg)
{
    gpio_intr_disable(START_STOP_BUT);
    // gpio_isr_handler_remove(START_STOP_BUT);
    date.year++;
    // gpio_isr_handler_remove(START_STOP_BUT);

  // gpio_isr_handler_remove(CHANGE_BUT);

    // if (press_flag1==1){
    //   press_flag1=0;
    //   while(1){
    //     tmp_counter++;
    //     tmp1 = tmp_counter;
    //     tmp_hour = tmp / 3600;
    //     tmp1 = tmp % 3600;
    //     tmp_minute = tmp / 60;
    //     tmp1 = tmp % 60;
    //     tmp_second = tmp;
    //     Print_String(tmp_hour,4,0);
    //     Print_String(tmp_minute,7,0);
    //     Print_String(tmp_second,10,0);

    //     gpio_isr_handler_add(START_STOP_BUT, gpio_isr_handler_5, NULL);
    //     gpio_intr_enable(START_STOP_BUT);
    //   }
    // }else{
    //   press_flag1=1;     
    //   while(1){
    //   Print_String(tmp_hour,4,0);
    //   Print_String(tmp_minute,7,0);
    //   Print_String(tmp_second,10,0); 
    //   gpio_isr_handler_add(START_STOP_BUT, gpio_isr_handler_5, NULL);
    //   gpio_intr_enable(START_STOP_BUT);
    //   break;
    //   }
    // }
  gpio_isr_handler_add(START_STOP_BUT, gpio_isr_handler_5, NULL);
  gpio_intr_enable(START_STOP_BUT);
}
static void IRAM_ATTR gpio_isr_handler_6(void *arg)
{
  gpio_intr_disable(RESET_BUT);
  gpio_isr_handler_remove(RESET_BUT);
  tmp_hour =0;
  tmp_minute = 0;
  tmp_second = 0;
  gpio_isr_handler_add(RESET_BUT, gpio_isr_handler_6, NULL);
  gpio_intr_enable(RESET_BUT);

}

void Print_Date(int hour,int minute,int second,int day,int month,int year){

  sprintf(buffer, "%02d", hour);
  lcd_set_cursor(1, 0);
  lcd_write_string(buffer);

  sprintf(buffer, "%02d", minute);
  lcd_set_cursor(4, 0);
  lcd_write_string(buffer);

  sprintf(buffer, "%02d", second);
  lcd_set_cursor(7, 0);
  lcd_write_string(buffer);

  sprintf(buffer, "%02d", day);
  lcd_set_cursor(0, 1);
  lcd_write_string(buffer);

  sprintf(buffer, "%02d", month);
  lcd_set_cursor(3, 1);
  lcd_write_string(buffer);

  sprintf(buffer, "%04d", year);
  lcd_set_cursor(6, 1);
  lcd_write_string(buffer);
  int ret = readDHT();
  if (ret==DHT_OK){
    int temp = getTemperature();
    int hum = getHumidity();

    sprintf(buffer, "%d", hum);
    if (temp>=10){
        lcd_set_cursor(13, 0);
    }else{
        lcd_set_cursor(14, 0);
    }
    lcd_write_string(buffer);

    Print_String("%",15,0);

    sprintf(buffer, "%d", temp);
    if (temp>=10){
        lcd_set_cursor(13, 1);
    }else{
        lcd_set_cursor(14, 1);
    }
    lcd_write_string(buffer);
    Print_String("C",15,1);
  }

  lcd_set_cursor(cursor[0], cursor[1]);

}
 


void Digital_Clock(void *arg)
{
  start_time = esp_timer_get_time();
  counter++;
  tmp = counter;
  hour = tmp / 3600;
  tmp = tmp % 3600;
  minute = tmp / 60;
  tmp = tmp % 60;
  second = tmp;

  // if (change_flag!=1)
  Print_Date(hour,minute,second,date.day,date.month,date.year);  
  
  if (counter == 86400)
  {
    counter = 0;
    CDate_Increment(&date);
  }
  end_time = esp_timer_get_time();
  elapsed_time = (double)(end_time - start_time);
}

void app_main()
{
  // initialize the library by associating any needed LCD interface pin
  // with the pin number it is connected to
  liquid_crystal(rs, en, d4, d5, d6, d7);

  setDHTgpio(GPIO_NUM_17);
  // set up the LCD's number of columns and rows:
  lcd_begin(16, 2);
  lcd_set_cursor(1, 0);
  lcd_write_string("00:00:00");
  lcd_set_cursor(0, 1);
  lcd_write_string("00/00/0000");

  Print_String("H:",11,0);
  Print_String("T:",11,1);

  date.day = 0;
  date.month = 0;
  date.year = 0;
  //    Khoi tao timer

  gpio_reset_pin(ADD_BUT);
  gpio_reset_pin(SUB_BUT);
  gpio_reset_pin(CHANGE_BUT);
  gpio_reset_pin(START_STOP_BUT);
  gpio_reset_pin(RESET_BUT);

  gpio_set_direction(ADD_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(SUB_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(CHANGE_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(START_STOP_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(RESET_BUT, GPIO_MODE_INPUT);

  
  gpio_pullup_en(ADD_BUT);
  gpio_pullup_en(SUB_BUT);
  gpio_pullup_en(CHANGE_BUT);
  gpio_pullup_en(START_STOP_BUT);
  gpio_pullup_en(RESET_BUT);

  gpio_pulldown_dis(ADD_BUT);
  gpio_pulldown_dis(SUB_BUT);
  gpio_pulldown_dis(CHANGE_BUT);
  gpio_pulldown_dis(START_STOP_BUT);
  gpio_pulldown_dis(RESET_BUT);

  gpio_set_intr_type(ADD_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(SUB_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(CHANGE_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(START_STOP_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(RESET_BUT, GPIO_INTR_POSEDGE);

  gpio_install_isr_service(0);

  gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_1, NULL);
  gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_2, NULL);
  gpio_isr_handler_add(CHANGE_BUT, gpio_isr_handler_4, NULL);
  gpio_isr_handler_add(START_STOP_BUT, gpio_isr_handler_5, NULL);
  gpio_isr_handler_add(RESET_BUT, gpio_isr_handler_6, NULL);

 


  // int intr_alloc_flags1 = ESP_INTR_FLAG_LEVEL1;  // Set the desired priority level (lowest)
  // esp_err_t err1 = esp_intr_alloc(ETS_GPIO_INTR_SOURCE, intr_alloc_flags1,gpio_isr_handler_1, (void*)ADD_BUT , NULL);
  // esp_err_t err2 = esp_intr_alloc(ETS_GPIO_INTR_SOURCE, intr_alloc_flags1,gpio_isr_handler_2, (void*)SUB_BUT , NULL);
  // esp_err_t err3 = esp_intr_alloc(ETS_GPIO_INTR_SOURCE, intr_alloc_flags1,gpio_isr_handler_4, (void*)CHANGE_BUT , NULL);
  // int intr_alloc_flags2 = ESP_INTR_FLAG_LEVEL2;  // Set the desired priority level (lowest)  
  // esp_err_t err4 = esp_intr_alloc(ETS_GPIO_INTR_SOURCE, intr_alloc_flags2,gpio_isr_handler_5, (void*)START_STOP_BUT , NULL);
  // esp_err_t err5 = esp_intr_alloc(ETS_GPIO_INTR_SOURCE, intr_alloc_flags2,gpio_isr_handler_6, (void*)RESET_BUT , NULL);


  gpio_intr_enable(ADD_BUT);
  gpio_intr_enable(SUB_BUT);
  gpio_intr_enable(CHANGE_BUT);
  gpio_intr_enable(START_STOP_BUT);
  gpio_intr_enable(RESET_BUT);
  elapsed_time = 0;

  const esp_timer_create_args_t periodic_timer_args ={
    .callback = &Digital_Clock,
    .name = "periodic"
  };
  esp_timer_handle_t periodic_timer;

  esp_timer_create(&periodic_timer_args,&periodic_timer);
  while (1)
  {
    esp_timer_start_periodic(periodic_timer,1000000 - elapsed_time);
    elapsed_time = 0;
  }
}