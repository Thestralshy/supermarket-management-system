#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "cashier_operation.h"
#include "boss_operation.h"
#include "md5.h"

/***************************
 * boss 结构体初始化函数
****************************/
void boss_struct_init(boss_info* boss_msg)
{
    memset(boss_msg, 0, sizeof(boss_info));
}

/****************************
 * 文件异常检测
 * 若管理员用户账号密码文件异常，会自动重置密码
****************************/
int boss_msg_init()
{
    FILE* fp_r = fopen(BOSS_MSG_FILE, "r");

    if (NULL == fp_r)
    {
        perror("boss_msg_init-fopenr");
        return 1;
    }

    boss_info boss_msg;    // 存放 boss 读写信息

    char passwd_tmp[33];  // 临时存放密码

    /* 若读取数据异常则自动重置 */
    if(sizeof(boss_info) != fread(&boss_msg, 1, sizeof(boss_info),fp_r))
    {
        FILE* fp_w = fopen(BOSS_MSG_FILE, "w");
        
        if (NULL == fp_w)
        {
            perror("boss_msg_init-fopenw");
            return 1;
        }

        md5_string("888888", passwd_tmp);

        
        boss_struct_init(&boss_msg);    // 对结构体内部数据进行初始化

        strcpy(boss_msg.name, "boss");  // 账号信息更改为默认账户名

        strcpy(boss_msg.pswd, passwd_tmp);// 账号信息更改为默认密码

        boss_msg.login_time = 0;        // 登录次数重置为 0
        
        fwrite(&boss_msg, 1, sizeof(boss_info), fp_w);
        
        fclose(fp_w);
        
        return 0;
    }

    fclose(fp_r);

    return 0;
}

/***************************
 * 管理员控制面板功能实现
 * 增删改查
****************************/
void boss_display()
{
    boss_first_login();
    int boss_choice = -1;
    int option_tmp = 0;
    int add_choice = 0;
    int find_choice = 0;    /* 存放用户选项 */
    int money_tmp;          /* 销售额 */
    char boss_choice_help[8] = {0};
    char add_file_name[48]; /* 临时存放文件名路径 */
    char ch_tmp[128];
    char chs_to_stdin[128];
    
    printf("\n欢迎使用《幻兽帕鲁》管理系统!");
    mean();

    while(1)
    {
        printf("\n> ");

        scanf("%d", &boss_choice);

        switch(boss_choice)
        {

        /* 1. 帕鲁 - 新增帕鲁 */
        /* 注意事项 */
        /* 每新增一个需要改写【注册时间】 */
        case 1:
            printf("\n- 正在新增帕鲁...");
            printf("\n请选择添加方式,或输入其他任意取消添加:");
            printf("\n1. 文件添加");
            printf("\n2. 手动添加");

            scanf("%d", &add_choice);
            while(getchar() != '\n');

            switch (add_choice)
            {
            case 1:
                printf("\n注意:文件内每一行之间换行;元素之间用分号隔开");
                printf("\n请输入文件名(48Byte Max):");

                scanf("%s", add_file_name);

                create_cashier_for_file(add_file_name);

                break;
            case 2:

                create_cashier();

                break;

            default:
                break;
            }
            break;

        /* 2. 帕鲁 - 肢解帕鲁 */
        case 2:
            printf("\n- 正在肢解帕鲁...");
            if(0 == delete_cashier())
            {
                printf("\n删除成功！");
            }
            break;
            
        /* 3. 帕鲁 - 修改帕鲁个人信息 */
        case 3:
            printf("\n- 正在修改帕鲁个人信息...");
            changed_cashier();
            break;

        /* 4. 帕鲁 - 查找帕鲁个人信息 */
        case 4:
            printf("\n- 正在检索帕鲁...");
            printf("\n查找方式:");
            printf("\n1.指定信息查找 2.全部显示");

            scanf("%d", &find_choice);
            while(getchar() != '\n');

            switch (find_choice)
            {
                
            /* 4.1 指定信息查找 */
            case 1:
                find_get_cashier();
                if(memcmp(cashier_found, cashier_zero, sizeof(cashier_info)))
                {
                    printf("\n您要查找的结果如下:");
                    cashier_print(cashier_found);
                }
                break;

            /* 4.2 全部显示 */
            case 2:
                find_all_cashier();
                break;
            
            default:
                printf("\n序号输入有误!");
                break;
            }
            break;

        /* 5. 帕鲁 - 销售记录 */
        case 5:
            printf("\n- 正在查询销售记录...");
            act_of_sale();// 所有销售记录查询
            break;

        /* 6. 帕鲁 - 销售额 */
        case 6:
            printf("\n- 正在查询销售额...");
            money_tmp = sales_money();
            if(0 == money_tmp)// 销售额查询
            {
                printf("\n该用户不存在或销售额总额为0");
            }
            else if(-1 == money_tmp)
            {
                printf("\n出了点错");
            }
            else if(-2 == money_tmp)
            {
                break;
            }
            else
            {
                printf("\n销售额总额为%d", money_tmp);;
            }
            break;

        /* 7. 商品 - 商品进货 */
        case 7:
            
            printf("\n进货方式:");
            printf("\n1.手动输入添加商品 2.文件添加");

            scanf("%d", &find_choice);
            while(getchar() != '\n');

            switch (find_choice)
            {
            /* 7.1 手动输入添加商品 */
            case 1:
                commodity_add();
                break;

            /* 7.2 文件添加商品 */
            case 2:
                printf("\n请输入文件路径:");
                scanf("%s", add_file_name);

                if(0 ==commodity_add_file(add_file_name))
                {
                    printf("\n添加成功!");
                }
                else
                {
                    printf("\n添加失败!");
                }

                break;
            
            default:
                printf("\n序号输入有误!");
                break;
            }
            break;

        /* 8. 商品 - 删除商品信息 */
        case 8:
            printf("\n删除商品...");
            printf("\n请输入商品条码号:");

            scanf("%s", ch_tmp);

            if(0 == commodity_del(ch_tmp))
            {
                printf("\n删除成功!");
            }
            else
            {
                printf("\n删除失败!");
            }

            break;

        /* 9. 商品 - 修改商品信息 */
        case 9:
            commodity_change();
            break;

        /* 10. 商品 - 查询商品信息 */
        case 10:
            printf("\n查找方式:");
            printf("\n1.编号查找 2.全部显示");

            scanf("%d", &find_choice);
            while(getchar() != '\n');

            switch (find_choice)
            {
            case 1:
                if(1 == commodity_find())
                {
                    printf("\n该商品不存在");
                }
                break;
            
            case 2:
                commodity_find_all();
                break;
            
            default:
                printf("\n序号输入有误!");
                break;
            }
            break;


        case 11:
            printf("\n正在修改管理员账号密码...");
            if(0 == boss_pswd_changed_login())
            {
                printf("\n修改成功!");
            }
            break;


        /* 若输入了 0 则退出该 boss 用户 */
        case 0:
            return;
            break;
        

        /* 如果没有输入任何数字，选择数仍旧为-1，则开始读取字符 */
        case -1:
            scanf("%s", boss_choice_help);
            
            /* help 检测 - 呼出菜单 */
            if(!strcmp(boss_choice_help, "help"))
            {
                mean();
                break;
            }

            /* clear 检测 - 清除面板 */
            if(!strcmp(boss_choice_help, "clear"))
            {
                printf("\033[2J");
                break;
            }

            /* off 检测 - 关闭本进程 */
            if(!strcmp(boss_choice_help, "off"))
            {
                exit(0);
                break;
            }

            printf("\n序号输入有误！请重新输入或 help 查看菜单");
            break;

        default:
            printf("\n序号输入有误！请重新输入或 help 查看菜单");
            break;
        }

        while(getchar()!='\n'); // 清空 strin

        boss_choice = -1;        // 清空选项
    }
}


/***************************
 * 管理员 首次登录检测 函数
****************************/
int boss_first_login()
{
    boss_info boss_msg;
    FILE* fp_r = fopen(BOSS_MSG_FILE, "r");
    
    if(NULL == fp_r)
    {
        perror("boss_first_login");
        return -1;
    }

    fread(&boss_msg, 1, sizeof(boss_info), fp_r);
    // printf("boss_msg.login_time = %d\n", boss_msg.login_time);
    fclose(fp_r);

    if(boss_msg.login_time == 1)
    {
        printf("\n首次登录，请修改账号密码");
        passwd_changed(&boss_msg);
    }
    
}

/***************************
 * 管理员 账号密码读取并修改存放 函数
****************************/
int boss_pswd_changed_login()
{
    boss_info boss_msg;
    FILE* fp_r = fopen(BOSS_MSG_FILE, "r");
    
    if(NULL == fp_r)
    {
        perror("boss_pswd_changed_login");
        return -1;
    }

    fread(&boss_msg, 1, sizeof(boss_info), fp_r);
    
    fclose(fp_r);

    return passwd_changed(&boss_msg);
}

/***************************
 * 管理员 账号密码修改 函数
****************************/
int passwd_changed(boss_info* boss_msg)
{
    FILE* fp_w = fopen(BOSS_MSG_FILE, "w");
    if(NULL == fp_w)
    {
        perror("passwd_changed-fopen");
        return 1;
    }

    char passwd[33], passwd_again[33];

    printf("\n新账号:");
    scanf("%s", boss_msg->name);

    printf("\n新密码:");
    system("stty -echo");scanf("%s", passwd);system("stty echo");

    printf("\n确认新密码:");
    system("stty -echo");scanf("%s", passwd_again);system("stty echo");

    if(strcmp(passwd, passwd_again))
    {
        printf("\n密码不一致,请检查输入.");
        return 1;
    }

    md5_string(passwd, boss_msg->pswd);

    fwrite(boss_msg, 1, sizeof(boss_info), fp_w);

    fclose(fp_w);

    return 0;
    
}


/***************************
 * 管理员 - 商品 - 进货
****************************/
void commodity_add()
{
    FILE* fp_commodity_a = fopen(COMMODITY_MSG_FILE, "r+");/* 注意！a没有覆写功能 */
    int comm_num = 0;
    int num = -1;

    if(NULL == fp_commodity_a)
    {
        perror("commodity_add-fopen");
        return;
    }

    commodity comm_tmp;

    printf("\n请输入商品信息:");

    printf("\n商品条码号:");
    scanf("%s", comm_tmp.number);
    while (getchar() != '\n');

    /* 商品存在性检测 */
    if(-1 != (num = commodity_exist(&comm_tmp)))
    {
        /* 若商品存在 - 直接修改新增数量 */
        printf("\n该商品存在!请填写进货数量:");
        scanf("%d", &comm_num);

        /* 在原来的基础上增加新的进货数量 */
        comm_tmp.total += comm_num;
        comm_tmp.lessnumber += comm_num;

        /* 定位到该商品在文件中的位置 */
        fseek(fp_commodity_a, num * sizeof(commodity), SEEK_SET);

        /* 在文件中修改覆盖该商品数据 */
        fwrite(&comm_tmp, 1, sizeof(comm_tmp), fp_commodity_a);

    }
    else
    {
        /* 若商品不存在 - 填写商品信息 */
        printf("\n商品名称:");
        scanf("%s", comm_tmp.name);
        while (getchar() != '\n');
        
        printf("\n商品单价:");
        scanf("%f", &comm_tmp.price);
        while (getchar() != '\n');

        printf("\n商品厂家:");
        scanf("%s", comm_tmp.factory);
        while (getchar() != '\n');
        
        printf("\n商品总进货数量:");
        scanf("%d", &comm_tmp.total);
        while (getchar() != '\n');

        comm_tmp.lessnumber = comm_tmp.total;

        // 指针指向文件末尾
        fseek(fp_commodity_a, 0, SEEK_END);

        // 在文件末尾新增写入数据
        if(sizeof(comm_tmp) == fwrite(&comm_tmp, 1, sizeof(comm_tmp), fp_commodity_a))
        {
            printf("\n写入成功");
        }
        else
        {
            printf("\n写入异常");
        }
    }

    fclose(fp_commodity_a);
}

/***************************
 * 管理员 - 商品 - 从文件录入进货信息
****************************/
int commodity_add_file(const void* comm_file)
{
    FILE* fp_read = fopen(comm_file, "r");

    if(NULL == fp_read)
    {
        perror("commodity_add_file-fopen error");
        return 1;
    }

    int num = 0;// 存放已读的条数,用于定位商品在文件中的位置

    commodity comm_read, comm_tmp;

    while(!feof(fp_read))
    {
        FILE* fp_changed_comm = fopen(COMMODITY_MSG_FILE, "r+");
        FILE* fp_add_comm = fopen(COMMODITY_MSG_FILE, "a");

        if(NULL == fp_add_comm || NULL == fp_changed_comm)
        {
            perror("commodity_add_file-fopen error");
            return 1;
        }

        /* 读取一条文件内的商品 */
        fscanf(fp_read, "%[^:]:%[^:]:%f:%[^:]:%d\n"
        ,comm_read.number   /* 进货条码号 */
        ,comm_read.name     /* 新货名字 */
        ,&comm_read.price    /* 新货单价 */
        ,comm_read.factory  /* 新货生产工厂 */
        ,&comm_read.total);  /* 进货数量 */ 
        comm_read.lessnumber = comm_read.total;/* 进货余量 */ 

        /* 取出条码号判断商品是否存在 */
        strcpy(comm_tmp.number, comm_read.number);

        /* 判断商品是否存在，若存在则进货；否则新增 */
        if(-1 != (num = commodity_exist(&comm_tmp)))
        {
            comm_tmp.total += comm_read.total;
            comm_tmp.lessnumber += comm_read.total;

            fwrite(&comm_tmp, 1, sizeof(commodity), fp_changed_comm);
        }
        else
        {
            /* 末尾追加一条从文件当中读取的商品信息 */
            fwrite(&comm_read, 1, sizeof(commodity), fp_add_comm);
        }

        fclose(fp_changed_comm);
        fclose(fp_add_comm);
    }
    fclose(fp_read);

    return 0;
}


/***************************
 * 管理员 - 商品 - 进货 - 检索是否存在
****************************/
int commodity_exist(commodity* user_commodity)
{
    FILE* fp_commodity_r = fopen(COMMODITY_MSG_FILE, "r");
    int i = 0;

    if(NULL == fp_commodity_r)
    {
        perror("commodity_exist-fopen");
        return -1;
    }

    commodity comm;

    while (!feof(fp_commodity_r))
    {
        fread(&comm, 1, sizeof(commodity), fp_commodity_r);




        if(!strcmp(comm.number, user_commodity->number))
        {
            /* 读取目标商品信息到输入的结构体中 */
            *user_commodity = comm;

            fclose(fp_commodity_r);

            return i;       // 存在目标商品
        }
        i++; // 记录读了第几个，用于后续文件指针定位
    }
    
    fclose(fp_commodity_r);

    return -1;   // 目标商品不存在
}

/***************************
 * 管理员 - 商品 - 下架
****************************/
int commodity_del(const char* comm_number)
{
    /* 在原路径后添加一个临时文件后缀 */
    char comm_file[128] = COMMODITY_MSG_FILE;

    strcat(comm_file, ".tmp");

    FILE* fp_commodity_r = fopen(COMMODITY_MSG_FILE, "r");
    FILE* fp_commodity_w = fopen(comm_file, "w");

    printf("以读模式打开%s\n", COMMODITY_MSG_FILE);
    printf("以写模式打开%s\n", comm_file);

    if(NULL == fp_commodity_r || NULL == fp_commodity_w)
    {
        perror("commodity_exist-fopen error");
        return 1;
    }

    /* 新建文件,检测到该商品就不复制过去,然后改名 */
    commodity comm_tmp;

    while (!feof(fp_commodity_r))
    {
        if(sizeof(commodity) == fread(&comm_tmp, 1, sizeof(commodity), fp_commodity_r))
        {
            /* 和目标条码不相等则保存下来 */
            if(strcmp(comm_tmp.number, comm_number))
            {
                printf("\n成功写入");
                fwrite(&comm_tmp, 1, sizeof(commodity), fp_commodity_w);
            }
        }
    }
    
    remove(COMMODITY_MSG_FILE);

    rename(comm_file, COMMODITY_MSG_FILE);

    fclose(fp_commodity_w);
    fclose(fp_commodity_r);

    return 0;
}

/***************************
 * 管理员 - 商品 - 修改信息
****************************/
void commodity_change()
{
    FILE* fp_commodity_a = fopen(COMMODITY_MSG_FILE, "a");
    int comm_num = 0;
    int num = -1;

    if(NULL == fp_commodity_a)
    {
        perror("commodity_add-fopen");
        return;
    }

    commodity comm_tmp;

    printf("\n请输入商品信息:");

    printf("\n商品条码号:");
    scanf("%s", comm_tmp.number);
    while (getchar() != '\n');

    /* 商品存在性检测 */
    if(-1 != (num = commodity_exist(&comm_tmp)))
    {
        /* 若商品存在 - 直接改写内容 */
        printf("\n商品名称:");
        scanf("%s", comm_tmp.name);
        while (getchar() != '\n');
        
        printf("\n商品单价:");
        scanf("%f", &comm_tmp.price);
        while (getchar() != '\n');

        printf("\n商品厂家:");
        scanf("%s", comm_tmp.factory);
        while (getchar() != '\n');
        
        printf("\n商品总进货数量:");
        scanf("%d", &comm_tmp.total);
        while (getchar() != '\n');

        printf("\n商品余货数量:");
        scanf("%d", &comm_tmp.lessnumber);
        while (getchar() != '\n');

        /* 定位到该商品在文件中的位置 */
        fseek(fp_commodity_a, num * sizeof(commodity), SEEK_SET);

        /* 在文件中修改覆盖该商品数据 */
        fwrite(&comm_tmp, 1, sizeof(comm_tmp), fp_commodity_a);
    }
    else
    {
        printf("\n商品不存在");
    }
    fclose(fp_commodity_a);
}

/***************************
 * 管理员 - 商品 - 条码号检索
****************************/
int commodity_find()
{   
    int comm_num = 0;
    int num = -1;

    commodity comm_tmp;

    printf("\n请输入商品条码号:");
    scanf("%s", comm_tmp.number);
    while (getchar() != '\n');

    /* 商品存在性检测 */
    if(-1 != commodity_exist(&comm_tmp))
    {
        /* 若商品存在 - 直接显示 */
        printf("\n该商品信息如下:");
        
        commodity_show(&comm_tmp);

        return 0;/* 商品存在 */
    }

    return 1; /* 商品不存在 */
}

/***************************
 * 管理员 - 商品 - 全部显示
****************************/
void commodity_find_all()
{
    FILE* fp_commodity_r = fopen(COMMODITY_MSG_FILE, "r");
    int i = 0;

    if(NULL == fp_commodity_r)
    {
        perror("commodity_find_all-fopen");
        return ;
    }
    /* 临时存放商品信息 */
    commodity comm;

    while (!feof(fp_commodity_r))
    {
        if(sizeof(commodity) != fread(&comm, 1, sizeof(commodity), fp_commodity_r))
        {
            break;
        }

        commodity_show(&comm);
    }
    fclose(fp_commodity_r);

    printf("\n------------");
}

/*************************
 * 指定商品信息展示
**************************/
void commodity_show(commodity* comm_tmp)
{
    printf("\n商品条码:%s", comm_tmp->number);
    printf("\n商品名称:%s", comm_tmp->name);
    printf("\n商品单价:%.2f", comm_tmp->price);
    printf("\n商品厂家:%s", comm_tmp->factory);
    printf("\n进货总量:%d", comm_tmp->total);
    printf("\n剩余数量:%d", comm_tmp->lessnumber);
}


/***************************
 * 管理员控制面板显示函数
****************************/
void mean()
{
    printf("\n********管理员控制面板********");
    printf("\n* 1. 帕鲁 - 新增帕鲁");
    printf("\n* 2. 帕鲁 - 肢解帕鲁");
    printf("\n* 3. 帕鲁 - 修改帕鲁个人信息");
    printf("\n* 4. 帕鲁 - 查找帕鲁个人信息");
    printf("\n* 5. 帕鲁 - 销售记录");
    printf("\n* 6. 帕鲁 - 销售额");

    printf("\n* 7. 商品 - 商品进货");
    printf("\n* 8. 商品 - 删除商品信息");  // 未完成
    printf("\n* 9. 商品 - 修改商品信息");
    printf("\n* 10. 商品 - 查询商品信息");

    printf("\n* 11. 管理员 - 修改用户名和密码");

    printf("\n* 0. 退出登录");
    printf("\n*****************************");
}


