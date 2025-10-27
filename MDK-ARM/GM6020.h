

#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "gpio.h"

//电机状态结构体
typedef struct{
    uint32_t MilliSec;        //当前时间戳 
    uint16_t Mechanical_Angle;//机械角度
    int16_t Speed_rpm;        //转速(RPM)
    int16_t Current;          //电流
    int16_t Temperature;      //温度(摄氏度)
}GM6020_Statu;





//多电机电压控制
void GM6020_SetVoltageAll(int16_t Voltage[4]);

//控制单个电机
void GM6020_SeyVoltage(uint8_t id,int16_t Voltage);

//通过回调函数获取电机数据
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);


