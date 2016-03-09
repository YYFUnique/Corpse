#ifndef SELF_STRUCT_H
#define SELF_STRUCT_H

typedef enum
{
	NOT_MODIFY = 1,			//未修改
	MODIFY_NOT_SAVE,			//修改未保存
	MODIFY_SAVED,					//修改保存
}MODIFY_SAVE , *PMODIFY_SAVE;

#endif //