#ifndef BOSS_OPERATION_H
#define BOSS_OPERATION_H



#define BOSS_MSG_FILE "./user_msg/boss.dat"
#define COMMODITY_MSG_FILE "./user_msg/commodity.dat"


typedef struct
{
    char name[128];
    char pswd[33];
    int login_time;         /* 登录次数 */
} boss_info;

/* 商品信息 */
typedef struct
{
    char number[14];/* 条码 */
    char name[33];  /* 名称 */
    float price;    /* 单价 */
    char factory[128];/* 厂家 */
    int total;    /* 进货时总数量 */
    int lessnumber;/* 剩余数量 */
} commodity;

int boss_msg_init();
void boss_display();
int boss_first_login();
int boss_pswd_changed_login();
void mean();
void boss_struct_init(boss_info* boss_msg);
int passwd_changed(boss_info* boss_msg);

void commodity_add();
int commodity_add_file(const void* comm_file);
int commodity_exist(commodity* user_commodity);
int commodity_del(const char* comm_number);
void commodity_change();
int commodity_find();
void commodity_find_all();
void commodity_show(commodity* comm_tmp);
#endif