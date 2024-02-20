#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cashier_operation.h"
#include "cashier_main.h"
#include "time2time.h"
#include "md5.h"

/*********************************
 * 初始化函数
 * 申请空间用于存放查找得到的数据
 * 申请空间用于存放 0 数据,用于判断是否找到
 * 检查文件完整性
*********************************/
void cashier_operation_init()
{
    cashier_found = malloc(sizeof(cashier_info));
    cashier_zero = malloc(sizeof(cashier_info));
    memset(cashier_zero, 0, sizeof(cashier_info));
}


/*********************************
 * 判断一个收银员是否存在于文件中
 * 
*********************************/
int find_cashier(cashier_info* ui)
{
    FILE* fp = fopen(CASHIER_MSG_FILE, "r");
    cashier_info tmp_ui;
    if(fp == NULL)
    {
        perror("find_cashier:fopen error");
        return -1;// 错误退出
    }

    while (!feof(fp))
    {
        fread(&tmp_ui, sizeof(cashier_info), 1, fp);
        
        if(0 == strcmp(tmp_ui.uname, ui->uname))
        {
            printf("\n用户名已存在！");
            return 0;// 该用户名已存在
        }
                
        if(tmp_ui.number == ui->number)
        {
            printf("\n编号已存在！请重新分配编号！");
            return 0;// 该编号存在
        }
    }
    return 1;// 未找到
}

/*********************************
 * 增加收银员:
 * 注册一个收银员并且保存到文件中
 * 
*********************************/
void create_cashier(void)
{
    cashier_info ui;
    char ch_tmp = 0;
    char password[51],password_again[51];

    printf("\n*** 正在新增收银员 ***");

    /* 编号 - 信息录入 */
    printf("\n收银员 - 编号:");
    scanf("%d", &ui.number);
    while (getchar() != '\n');

    /* 账户 - 信息录入 */
    printf("\n收银员 - 账户:");
    scanf("%s", ui.uname);
    while (getchar() != '\n');

    /* 密码 - 信息录入 */
    printf("\n收银员 - 密码:");

    system("stty -echo");// 关闭回显
    scanf("%s", password);// 密码输入应当无回显
    system("stty echo");// 开启回显

    printf("\n收银员 - 再次输入密码:");

    system("stty -echo");// 关闭回显
    scanf("%s", password_again);// 密码输入应当无回显
    system("stty echo");// 开启回显

    if(strcmp(password, password_again))
    {
        printf("\n密码不一致,请检查输入.");
        return;
    }

    md5_string(password, ui.upass);

    /* 真实姓名 - 信息录入 */
    printf("\n收银员 - 真实姓名:");
    scanf("%s", ui.relname);
    while (getchar() != '\n');

    /* 性别 - 信息录入 */
    printf("\n收银员 - 性别(1-man 0-wuman):");
    scanf("%hhd", &ui.sex);
    while (getchar() != '\n');

    /* 手机号 - 信息录入 */
    printf("\n收银员 - 手机号:");
    scanf("%s", ui.phone);
    while (getchar() != '\n');

    /* 身份证号 - 信息录入 */
    printf("\n收银员 - 身份证号:");
    scanf("%s", ui.idcard);
    while (getchar() != '\n');

    /* 地址 - 信息录入 */
    printf("\n收银员 - 家庭住址:");
    scanf("%s", ui.address);
    while (getchar() != '\n');

    /* 判断是否存在 */
    if(find_cashier(&ui) == 0)
    {
        return;
    }
    /* 登录次数为 0 */
    ui.login_time = 0;

    /* 注册时间 */
    ui.reg_time = time(NULL);

    /* 最近一次登陆时间 */
    ui.last_login_time = 0;

    cashier_print(&ui);

    printf("\n注册信息如上，核对是否确定创建该用户Y/n:");

    ch_tmp = getchar();

    if(ch_tmp == 'Y' || ch_tmp == 'y')
    {
        /* 写入模块 */
        FILE* fp = fopen(CASHIER_MSG_FILE, "ab");

        if(NULL == fp)
        {
            perror("fopen");
            return;
        }

        /* 将结构体以二进制写入文件 */
        fwrite(&ui, 1, sizeof(cashier_info), fp);
        
        fclose(fp);
        printf("\n注册成功！");
    }
    else
    {
        printf("\n取消添加");
        return;
    }
}

/*********************************
 * 从文件当中增加收银员:
 * 注册收银员并以二进制形式保存到文件中
 * 
*********************************/
void create_cashier_for_file(const char* addfile)
{
    cashier_info cashier_tmp;
    
    char passwd[33];

    FILE* fp_r = fopen(addfile, "r");
    FILE* fp_a = fopen(CASHIER_MSG_FILE, "a");

    if(NULL == fp_r || NULL == fp_a)
    {
        perror("create_cashier_for_file-fopen");
        return;
    }

    while(!feof(fp_r))
    {
        fscanf(fp_r,"%d;%[^;];%[^;];%[^;];%c;%[^;];%[^;];%ld;%ld;%d;"
        , &cashier_tmp.number
        , cashier_tmp.uname
        , cashier_tmp.relname
        , passwd
        , &cashier_tmp.sex
        , cashier_tmp.phone
        , cashier_tmp.idcard
        , &cashier_tmp.last_login_time
        , &cashier_tmp.reg_time
        , &cashier_tmp.login_time);

        time(&cashier_tmp.reg_time);    /* 写入注册时的时间戳 */
        
        md5_string(passwd, cashier_tmp.upass);
        
        /* 判断是否存在 */
        if(find_cashier(&cashier_tmp) == 0)
        {
            printf("\n编号%d添加失败,该编号已存在", cashier_tmp.number);
            continue;
        }
        
        /* 若不存在则添加进文件 */
        fwrite(&cashier_tmp, 1, sizeof(cashier_info), fp_a);
    
        printf("\n编号%d添加成功", cashier_tmp.number);
    }
    fclose(fp_r);
    fclose(fp_a);
}

/*********************************
 * 删除收银员:
 * 删除文件中的目标收银员
 * 删除方案：
 * 新建临时文件为<目标文件.tmp>
 * 将所有非目标收银员写入到该文件
 * 移除源文件并修改文件名同源文件
*********************************/
int delete_cashier()
{
    char cashier_msg_file_tmp[128] = CASHIER_MSG_FILE;

    /* 方案:查找，获取其编号，删除编号对应的用户 */
    int del_cashier_number;

    if(0 == (del_cashier_number = find_get_cashier()))// 查找函数
    {
        printf("\n该角色不存在，无法删除！");
        return 1;
    }

    strcat(cashier_msg_file_tmp, ".tmp");
    
    FILE* fp_w = fopen(cashier_msg_file_tmp, "w");
    FILE* fp_r = fopen(CASHIER_MSG_FILE, "r");
    
    cashier_info cashier_tmp;

    while (!feof(fp_r))
    {   
        if(sizeof(cashier_info) == fread(&cashier_tmp, 1, sizeof(cashier_info), fp_r))
        {
            // 判断读取的这一个是不是目标删除员工
            if(del_cashier_number != cashier_tmp.number)
            {
                // 如果不是，则写入到另一个文件中
                fwrite(&cashier_tmp, 1, sizeof(cashier_info), fp_w);
            }
        }
    }
    fclose(fp_w);
    fclose(fp_r);

    remove(CASHIER_MSG_FILE);

    rename(cashier_msg_file_tmp, CASHIER_MSG_FILE);

    return 0;
}


/*********************************
 * 修改收银员信息:
 * 修改文件中的目标收银员部分信息
 * 查找方案：
 *      检索:by编号
 * 修改方案:
 *      局部修改
*********************************/
int changed_cashier()
{
    cashier_info cashier_tmp, cashier_read;
    int num_of_cashier = 0;

    printf("\n请输入你要修改的成员编号:");
    scanf("%d", &cashier_tmp.number);
    while (getchar() != '\n');

    FILE* fp_r = fopen(CASHIER_MSG_FILE, "r");
    FILE* fp_a = fopen(CASHIER_MSG_FILE, "r+");
    if(NULL == fp_r || NULL == fp_a)
    {
        perror("changed_cashier-fopen");
        return 1;
    }

    while (!feof(fp_r))
    {
        fread(&cashier_read, 1, sizeof(cashier_info), fp_r);

        if(cashier_read.number == cashier_tmp.number)
        {
            if(0 == changed_cashier_active(&cashier_read))
            {
                cashier_print(&cashier_read);
                fseek(fp_a, num_of_cashier * sizeof(cashier_info), SEEK_SET);

                fwrite(&cashier_read, 1, sizeof(cashier_info), fp_a);
                
                fclose(fp_r);
                fclose(fp_a);

                printf("\n修改成功");
                return 0;
            }
        }
        /* 计数，读取到了第几个收银员,用于确定文件覆写指针位置 */
        num_of_cashier++;
    }
    
    fclose(fp_r);
    fclose(fp_a);
    
    printf("\n修改失败，未查找到目标数据.");
    return 0;
}

int changed_cashier_active(cashier_info* ui)
{
    int changed_choice = -1;
    char password[33];
    printf("\n***********************");
    printf("\n1. 编号");
    printf("\n2. 账户");
    printf("\n3. 密码");
    printf("\n4. 真实姓名");
    printf("\n5. 性别");
    printf("\n6. 手机号");
    printf("\n7. 身份证号");
    printf("\n***********************");
    printf("\n请选择你要修改的编号，或按 0 取消修改:");

    while (1)
    {
        scanf("%d", &changed_choice);
        while (getchar() != '\n');
        
        switch(changed_choice)
        {
        case 1:
            printf("\n收银员 - 编号:");
            scanf("%d", &ui->number);
            break;

        case 2:
            printf("\n收银员 - 账户:");
            scanf("%s", ui->uname);
            break;

        case 3:
            printf("\n收银员 - 密码:");
            scanf("%s", password);// 密码输入应当无回显
            md5_string(password, ui->upass);
            break;

        case 4:
            printf("\n收银员 - 真实姓名:");
            scanf("%s", ui->relname);
            break;

        case 5:
            printf("\n收银员 - 性别(1-man 0-wuman):");
            scanf("%hhd", &ui->sex);
            break;
            
        case 6:
            printf("\n收银员 - 手机号:");
            scanf("%s", ui->phone);
            break;

        case 7:
            printf("\n收银员 - 身份证号:");
            scanf("%s", ui->idcard);
            break;

        case 0:
            printf("\n返回上一级");
            return 1; /* 取消修改，不执行写入 */
            break;

        default:
            printf("\n序号输入有误,请检查或输入 0 退出:");
            continue;
        }

        return 0;/* 修改成功，准备写入 */
    }

}



/*********************************
 * 查找收银员:
 * 查找文件中的目标收银员
 * 查找方案：
 * 通过 编号 检索
 * 通过 真实姓名 检索
 * 通过 用户名 检索
*********************************/
int find_get_cashier()
{
    int option_tmp = 0;
    cashier_info cashier, cashier_tmp;
    memset(cashier_found, 0, sizeof(cashier_info));// 初始化存放空间
    
    FILE* fp = fopen(CASHIER_MSG_FILE, "r");
    if(NULL == fp)
    {
        perror("find_get error");
    }

    while (1)
    {
        printf("\n请选择通过何种信息检索\n"
        "1:by编号 2:by用户名 3:by真实姓名(支持模糊查找)");

        printf("\n>");

        scanf("%d", &option_tmp);
        while(getchar() != '\n');

        switch (option_tmp)
        {
        case 1:
            printf("\n编号查找...");
            printf("\n请输入编号:");
            scanf("%d", &cashier.number);  
            while(getchar() != '\n');
            
            while(!feof(fp))
            {
                fread(&cashier_tmp, 1, sizeof(cashier_info), fp);
                // cashier_print(&cashier_tmp); // TMP!
                if(cashier.number == cashier_tmp.number)
                {
                    *cashier_found = cashier_tmp;
                    return cashier_tmp.number;
                }
            }
            printf("\n未找到该编号对应的数据！请检查编号");
            return 0;
        
        case 2:
            printf("\n用户名查找...");
            printf("\n请输入用户名:");
            scanf("%s", cashier.uname);
            while(getchar() != '\n');

            while(!feof(fp))
            {
                fread(&cashier_tmp, 1, sizeof(cashier_info), fp);
                if(!strcmp(cashier.uname, cashier_tmp.uname))
                {
                    *cashier_found = cashier_tmp;
                    return cashier_tmp.number;
                }
            }
            printf("\n未找到该用户名对应的数据！请检查输入");
            return 0;

        case 3:
            printf("\n真实姓名查找...");
            printf("\n请输入真实姓名:");
            scanf("%s", cashier.relname);
            while(getchar() != '\n');

            while(!feof(fp))
            {
                fread(&cashier_tmp, 1, sizeof(cashier_info), fp);

                if(NULL != strstr(cashier_tmp.relname, cashier.relname))
                {
                    *cashier_found = cashier_tmp;
                    return cashier_tmp.number;
                }
            }
            printf("\n未找到该真实姓名对应的数据!");
            
            return 0;

        default:
        printf("\n序号输入有误,请重新输入:");
            break;
        }
    }
}


/*********************************
 * 查找收银员:
 * 查找文件中的所有收银员
*********************************/
int find_all_cashier()
{
    cashier_info cashier_tmp;

    memset(cashier_found, 0, sizeof(cashier_info));// 初始化存放空间
    
    FILE* fp = fopen(CASHIER_MSG_FILE, "r");
    
    if(NULL == fp)
    {
        perror("find_all_cashier-fopen error");
        return 1;
    }

    while(!feof(fp))
    {
        if(sizeof(cashier_info) == fread(&cashier_tmp, 1, sizeof(cashier_info), fp))
        {
            cashier_print(&cashier_tmp);
            printf("\n-- -- -- --");
        }
    }

    return 0;

}



/*********************************
 * 管理员 - 销售记录查询
 * 查找所有收银员的销售记录(默认按照时间排序)
*********************************/
int act_of_sale()
{
    // 销售记录文件
    FILE* fp_aos = fopen(ACT_OF_SALE_FILE, "r");
    if(NULL == fp_aos)
    {
        perror("act_of_sale-fopen");
        return 1;
    }
    
    sale_log sale_tmp;

    printf("\n总销售记录如下:");

    while(!feof(fp_aos))
    {
        if(sizeof(sale_log) != fread(&sale_tmp, 1, sizeof(sale_log), fp_aos))
        {
            break;
        }

        printf_sale_log(&sale_tmp);
        printf("\n------------------");
    }

    printf("\n\n查询完毕");

    fclose(fp_aos);

    return 0;
}

/*********************************
 * 管理员 - 销售额查询
 * 1. 查找指定收银员的     销售额
 * 2. 查找指定时间之间的   销售额
*********************************/
int sales_money()
{
    float sum = 0;      /* 销售额 */
    int boss_choice = 0;
    char uname[51];     /* 存放收银员编号 */

    bbtTM_S time_start, time_end;   /* 存放起始与结束时间 */
    
    // 销售记录文件
    FILE* fp_aos = fopen(ACT_OF_SALE_FILE, "r");

    if(NULL == fp_aos)
    {
        perror("act_of_sale-fopen");
        return -1;
    }
    /* 临时存放读取到的一条销售记录 */
    sale_log sale_tmp;

    printf("\n通过 1.收银员 2.时间区间 查询销售额(输入其他值取消):");
    scanf("%d", &boss_choice);
    while(getchar() != '\n');

    switch (boss_choice)
    {
    // 通过 【目标收银员编号】 查询文件内部分销售额
    case 1:
        printf("\n请输入目标收银员用户名:");

        scanf("%s", uname);

        // while(getchar() != '\n');

        // 通过 【目标收银员编号】 遍历查询文件内所有销售记录
        while(!feof(fp_aos))
        {
            fread(&sale_tmp, 1, sizeof(sale_log), fp_aos);

            if(!strcmp(uname, sale_tmp.uname))
            {
                sum += sale_tmp.total;
            }
        }

        fclose(fp_aos);

        return sum;

    // 通过 【时间区间】 查询文件内部分销售额
    case 2:
        printf("\n请输入时间区间-起始(格式2020-01-01 12:00:30):");

        scanf("%hd-%hd-%hd %hd:%hd:%hd", &time_start.tm_year, &time_start.tm_mon,
                &time_start.tm_mday, &time_start.tm_hour, &time_start.tm_min, &time_start.tm_sec);

        // 起始日期转换为时间戳
        time_start.tm_dts = bbt_mktime(time_start);

        printf("\n请输入时间区间-结束(格式2020-01-01):");
        
        scanf("%hd-%hd-%hd %hd:%hd:%hd", &time_end.tm_year, &time_end.tm_mon,
                &time_end.tm_mday, &time_end.tm_hour, &time_end.tm_min, &time_end.tm_sec);

        // 结束日期转换为时间戳
        time_end.tm_dts = bbt_mktime(time_end);

        // 通过 【时间区间】 遍历查询文件内所有销售记录
        while(!feof(fp_aos))
        {
            fread(&sale_tmp, 1, sizeof(sale_log), fp_aos);

            /* 获取这一条销售记录的时间戳，用于比较是否符合条件 */
            if(time_start.tm_dts < sale_tmp.sale_time.tm_dts && 
            sale_tmp.sale_time.tm_dts < time_end.tm_dts)
            {
                sum += sale_tmp.total;
            }
        }
        fclose(fp_aos);
        return sum;

    default:
        printf("\n退出");

        return -2;
    }
}

void cashier_print(cashier_info* ui)
{
    // printf("\n测试打印结果如下:");
    printf("\n> 编    号: %d", ui->number);
    printf("\n> 用 户 名: %s", ui->uname);
    printf("\n> 真实姓名: %s", ui->relname);
    printf("\n> 性    别: %s", ui->sex?"男士":"女士");
    printf("\n> 电话号码: %s", ui->phone);
    printf("\n> 身份证号: %s", ui->idcard);
    printf("\n> 登录次数: %d", ui->login_time);
    // printf("\n> 注册时间: %ld", ui->reg_time);
    printf("\n> 注册时间:");
    print_time(ui->reg_time);
    printf("\n> 最近登录时间: %ld", ui->last_login_time);
}

