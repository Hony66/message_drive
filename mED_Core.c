/**
  ******************************************************************************
  * @file    mED_Core.c
  * @author  Hony
  * @version V1.1
  * @date    2020-01-10
  * @brief   简单的事件驱动
  @verbatim
  2016-12-05 V1.0
  创建
  2020-01-10 V1.1
  修改ENTER及EXIT消息执行实现
  @endverbatim
  ******************************************************************************
  * @attention
  * mED_PostMsg 和 mED_PopupMsg不能在中断中使用
  ******************************************************************************  
  */
#include "mED_Core.h"

/* privte variable */
//创建消息控制器
static MSG_Ctrl_TypeDef Msg_Ctrler = {0} ;
//创建GUI系统对象
static mED_OBJ_TypeDef mED_OBJ = { NULL , NULL, 0};

/* privte function */



/**
* @brief  GUI初始化回调 
* @param  无
* @retval 无
*/
__weak void mED_Init_Callback(void)
{
		
}

/**
  * @brief  GUI默认处理函数
  * @param  pMsg 消息指针
  * @retval 无
  */ 
__weak void mED_DfuProc(const MESSAGE *pMsg)
{
	UNUSED(pMsg);
}


/**
* @brief  发送消息到消息表 FIFO
* @param  pMsg:消息指针
* @retval 无
*/
void mED_Init(void)
{
    Msg_Ctrler.Len = 0;
    Msg_Ctrler.Start = 0;
    Msg_Ctrler.End = 0;
    mED_OBJ.data = 0;
    mED_OBJ.OBJ_Func = NULL;
    mED_OBJ.OBJ_Func_Next = NULL;
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
* @retval MESSAGE
*/
static MESSAGE* mED_PopupMsg(void)
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
* @brief  设置系统对象功能回调
* @param  无
* @retval 无
*/ 
void mED_Set_OBJ_Func(mED_OBJ_Func_Template pCb, unsigned int data)
{
	if (pCb == NULL)
	{
		return;
	}
    mED_OBJ.OBJ_Func_Next = pCb;
    mED_OBJ.data = data;
}

/**
  * @brief  系统执行函数，需要被周期性调用
  * @param  无
  * @retval 无
  */
void mED_Exec(void)
{
	if (mED_OBJ.OBJ_Func_Next != NULL)
	{
		MESSAGE msg;
        if(mED_OBJ.OBJ_Func != NULL)
        {
            msg.MsgId = EXIT;
            msg.data = NULL;
            mED_OBJ.OBJ_Func(&msg);
        }
		msg.MsgId = ENTER;
		msg.data = mED_OBJ.data;
		mED_OBJ.OBJ_Func = mED_OBJ.OBJ_Func_Next;
		mED_OBJ.OBJ_Func_Next = NULL;
		mED_OBJ.OBJ_Func(&msg);
	}
    MESSAGE *pMsg;
    while(1)
    {
        pMsg = mED_PopupMsg();
        if (pMsg != NULL)
        {
            if (mED_OBJ.OBJ_Func != NULL)
            {
                if (UNHANDLED == mED_OBJ.OBJ_Func(pMsg))
                {
                    mED_DfuProc(pMsg);
                }
            }
            else
            {
                mED_DfuProc(pMsg);
            }
        }
        else
        {
            break;
        }
        if (mED_OBJ.OBJ_Func_Next != NULL)
        {
            MESSAGE msg;
            if(mED_OBJ.OBJ_Func != NULL)
            {
                msg.MsgId = EXIT;
                msg.data = NULL;
                mED_OBJ.OBJ_Func(&msg);
            }
            msg.MsgId = ENTER;
            msg.data = mED_OBJ.data;
            mED_OBJ.OBJ_Func = mED_OBJ.OBJ_Func_Next;
            mED_OBJ.OBJ_Func_Next = NULL;
            mED_OBJ.OBJ_Func(&msg);
        }
    }
}











