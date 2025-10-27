#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "gpio.h"

#define errLength 10

//PID数据结构体
typedef struct{
    uint32_t LastTime ;        //上一次的时间
    float LastData;            //上一次数据
    float Kp;
    float Ki;
    float Kd;
    double errPlus;
    int16_t Limit_Max;         //积分限幅
    int16_t Limit_Min;
    int16_t Put_Max;           //输出限幅
    int16_t Put_Min;
}PID_Data;

void PID_Init(PID_Data* pPID_Data,
            float Kp,
            float Ki,
            float Kd,
            int16_t Limit_Max,
            int16_t LimitMin,
            int16_t Put_Max,
            int16_t Put_Min);



//通过PID控制速度
//NowData:现在的数据
//TargetData:目标的数据
float PID_Set(PID_Data* pPID_Data,float NowData,float TargetData);

