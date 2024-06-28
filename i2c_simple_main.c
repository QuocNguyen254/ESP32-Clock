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
#include "Buzz.h"
#include "image.h"
char *hello = "Hello World!";
char buffer[10];
const uint8_t rs = 22, en = 21, d4 = 17, d5 = 16, d6 = 4, d7 = 2;

int cursor[2] = {0, 0};
int cursor2[2] = {0, 0};
bool cursor_flag = 0;
bool cursor_flag_2 = 0;
int cursor_press_flag = 0;
// int before_mode = 0;
int signtime = 0;

int clear_cld = 0;
int counter = 0;
int tmp_counter = 0;
int tmp;
int tmp1; // For Stopwatch
int tmp_hour = 0;
int tmp_minute = 0;
int tmp_second = 0;
int tmp_day;
int tmp_month;
int tmp_year;
int hour, minute, second;

int menu_flag = 1;
// int press_flag = 1;
int stop_flag = 1;

int alarm_flag = 0;
int alarm_value = 0;
int ring_flag = 0;
int indx = -1;
int cursor_a[3] = {24, 60, 60};
CDate date;
uint64_t start_time;
uint64_t end_time;
uint64_t elapsed_time;
uint64_t add_time;

#define ADD_BUT GPIO_NUM_25
#define SUB_BUT GPIO_NUM_26
#define CUR_BUT GPIO_NUM_27
#define CHANGE_BUT GPIO_NUM_14

void Set_clock();
void Change_time();
void Change_date();
void Print_Number(int write_value, int collumn, int row);
void Print_String(char *str, int collumn, int row);
void Digital_Clock1(void *arg);
void Print_Date(int hour, int minute, int second, int day, int month, int year);
void Repair_time();

void Repair_time()
{
  tmp = counter;
  hour = tmp / 3600;
  tmp = tmp % 3600;
  minute = tmp / 60;
  tmp = tmp % 60;
  second = tmp;

  if (counter >= 86400)
  {
    counter -= 86400;
    Repair_time();
    CDate_Increment(&date);
    lcd_clear();
    lcd_set_cursor(cursor[0], cursor[1]);
    if (cursor_flag == 1)
      lcd_cursor();
  }
  else if (counter < 0)
  {
    counter += 86400;
    Repair_time();
    CDate_Decrement(&date);
    lcd_clear();
    lcd_set_cursor(cursor[0], cursor[1]);
    if (cursor_flag == 1)
      lcd_cursor();
  }
}

void Set_clock()
{
  if (cursor[1] == 0)
  {
    Change_time();
  }
  else
  {
    Change_date();
  }
}

void Change_time()
{
  switch (cursor[0])
  {
  case 2:
    counter += 36000 * signtime;
    break;
  case 3:
    counter += 3600 * signtime;
    break;
  case 5:
    counter += 600 * signtime;
    break;
  case 6:
    counter += 60 * signtime;
    break;
  case 8:
    counter -= second;
    break;
  }
  Print_Date(hour, minute, second, date.day, date.month, date.year);
}

void Change_date()
{
  switch (cursor[0])
  {
  case 0:
    if (signtime == 1)
    {
      for (int i = 0; i < 10; i++)
        CDate_Increment(&date);
    }
    else
    {
      for (int i = 0; i < 10; i++)
        CDate_Decrement(&date);
    }
    break;
  case 1:
    if (signtime == 1)
      CDate_Increment(&date);
    else
      CDate_Decrement(&date);
    break;
  case 3:
    for (int i = 0; i < 10; i++)
      CDate_Month_Change(&date, signtime);
    break;
  case 4:
    CDate_Month_Change(&date, signtime);
    break;
  case 6:
    for (int i = 0; i < 1000; i++)
      CDate_Year_Change(&date, signtime);
    break;
  case 7:
    for (int i = 0; i < 100; i++)
      CDate_Year_Change(&date, signtime);
    break;
  case 8:
    for (int i = 0; i < 10; i++)
      CDate_Year_Change(&date, signtime);
    break;
  case 9:
    CDate_Year_Change(&date, signtime);
    break;
  }
}

void Print_Number(int write_value, int collumn, int row)
{
  sprintf(buffer, "%02d", write_value);
  lcd_set_cursor(collumn, row);
  lcd_write_string(buffer);
  // lcd_set_cursor(cursor[0], cursor[1]);
}

void Print_String(char *str, int collumn, int row)
{
  lcd_set_cursor(collumn, row);
  lcd_write_string(str);
  // lcd_set_cursor(cursor[0], cursor[1]);
}

static void IRAM_ATTR gpio_isr_handler_1(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
    signtime = 1;
    if (cursor_flag == 1)
    {

    gpio_intr_disable(ADD_BUT);
    gpio_isr_handler_remove(ADD_BUT);

    Set_clock();
    lcd_clear();
    Print_Date(hour, minute, second, date.day, date.month, date.year);

    gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_1, NULL);
    gpio_intr_enable(ADD_BUT);
    }
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}

static void IRAM_ATTR gpio_isr_handler_2(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
    signtime = -1;
    if (cursor_flag == 1)
    {
    gpio_intr_disable(SUB_BUT);
    gpio_isr_handler_remove(SUB_BUT);

    Set_clock();
    lcd_clear();
    Print_Date(hour, minute, second, date.day, date.month, date.year);

    gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_2, NULL);
    gpio_intr_enable(SUB_BUT);
    }
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}

static void IRAM_ATTR gpio_isr_handler_3(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(CUR_BUT);
  gpio_isr_handler_remove(CUR_BUT);
  cursor_press_flag = 1;
  if (cursor_flag == 0)
  {
    cursor_flag = 1;
    cursor[0] = 2;
    cursor[1] = 0;
    lcd_cursor();
  }
  else
  {
    if (cursor[0] >= 8 && cursor[1] == 0)
    {
      cursor[0] = 0;
      cursor[1] = 1;
    }
    else if (cursor[0] >= 9 && cursor[1] == 1)
    {
      lcd_no_cursor();
      cursor_flag = 0;
    }
    else
    {
      cursor[0]++;
    }
  }
  lcd_set_cursor(cursor[0], cursor[1]);
  gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_3, NULL);
  gpio_intr_enable(CUR_BUT);

  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}

static void IRAM_ATTR gpio_isr_handler_4(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  lcd_clear();
  if (menu_flag == 3)
  {
    menu_flag = 0;
    //Print_Date(hour, minute, second, date.day, date.month, date.year);
  }
  menu_flag++;
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}
static void IRAM_ATTR gpio_isr_handler_5(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(SUB_BUT);
  gpio_isr_handler_remove(SUB_BUT);
  if (stop_flag == 1)
  {
    stop_flag = 0;
  }
  else
  {
    stop_flag = 1;
  }
  gpio_intr_enable(SUB_BUT);
  gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_5, NULL);
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}
static void IRAM_ATTR gpio_isr_handler_6(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(SUB_BUT);
  gpio_isr_handler_remove(SUB_BUT);
  tmp_counter = 0;
  tmp_hour = 0;
  tmp_minute = 0;
  tmp_second = 0;
  gpio_intr_enable(SUB_BUT);
  gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_6, NULL);

  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}

static void IRAM_ATTR gpio_isr_handler_7(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(ADD_BUT);
  gpio_isr_handler_remove(ADD_BUT);
  if (cursor_flag_2 == 1)
  {
    if (indx == 0)
    {
      if (cursor_a[indx] >= 0 && cursor_a[indx] < 23)
        cursor_a[indx]++;
      else
        cursor_a[indx] = 0;
    }
    else if (indx == 1)
    {
      if (cursor_a[indx] >= 0 && cursor_a[indx] < 59)
        cursor_a[indx]++;
      else
        cursor_a[indx] = 0;
    }
    else if (indx == 2)
    {
      if (cursor_a[indx] >= 0 && cursor_a[indx] < 59)
        cursor_a[indx]++;
      else
        cursor_a[indx] = 0;
    }
  }
  alarm_value = cursor_a[0] * 3600 + cursor_a[1] * 60 + cursor_a[0];
  gpio_intr_enable(ADD_BUT);
  gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_7, NULL);
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}
static void IRAM_ATTR gpio_isr_handler_8(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(SUB_BUT);
  gpio_isr_handler_remove(SUB_BUT);
  if (cursor_flag_2 == 1)
  {
    if (indx == 0)
    {
      if (cursor_a[indx] > 0 && cursor_a[indx] <= 23)
        cursor_a[indx]--;
      else
        cursor_a[indx] = 23;
    }
    else if (indx == 1)
    {
      if (cursor_a[indx] > 0 && cursor_a[indx] <= 59)
        cursor_a[indx]--;
      else
        cursor_a[indx] = 59;
    }
    else if (indx == 2)
    {
      if (cursor_a[indx] > 0 && cursor_a[indx] <= 59)
        cursor_a[indx]--;
      else
        cursor_a[indx] = 59;
    }
  }
  alarm_value = cursor_a[0] * 3600 + cursor_a[1] * 60 + cursor_a[0];
  gpio_intr_enable(SUB_BUT);
  gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_8, NULL);
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}

static void IRAM_ATTR gpio_isr_handler_9(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(CUR_BUT);
  gpio_isr_handler_remove(CUR_BUT);
  if (alarm_flag == 0)
    alarm_flag = 1;
  lcd_cursor();
  if (indx == -1)
  {
    cursor_flag_2 = 1;
    cursor2[0] = 4;
    cursor2[1] = 0;
    indx++;
  }
  else if (indx == 0)
  {
    cursor2[0] = 7;
    cursor2[1] = 0;
    indx++;
  }
  else if (indx == 1)
  {
    cursor2[0] = 10;
    cursor2[1] = 0;
    indx++;
  }
  else{
    lcd_no_cursor();
    cursor_flag_2 = 0;
    indx = -1;
  }

  lcd_set_cursor(cursor2[0], cursor2[1]);
  gpio_intr_enable(CUR_BUT);
  gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_9, NULL);
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}

static void IRAM_ATTR gpio_isr_handler_10(void *arg)
{
  uint64_t start_add = esp_timer_get_time();
  gpio_intr_disable(CUR_BUT);
  gpio_isr_handler_remove(CUR_BUT);
  ring_flag = 0;
  gpio_intr_enable(CUR_BUT);
  gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_10, NULL);
  uint64_t end_add = esp_timer_get_time();
  add_time += (end_add - start_add);
}
void Print_Date(int hour, int minute, int second, int day, int month, int year)
{
  lcd_no_cursor();
  Repair_time();
  Print_String(":", 4, 0);
  Print_String(":", 7, 0);
  Print_String("/", 2, 1);
  Print_String("/", 5, 1);
  Print_String("H:", 11, 0);
  Print_String("T:", 11, 1);

  sprintf(buffer, "%02d", hour);
  lcd_set_cursor(2, 0);
  lcd_write_string(buffer);

  sprintf(buffer, "%02d", minute);
  lcd_set_cursor(5, 0);
  lcd_write_string(buffer);

  sprintf(buffer, "%02d", second);
  lcd_set_cursor(8, 0);
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
  // if (ret == DHT_OK)
  // {
  int temp = getTemperature();
  int hum = getHumidity();

  sprintf(buffer, "%d", hum);
  if (temp >= 10)
  {
    lcd_set_cursor(13, 0);
  }
  else
  {
    lcd_set_cursor(14, 0);
  }
  lcd_write_string(buffer);

  Print_String("%", 15, 0);

  sprintf(buffer, "%d", temp);
  if (temp >= 10)
  {
    lcd_set_cursor(13, 1);
  }
  else
  {
    lcd_set_cursor(14, 1);
  }
  lcd_write_string(buffer);
  Print_String("C", 15, 1);
  // }
  if (cursor_flag == 1)
    lcd_cursor();
  lcd_set_cursor(cursor[0], cursor[1]);
}

void Digital_Clock1(void *arg)
{
  start_time = esp_timer_get_time();
  if (stop_flag == 0)
    tmp_counter++;

  if (counter == alarm_value)
  {
    // Chuong reo ;
    ring_flag = 1;
  }
  if (ring_flag == 1)
  { // Chuong reo
    buzzer_SetDuty_On();
    add_clock_img();
  }
  else
  {
    // Tat chuong
    buzzer_SetDuty_Off();
    lcd_set_cursor(0, 0);
    lcd_write_string("  ");
    lcd_set_cursor(cursor[0], cursor[1]);
  }
  counter++;

  if (menu_flag == 1)
  { // Che do man hinh chinh
    // if (before_mode == 3 && cursor_press_flag == 0)
    // {
    //   lcd_no_cursor();
    // }
    Print_Date(hour, minute, second, date.day, date.month, date.year);
    gpio_isr_handler_remove(SUB_BUT);
    gpio_isr_handler_remove(ADD_BUT);
    gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_1, NULL);
    gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_2, NULL);
    gpio_isr_handler_remove(CUR_BUT);
    if (ring_flag == 1)
    {
      gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_10, NULL);
    }
    else
    {
      gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_3, NULL);
    }
  }
  else if (menu_flag == 2)
  {
    // Che do bao thuc

    gpio_isr_handler_remove(ADD_BUT);
    gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_7, NULL);
    gpio_isr_handler_remove(SUB_BUT);
    gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_8, NULL);
    gpio_isr_handler_remove(CUR_BUT);
    if (ring_flag == 1)
    {
      gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_10, NULL);
    }
    else
    {
      gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_9, NULL);
    }
    if (alarm_flag == 0)
    {
      cursor_a[0] = hour;
      cursor_a[1] = minute;
      cursor_a[2] = second;
      Print_Number(hour, 4, 0);
      Print_Number(minute, 7, 0);
      Print_Number(second, 10, 0);
    }
    else
    {
      Print_Number(cursor_a[0], 4, 0);
      Print_Number(cursor_a[1], 7, 0);
      Print_Number(cursor_a[2], 10, 0);
      alarm_value = cursor_a[0] * 3600 + cursor_a[1] * 60 + cursor_a[0];
    }
    Print_String("ALARM", 5, 1);
    Print_String(":", 6, 0);
    Print_String(":", 9, 0);
    lcd_set_cursor(cursor2[0], cursor2[1]);
  }
  else
  {
    // Che do bam gio
    gpio_isr_handler_remove(ADD_BUT);
    gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_5, NULL);
    gpio_isr_handler_remove(SUB_BUT);
    gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_6, NULL);
    gpio_isr_handler_remove(CUR_BUT);
    gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_10, NULL);
    lcd_no_cursor();
    lcd_set_cursor(cursor[0], cursor[1]);
    if (stop_flag == 0)
    {
      tmp1 = tmp_counter;
      tmp_hour = tmp1 / 3600;
      tmp1 = tmp1 % 3600;
      tmp_minute = tmp1 / 60;
      tmp1 = tmp1 % 60;
      tmp_second = tmp1;
    }

    Print_Number(tmp_hour, 4, 0);
    Print_Number(tmp_minute, 7, 0);
    Print_Number(tmp_second, 10, 0);
    Print_String(":", 6, 0);
    Print_String(":", 9, 0);
    Print_String("STOPWATCH", 3, 1);
    lcd_set_cursor(cursor[0], cursor[1]);
    // before_mode = menu_flag;
    cursor_press_flag = 0;
  }

  end_time = esp_timer_get_time();
  elapsed_time = (end_time - start_time);
}

void app_main()
{
  // initialize the library by associating any needed LCD interface pin
  // with the pin number it is connected to
  liquid_crystal(rs, en, d4, d5, d6, d7);

  setDHTgpio(GPIO_NUM_32);
  // set up the LCD's number of columns and rows:
  lcd_begin(16, 2);
  // set up custom character
  config_images();
  // initialize buzzer
  buzzer_init();

  gpio_reset_pin(ADD_BUT);
  gpio_reset_pin(SUB_BUT);
  gpio_reset_pin(CUR_BUT);
  gpio_reset_pin(CHANGE_BUT);

  gpio_set_direction(ADD_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(SUB_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(CUR_BUT, GPIO_MODE_INPUT);
  gpio_set_direction(CHANGE_BUT, GPIO_MODE_INPUT);

  gpio_pullup_en(ADD_BUT);
  gpio_pullup_en(SUB_BUT);
  gpio_pullup_en(CUR_BUT);
  gpio_pullup_en(CHANGE_BUT);

  gpio_pulldown_dis(ADD_BUT);
  gpio_pulldown_dis(SUB_BUT);
  gpio_pulldown_dis(CUR_BUT);
  gpio_pulldown_dis(CHANGE_BUT);

  gpio_set_intr_type(ADD_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(SUB_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(CUR_BUT, GPIO_INTR_POSEDGE);
  gpio_set_intr_type(CHANGE_BUT, GPIO_INTR_POSEDGE);

  gpio_install_isr_service(0);

  gpio_isr_handler_add(ADD_BUT, gpio_isr_handler_1, NULL);
  gpio_isr_handler_add(SUB_BUT, gpio_isr_handler_2, NULL);
  gpio_isr_handler_add(CUR_BUT, gpio_isr_handler_3, NULL);
  gpio_isr_handler_add(CHANGE_BUT, gpio_isr_handler_4, NULL);

  gpio_intr_enable(ADD_BUT);
  gpio_intr_enable(SUB_BUT);
  gpio_intr_enable(CUR_BUT);
  gpio_intr_enable(CHANGE_BUT);

  elapsed_time = 0;
  counter = 30;
  alarm_value = 34;

  date.day = 29;
  date.month = 2;
  date.year = 2024;

  const esp_timer_create_args_t periodic_timer_args = {
      .callback = &Digital_Clock1,
      .name = "periodic"};
  esp_timer_handle_t periodic_timer;

  esp_timer_create(&periodic_timer_args, &periodic_timer);
  while (1)
  {
    esp_timer_start_periodic(periodic_timer, 1000000 - elapsed_time);
    elapsed_time = 0;
    add_time = 0;
  }
}