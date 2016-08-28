#ifndef __LIB_RECORD_H__
#define __LIB_RECORD_H__

enum PATH_ID
{
    WORK_PATH = 1,//P'
    BACK_PATH = 2//P''
};

//将结果记录到result缓冲区
extern void record_result(const PATH_ID path_id, const unsigned short edge);
//清除result缓冲区为初始状态
extern void clear_result();

#endif
