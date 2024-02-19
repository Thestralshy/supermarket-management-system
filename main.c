/************************************************
 * 
 * 在登录BOSS 管理员系统后可以通过
 *          - help 查看菜单;
 *          - clear 清空面板;
 *          - off 关闭进程
*************************************************/

#include <stdio.h>
#include <string.h>

#include "md5.h"
#include "login.h"
#include "boss_operation.h"
#include "cashier_operation.h"

int main(int argc, char** argv)
{
    /* 初始化boss信息检测 */
    boss_msg_init();
    cashier_operation_init();
    printf("初始化成功\n");

    /* 登录 */
    login_select();

    return 0;
}