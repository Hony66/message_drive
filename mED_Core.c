/**
  ******************************************************************************
  * @file    mED_Core.c
  * @author  Hony
  * @version V1.0
  * @date    2016-12-05
  * @brief   简单的事件驱动
  @verbatim
  
  @endverbatim
  ******************************************************************************
  * @attention
  * 20190123 修改了一些注释
  *
  ******************************************************************************  
  */
#include "mED_Core.h"


/* privte variable */
//创建消息控制器
static MSG_Ctrl_TypeDef Msg_Ctrler;
//创建系统对象
static mED_OBJ_TypeDef mED_OBJ = { NULL , NULL, 0};

/* privte function */



/**
* @brief  系统初始化回调 
* @param  无
* @retval 无
*/
__weak void mED_Init_Callback(void)
{
		
}

/**
  * @brief  系统默认处理函数
  * @param  pMsg 消息指针
  * @retval 无
  */ 
__weak void mED_DfuProc(const MESSAGE *pMsg)
{
	UNUSED(pMsg);
}


/**
* @brief  发送消息到消息表 FIFO
* @param  无
* @retval 无
*/
void mED_Init(void)
{
    Msg_Ctrler.Len = 0;
    Msg_Ctrler.Start = 0;
    Msg_Ctrler.End = 0;
    mED_Init_Callback();
}


/**
* @brief  发送消息到消息表 FIFO
* @param  pMsg:消息指针
* @retval 无
*/
MED_Status_Enum mED_PostMsg(const MESSAGE * pMsg)
{
    MED_Status_Enum sta;
    if(pMsg == NULL)
    {
        sta = MED_PARAM_ERR;
    }
    else if(Msg_Ctrler.Len >= MSGTABLE_MAX_LEN)//空间满
    {
        sta = MED_MSG_POST_FAILED;
    }
    else
    {
        Msg_Ctrler.Table[Msg_Ctrler.End].data = pMsg->data;
        Msg_Ctrler.Table[Msg_Ctrler.End].MsgId = pMsg->MsgId;
        Msg_Ctrler.End++;
        if(Msg_Ctrler.End == MSGTABLE_MAX_LEN)
        {
            Msg_Ctrler.End = 0;
        }
        Msg_Ctrler.Len++;
        sta = MED_OK;
    }
    return sta;
}

/**
* @brief  弹出一条消息
* @param  无
* @retval 消息指针
*/
MESSAGE* mED_PopupMsg(void)
{
    MESSAGE *ptMsg;
    if(Msg_Ctrler.Len == 0)
    {
        return NULL;
    }
    Msg_Ctrler.Len -= 1;
    ptMsg = &Msg_Ctrler.Table[Msg_Ctrler.Start];
    Msg_Ctrler.Start++;
    if(Msg_Ctrler.Start == MSGTABLE_MAX_LEN)
    {
        Msg_Ctrler.Start = 0;
    }
    return ptMsg;
}



/**
* @brief  设置系统功能回调
* @param  回调函数指针pCb，传入的数据data
* @retval 无
*/ 
void mED_Set_OBJ_Func(mED_OBJ_Func_Template  pCb, unsigned int data)
{
    if(pCb == NULL)
    {
        return ;
    }
    if(mED_OBJ.OBJ_Func == NULL)
    {
        MESSAGE ENTER_MSG = {ENTER, 0};
        ENTER_MSG.data = data;
        mED_OBJ.OBJ_Func = pCb;
        mED_OBJ.OBJ_Func(&ENTER_MSG);
    }
    else
    {
        MESSAGE EXIT_MSG = {EXIT, 0};
        mED_PostMsg(&EXIT_MSG);
        mED_OBJ.data = data;
        mED_OBJ.OBJ_Func_Cache = pCb;
    }
}

/**
* @brief  系统执行函数，需要被周期性调用
* @param  无
* @retval 无
*/ 
void mED_Exec(void)
{
    MESSAGE *ptMsg;
    ptMsg = mED_PopupMsg();
    if(ptMsg != NULL)
    {
        if(mED_OBJ.OBJ_Func != NULL)
        {  
            if(UNHANDLED == mED_OBJ.OBJ_Func(ptMsg))
            {
                mED_DfuProc(ptMsg);
            }
            if(ptMsg->MsgId == EXIT)
            {
                mED_OBJ.OBJ_Func = mED_OBJ.OBJ_Func_Cache;
                mED_OBJ.OBJ_Func_Cache = NULL;
                if(mED_OBJ.OBJ_Func != NULL)
                {
                    MESSAGE ENTER_MSG = {ENTER, 0};
                    ENTER_MSG.data = mED_OBJ.data;
                    mED_OBJ.OBJ_Func(&ENTER_MSG);
                }
            }
        }
        else
        {
            mED_DfuProc(ptMsg);
        }
    }
}












