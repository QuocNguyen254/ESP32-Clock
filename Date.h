#ifndef _CDATE
#define _CDATE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
typedef struct {
    int day;
    int month;
    int year;
} CDate;

void CDate_init(CDate* date, int day, int month, int year);
bool CDate_NamNhuan(const CDate* date);
bool CDate_Thang30(const CDate* date);
bool CDate_Thang31(const CDate* date);
bool CDate_Thang2(const CDate* date);
void CDate_Cong(CDate* date, int days);
int CDate_TotalTime(const CDate* date);
void CDate_input(CDate* date);
void CDate_output(const CDate* date);
void CDate_Increment(CDate* date);
void CDate_Decrement(CDate* date);
void CDate_Month_Change(CDate *date,int num);
void CDate_Year_Change(CDate *date, int num);

#endif // _CDATE

