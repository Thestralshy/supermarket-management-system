#ifndef CASHIER_MAIN_H
#define CASHIER_MAIN_H



#include "cashier_operation.h"

void cashier_display();
float get_money();
void give_money(float price_sum);
void information_show();
void cashier_mean(cashier_info* my_cashier);
void act_of_sale_show();
float sales_my_money();
void printf_sale_log(sale_log* sale_log_tmp);
int cashier_first_login();
int cashier_passwd_changed();


#endif