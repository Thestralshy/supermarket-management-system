
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"
#include "cashier_main.h"
#include "boss_operation.h"
#include "cashier_operation.h"


/*********************************
 * 登录到收银员后的显示主界面函数
*********************************/
void cashier_display()
{
    cashier_first_login();  // 检测是否为首次登录
    cashier_info my_cashier = *cashier_found;
    int cashier_choice = -1;
    
    char cashier_choice_help[8];
    
    char ch_tmp;
    
    float should_get_money = -1;// 应收款
    float sale_money_tmp = 0;   // 存放该用户自身的销售额

    printf("\n打卡成功！欢迎您上班，收银员 %s", my_cashier.relname);
    cashier_mean(&my_cashier);

    while (1)
    {
        printf("\n请输入序号进行办公...");
        printf("\n> ");
        scanf("%d", &cashier_choice);
        //while (getchar() != '\n');

        switch(cashier_choice)
        {
        /* 收银 */
        case 1:
            should_get_money = get_money();
            if(should_get_money != -1)
            {
                printf("\n 应收%.2f元,请选择选项2进行找零...", should_get_money);
            }
            break;

        /* 找零 */
        case 2:
            give_money(should_get_money);
            break;
        
        /* 查看个人信息 */
        case 3:
            printf("\n正在查看个人信息...");
            information_show();
            printf("\n");
            break;
        
        /* 查看销售记录 */
        case 4:
            printf("\n正在查看销售记录...");
            act_of_sale_show();
            break;

        /* 查看销售额 */
        case 5:
            printf("\n正在查看销售额...");
            if(0 != (sale_money_tmp = sales_my_money()))
            {
                printf("\n销售额为%.2f", sale_money_tmp);
            }
            printf("\n销售额为 0");
            break;

        /* 修改密码 */
        case 6:
            cashier_passwd_changed();
            break;

        /* 退出登录 */
        case 0:
            while (getchar() != '\n');
            
            printf("\n退出登录?Y/n");

            char ch_tmp = getchar();

            while (getchar() != '\n');
            
            if(ch_tmp == 'Y' || ch_tmp == 'y')
            {
                return;
            }

            break;

        /* 如果没有输入任何数字，选择数仍旧为-1，则开始读取字符 */
        case -1:
            scanf("%s", cashier_choice_help);
            // "help" 输入检测
            if(!strcmp(cashier_choice_help, "help"))
            {
                cashier_mean(&my_cashier);
                break;
            }

            printf("\n序号输入有误！请重新输入或 help 查看菜单");

            break;

        /* 如果输入了其他一些非法字符，则提示重新输入 */
        default:
            printf("\n序号输入有误！请重新输入或 help 查看菜单");
            break;
        }

        /* 关键临时变量清空 */
        // should_get_money = -1;
        cashier_choice = -1;

    }
    
}

/************************
 * 收银员 - 1. 收银
 * 步骤：
 * 1-输入一个商品条码
 * 2-通过商品条码找到商品信息 - 计入销售记录;
 * 3-通过商品条码 - 将该商品从库存中-1;
 * 4-询问继续循环1-3步骤（循环1-3）还是结束录入(跳转到5)
 * 5-确认收银
 * 6-结束收银
*************************/
float get_money()
{
    
    const int con = 48; /* 最多一次扫48件商品 */
    char c_tmp;         /* 存放收银员的选择，继续录入还是结束 */
    float price_sum = 0;/* 用于存放总价格 */
    int comm_number[con];    /* 用于存放商品数量 */
    int sale_comm[con]; /* 用于记录某个商品在文件中的位置 */
    int i, j;
    
    commodity comm_tmp[con]; /* 存放录入的条码,及其信息 */

    /* 商品售卖信息录入模块 */
    for(i = 0; i < con; i++)
    {
        /* 录入该件商品条码 */
        printf("\n请输入商品%d条码:", i);
        scanf("%s", comm_tmp[i].number);

        /* 检查该件商品条码是否正确 */
        /* 若正确则将该商品条码对应的商品信息提取出来 */
        if((sale_comm[i] = commodity_exist(&comm_tmp[i])) == -1)
        {
            commodity_show(&comm_tmp[i]);

            printf("\n条码不正确!");

            i--;

            continue;
        }
        
        /* 输入售出的商品数量 */
        printf("\n请输入商品%d数量", i);
        scanf("%d", &comm_number[i]);
        while (getchar() != '\n');
        
        /* 卖出,剩余数量减少 comm_number */
        comm_tmp[i].lessnumber -= comm_number[i];

        /* 卖出,计入总价格 price_sum */
        price_sum += comm_tmp[i].price * comm_number[i];

        printf("\n按下Y继续,输入任意其他值结束商品录入...");

        c_tmp = getchar();
        while (getchar() != '\n');

        if(c_tmp == 'Y' || c_tmp == 'y')
        {
            continue;
        }

        break;
    }

    /* 将【商品 - 库存变化】写入到文件中 */
    FILE* fp = fopen(COMMODITY_MSG_FILE, "r+");
    if(NULL == fp)
    {
        perror("get_money-fopen");
        return 0;
    }

    for(j = 0; j <= i && j < con; j++)
    {
        fseek(fp,sale_comm[j] * sizeof(commodity) ,SEEK_SET);

        fwrite(&comm_tmp[j], 1, sizeof(commodity), fp);
    }

    fclose(fp);

    /* 将【销售记录】写入到文件中 */
    FILE* fp_salelog = fopen(ACT_OF_SALE_FILE, "a");
    if(NULL == fp_salelog)
    {
        perror("get_money-fopen error");
        return 0;
    }

    sale_log sale_cog_tmp;/* 临时存放单条销售记录,然后整体写入文件夹 */

    for(j = 0; j <= i && j < con; j++)
    {
        strcpy(sale_cog_tmp.uname, cashier_found->uname);   /* 收银员用户名 */
        strcpy(sale_cog_tmp.number, comm_tmp[j].number);      /* 商品条码 */
        sale_cog_tmp.price = comm_tmp[j].price;               /* 商品单价 */
        sale_cog_tmp.quantity = comm_number[j];             /* 售卖数量 */
        sale_cog_tmp.total = sale_cog_tmp.price * sale_cog_tmp.quantity;/* 售卖总价 */
        sale_cog_tmp.sale_time.tm_dts = time(NULL);         /* 售卖时间 */
        strcpy(sale_cog_tmp.remark, "无");                  /* 默认备注 */
        
        printf("\n对于商品:[%s] 是否需要输入备注 Y/n:", comm_tmp[j].name);
        
        char ch_tmp = getchar();
        while (getchar() != '\n');

        if(ch_tmp == 'Y' ||ch_tmp == 'y')
        {
            scanf("%s", sale_cog_tmp.remark);
        }
        
        /* 售卖商品信息打印 */
        printf("\n商品信息如下:");
        printf_sale_log(&sale_cog_tmp);

        fwrite(&sale_cog_tmp, 1, sizeof(sale_log), fp_salelog);
    }

    fclose(fp_salelog);

    /* 应付 n 元 */
    return price_sum;/* 结束正常收银 */
}



/* 找零(应收) */
void give_money(float price_sum)
{
    float true_money;   /* 实收 */

    printf("\n请输入实际收款:");

    scanf("%f", &true_money);

    printf("\n应找 %.2f 元", true_money - price_sum);

    return;
}

/***************************
 * 显示个人信息
 ***************************/
void information_show()
{   
    printf("您的个人信息如下:");
    cashier_print(cashier_found);
}

/* 收银员显示菜单 */
void cashier_mean(cashier_info* my_cashier)
{
    printf("\n--------------------------------------------------");
    printf("\n      --       收银员管理界面           --");
    printf("\n--------------------------------------------------");
    printf("\n- 用户:%s   登陆时间:", my_cashier->uname);// 用户名 登陆时间
    print_time(time(NULL));
    printf("\n--------------------------------------------------");
    printf("\n- 1. 收银");
    printf("\n- 2. 找 0");
    printf("\n- 3. 查看个人信息");
    printf("\n- 4. 查看销售记录");
    printf("\n- 5. 查看销售额");    // 该功能暂未开放
    printf("\n- 6. 修改登陆密码");
    printf("\n- 0. 退出登录");
    printf("\n--------------------------------------------------");
}

/***************************
 * 查看自己的销售记录
***************************/
void act_of_sale_show()
{
    /* 打开销售记录文件 */
    FILE* fp_act_sale = fopen(ACT_OF_SALE_FILE, "r");

    if(NULL == fp_act_sale)
    {
        perror("act_of_sale_show-fopen error");
        return ;
    }

    sale_log sale_log_tmp;

    /* 一条条地读取销售记录 */
    while(!feof(fp_act_sale))
    {
        if(sizeof(sale_log) != fread(&sale_log_tmp, 1, sizeof(sale_log), fp_act_sale))
        {
            printf("\n读取完毕!");
            break;
        }

        /* 若收银员姓名对的上 */
        if(!strcmp(sale_log_tmp.uname, cashier_found->uname))
        {
            /* 打印销售记录 */
            printf_sale_log(&sale_log_tmp);
            printf("\n--------");
        }
    }

    fclose(fp_act_sale);
}

/***************************
 * 查看自己的销售额
 * 访问销售记录文件，遍历读取所有自己的销售信息，
 *  并将其中的数字读取进行累加和
***************************/
float sales_my_money()
{
    FILE* fp = fopen(ACT_OF_SALE_FILE, "r");
    float sum = 0;

    if(NULL == fp)
    {
        perror("sales_my_money-fopen error");
        return 0;
    }

    sale_log sale_log_tmp;

    while(!feof(fp))
    {
        if(sizeof(sale_log) == fread(&sale_log_tmp, 1, sizeof(sale_log), fp))
        {
            if(!strcmp(sale_log_tmp.uname, cashier_found->uname))
            {
                sum += sale_log_tmp.quantity * sale_log_tmp.price;
            }
        }
    }

    fclose(fp);

    return sum;
}

/***************************
 * 收银员 首次登录检测 函数
****************************/
int cashier_first_login()
{
    if(cashier_found->login_time <= 1)
    {
        printf("首次使用，需要修改密码方可登录:");

        cashier_passwd_changed();

        return 0;
    }
    return 0;
}

/***************************
 * 收银员 账号密码修改 函数
****************************/
int cashier_passwd_changed()
{
    FILE* fp_w = fopen(CASHIER_MSG_FILE, "r+");
    if(NULL == fp_w)
    {
        perror("cashier_passwd_changed-fopen error");
        return 1;
    }

    char passwd[33], passwd_again[33];
    cashier_info cashier_tmp, cashier_read_tmp;   /* 临时存放收银员数据 */

    cashier_tmp = *cashier_found;

    /* 将新内容改写到临时容器 cashier_tmp */

    /*  新密码 BEGIN  */
    printf("\n新密码:");
    system("stty -echo");scanf("%s", passwd);system("stty echo");

    printf("\n确认新密码:");
    system("stty -echo");scanf("%s", passwd_again);system("stty echo");

    if(strcmp(passwd, passwd_again))
    {
        printf("\n密码不一致,请检查输入.");
        return 1;
    }

    md5_string(passwd, cashier_tmp.upass);
    /*  新密码 END  */

    /* 遍历获取文件中自己用户的数据位置 */
    int i = 0;
    while(!feof(fp_w))
    {
        if(sizeof(cashier_info) == fread(&cashier_read_tmp, 1, sizeof(cashier_info), fp_w))
        {
            if(cashier_read_tmp.number == cashier_found->number)
            {
                break;
            }
        }
        i++;
    }

    // 定位到文件位置
    fseek(fp_w, i * sizeof(cashier_info), SEEK_SET);

    // 改写文件中的自己账号密码
    fwrite(&cashier_tmp, 1, sizeof(boss_info), fp_w);

    // 改写临时存放的账号密码数据
    *cashier_found = cashier_tmp;

    fclose(fp_w);

    return 0;
}



/* 销售记录打印函数 */
void printf_sale_log(sale_log* sale_log_tmp)
{
    printf("\n收 银 员:%s", sale_log_tmp->uname);
    printf("\n商品条码:%s", sale_log_tmp->number);
    printf("\n售卖单价:%.2f", sale_log_tmp->price);
    printf("\n售卖数量:%d", sale_log_tmp->quantity);
    printf("\n售卖总价:%.2f", sale_log_tmp->total);
    printf("\n售卖时间:");
    print_time(sale_log_tmp->sale_time.tm_dts);
    printf("\n备    注:%s", sale_log_tmp->remark);
}

