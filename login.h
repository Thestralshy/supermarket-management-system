#ifndef LOGIN_H
#define LOGIN_H

int login_select();
int login_display(int(*login)(void* ,void* ));
int boss_login(void* username, void* passwd);
int cashier_login(void* username, void* passwd);

#endif