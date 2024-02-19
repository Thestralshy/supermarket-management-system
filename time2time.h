#ifndef TIME2TIME_H
#define TIME2TIME_H

#include <time.h>

#define uint32_t unsigned int

/* 时间结构体 */
typedef struct bbtTM {
	short tm_sec; /* 秒 – 取值区间为[0,59] */
	short tm_min; /* 分 - 取值区间为[0,59] */
	short tm_hour; /* 时 - 取值区间为[0,23] */
	short tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
	short tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
	short tm_year; /* 年份，其值等于实际年份 */
    time_t tm_dts; /* 时间戳 */
} bbtTM_S;

void bbt_localtime(uint32_t time, bbtTM_S *t);
int bbtIsLeap(int year);
uint32_t bbt_mktime(bbtTM_S dt);

/* 基于时间戳打印指定格式的时间 */
void print_time(time_t tm_dts);


#endif