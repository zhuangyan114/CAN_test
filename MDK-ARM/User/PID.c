#include "PID.h"

void PID_Init(PID_Data* pPID_Data,
            float Kp,
            float Ki,
            float Kd,
            int16_t Limit_Max,
            int16_t LimitMin,
            int16_t Put_Max,
            int16_t Put_Min){
   pPID_Data->Kp = Kp;
   pPID_Data->Ki = Ki;
   pPID_Data->Kd = Kd;
   pPID_Data->Limit_Max = Limit_Max;
   pPID_Data->Limit_Min = LimitMin;
   pPID_Data->Put_Max = Put_Max;
   pPID_Data ->Put_Min = Put_Min;
   pPID_Data->LastData = 0;
   pPID_Data->LastTime = 0;
   pPID_Data->errPlus = 0;             
    
}


float PID_Set(PID_Data* pPID_Data,float NowData,float TargetData){
    float err = TargetData - NowData ;
    
    //更新积分数据
    pPID_Data->errPlus = (pPID_Data->errPlus + err);
    if(pPID_Data->errPlus > pPID_Data->Limit_Max)pPID_Data->errPlus = pPID_Data->Limit_Max;
    if(pPID_Data->errPlus < pPID_Data->Limit_Min)pPID_Data->errPlus = pPID_Data->Limit_Min;
    //获取当前时间
    uint32_t NowTime = HAL_GetTick();
    uint32_t DeltaTime = NowTime - pPID_Data->LastTime;
    if(DeltaTime == 0)DeltaTime =1;
    //计算微分数据
    float Differential = ( NowData - pPID_Data->LastData) / DeltaTime;//尝试的方案1
    //float Differential = ( pPID_Data->err[errLength - 2] - pPID_Data->err[errLength - 1])/(NowTime - pPID_Data->LastTime);//尝试的方案二


    float reData = (pPID_Data->Kp * err
                  + pPID_Data->Ki * pPID_Data->errPlus 
                  - pPID_Data->Kd * Differential);
    

    pPID_Data->LastTime = NowTime;
    pPID_Data->LastData = NowData;

    if(reData > pPID_Data->Put_Max)reData = pPID_Data->Put_Max;
    if(reData < pPID_Data->Put_Min)reData = pPID_Data->Put_Min;

    return reData;
}




