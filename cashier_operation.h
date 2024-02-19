#ifndef USER_MSG_H
#define USER_MSG_H

#include <time.h>
#include "time2time.h"

#define CASHIER_MSG_FILE "./user_msg/cashier_info_file.dat"
#define ACT_OF_SALE_FILE "./user_msg/act_of_sale_file.dat"
typedef struct
{
    int number;             /* 收银员编号 */
    char uname[51];         /* 收银员用户名 */
    char relname[20];       /* 收银员真实姓名 */
    char upass[33];         /* 收银员密码 */
    char sex;               /* 收银员性别 */
    char phone[12];         /* 收银员电话号码 */
    char idcard[20];        /* 收银员身份证号 */
    char address[128];       /* 收银员住址 */
    time_t last_login_time; /* 最近一次登录时间 */
    time_t reg_time;        /* 注册时间 */
    int login_time;         /* 登录次数 */

    // ...
} cashier_info;



typedef struct
{
    char uname[51];         /* 收 银 员 */
    char number[14];        /* 商品条码 */
    float price;            /* 售卖单价 */
    int quantity;           /* 售卖数量 */
    float total;            /* 售卖总价 */
    bbtTM_S sale_time;      /* 售卖时间 */
    char remark[128];       /* 备    注 */
} sale_log;




cashier_info* cashier_found;
cashier_info* cashier_zero;
void cashier_operation_init();      /* 初始化函数 */

int find_cashier(cashier_info* ui);
int find_all_cashier();

/* 收银员用户 - 增 */
void create_cashier(void);
void create_cashier_for_file(const char* addfile);

/* 收银员用户 - 删 */
int delete_cashier();

/* 收银员用户 - 改 */
int changed_cashier();
int changed_cashier_active(cashier_info* ui);

/* 收银员用户 - 查 */
int find_get_cashier();

/* 销售记录 - 查 */
int act_of_sale();

/* 销售额 - 查 */
int sales_money();


void cashier_print(cashier_info* ui);


#endif