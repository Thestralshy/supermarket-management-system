/************************************************
 * 文件名 - login.c
 * 文件主要功能:
 *      提供登录界面、验证登录信息
 *      提供登录后的操作界面
*************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "login.h"
#include "cashier_main.h"
#include "boss_operation.h"
#include "cashier_operation.h"
#include "md5.h"
/*** 函数功能 ***
 * 选择登录方式
 * 1 - 管理员登录
 * 2 - 收银员登录
*/
int login_select()
{
    int how_login = 0;
    int cashier_number = 0;

    while (1)
    {
        
        printf("\n***欢迎使用夜骐收银系统***\n");
        printf("\n*    请选择登录方式      *");
        printf("\n*************************");
        printf("\n* 1 - im boss           *");
        printf("\n* 2 - im cashier        *");
        printf("\n*************************");
        printf("\n请选择登录方式:\n");
        printf("> ");
        
        scanf("%d", &how_login);
        while(getchar()!='\n');
        
        switch (how_login)
        {
            case 1:
                if(1 == login_display(boss_login))
                {
                    printf("\033[2J"); // 进入boss界面前清空内容
                    boss_display(); // 进入boss操作界面
                    printf("\033[2J"); // 退出boss界面后清空内容
                }
                break;
            
            case 2:
                if(1 != (cashier_number = login_display(cashier_login)))
                {   
                    // 核对登录信息
                    if(cashier_found->number == cashier_number)
                    {
                        printf("\033[2J"); // 进入收银界面前清空内容
                        cashier_display();// 进入收银员操作界面
                        printf("\033[2J"); // 退出收银界面后清空内容
                    }
                    
                }
                break;

            default:
                printf("\n输入序号不正确,请按回车键重新输入...");
                getchar();
                // printf("\033[2J");
                break;
        }
        how_login = 0;
    }
}

/**************************
 * 登录界面,输入账号密码
***************************/
int login_display(int(*login)(void* ,void* ))
{
    char username[128] = {0};
    char passwd[128] = {0};
    int cashier_number = 0;

    printf("\n请输入账号:");
    scanf("%s", username);
    
    printf("\n请输入密码:");
    
    system("stty -echo");// 关闭回显
    scanf("%s", passwd);
    system("stty echo");// 开启回显
    
    if(0 != (cashier_number = login(username, passwd)))
    {
        printf("\n登录成功!");
        return cashier_number; // 成功登录到用户
    }
    else
    {
        printf("\n用户名或密码错误，请按任意键重试!");
        
        while (getchar() != '\n');

        getchar();

        printf("\033[2J"); // 输入错误后回车清空内容
    }

}



/******************************
 * 管理员登录判断函数
 * 形参(管理员账号，管理员密码)
 * 返回值 是否成功登录
******************************/
int boss_login(void* username, void* passwd)
{
    FILE* fp = fopen(BOSS_MSG_FILE, "r+");
    if(NULL == fp)
    {
        perror("boss_login-fopen");
        return 0;
    }
    boss_info boss_tmp;
    char passwd_tmp[33];

    // 密码转换为指纹后判断
    md5_string(passwd, passwd_tmp);

    // 将管理员信息读取到内存中
    fread(&boss_tmp, 1, sizeof(boss_info), fp);
    
    fclose(fp);
    
    // 判断手动输入的账号密码与文件内是否相等
    if((!strcmp(boss_tmp.name, username)) 
    && (!strcmp(boss_tmp.pswd, passwd_tmp)))
    {
        boss_tmp.login_time++;// 登录次数+1
        
        FILE* fp_w = fopen(BOSS_MSG_FILE, "w");
        
        fwrite(&boss_tmp, 1, sizeof(boss_info), fp_w);// 写入新登录次数
        
        fclose(fp_w);

        return 1; // 返回 1 表示登陆成功,正常退出
    }

    
    return 0; // 0 表示账号或密码错误
}



/******************************
 * 收银员登录判断函数
 * 形参(收银员账号，收银员密码)
 * 返回值 是否成功登录
******************************/
int cashier_login(void* username, void* passwd)
{
    FILE* fp = fopen(CASHIER_MSG_FILE, "r+");

    if(NULL == fp)
    {
        perror("cashier_login-fopen");
        return 0;      // 文件打开失败，返回 0 表示失败
    }

    cashier_info cashier_tmp;

    char passwd_tmp[33];// 存放密码的指纹
    
    int i = 0;

    // 密码转换为指纹后判断
    md5_string(passwd, passwd_tmp);

    while (!feof(fp))
    {
        fread(&cashier_tmp, 1, sizeof(cashier_info), fp);

        if((!strcmp(cashier_tmp.uname, username)) 
        && (!strcmp(cashier_tmp.upass, passwd_tmp)))
        {
            // 登录次数增加 1 次
            cashier_tmp.login_time++;

            printf("登录成功!");
            // 将读取到的数据保存到全局变量中
            *cashier_found = cashier_tmp;

            // 将读取指针移动到本人信息开头
            fseek(fp, i * sizeof(cashier_info), SEEK_SET);
            
            // 然后将该登录次数保存进文件
            fwrite(&cashier_tmp, 1, sizeof(cashier_info), fp);

            // 将所有修改正式写入到文件！
            fclose(fp);

            return cashier_tmp.number; // 返回该用户的id号
        }
        i++; // 每读取一个用户则计数 +1
    }
    
    fclose(fp);
    return 0; // 账号或密码错误
}