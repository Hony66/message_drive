#ifndef __MED_CORE_H
#define __MED_CORE_H

#ifndef NULL
    #define NULL 0u
#endif

#define MSGTABLE_MAX_LEN              20u  //消息表的长度

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */
#define UNUSED(x) ((void)(x))


//枚举所有消息
typedef enum{
    ENTER = 1,
    EXIT,
}MsgEnum;

typedef struct{
    volatile MsgEnum MsgId;
    volatile unsigned char data;
}MESSAGE;


typedef struct{
    MESSAGE Table[MSGTABLE_MAX_LEN];
    volatile unsigned  char Len;
    unsigned  char  Start;
    unsigned  char End;
}MSG_Ctrl_TypeDef;

//消息处理结果
typedef enum{
    HANDLED = 1,
    UNHANDLED,
}MsgPrcResult;

//消息处理结果
typedef enum{
    MED_OK,
    MED_MSG_POST_FAILED,
    MED_PARAM_ERR,
}MED_Status_Enum;


//对象处理函数模板
typedef MsgPrcResult (* mED_OBJ_Func_Template)(const MESSAGE* );


typedef struct{
    mED_OBJ_Func_Template OBJ_Func;
    mED_OBJ_Func_Template OBJ_Func_Cache;
    unsigned int data;
}mED_OBJ_TypeDef;




void mED_Exec(void);
MED_Status_Enum mED_PostMsg(const MESSAGE * pMsg);
void mED_Init(void);
void mED_Set_OBJ_Func(mED_OBJ_Func_Template  pCb, unsigned int data);
void mED_Init_Callback(void);
void mED_DfuProc(const MESSAGE *pMsg);


#endif



