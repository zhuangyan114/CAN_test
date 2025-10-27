#include "GM6020.h"




void GM6020_SetVoltageAll(int16_t Voltage[4]){

  //Voltage :  -15000 ~ +15000
  uint8_t data[8] = {0};
    //配置CAN报文的关键结构体
  CAN_TxHeaderTypeDef TxHeader;
  TxHeader.StdId = 0x1ff;     //标准ID
	TxHeader.ExtId = 0;
  TxHeader.IDE = CAN_ID_STD;  //标准�?
  TxHeader.RTR = CAN_RTR_DATA;//数据�?
  TxHeader.DLC = 8;	//数据长度8字节
	TxHeader.TransmitGlobalTime = DISABLE;

  uint32_t TxMailbox;

  data[0] = (Voltage[0] >> 8)&0xff;
  data[1] = Voltage[0] &0xff; 
  data[2] = (Voltage[1] >> 8)&0xff;
  data[3] = Voltage[1] &0xff; 
  data[4] = (Voltage[2] >> 8)&0xff;
  data[5] = Voltage[2] &0xff; 
  data[6] = (Voltage[3] >> 8)&0xff;
  data[7] = Voltage[3] &0xff; 

  HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
}

/*
id为电机ID,1~4
*/
void GM6020_SeyVoltage(uint8_t id,int16_t Voltage){
    uint8_t data[8] = {0};
    //配置CAN报文的关键结构体
  CAN_TxHeaderTypeDef TxHeader;
  TxHeader.StdId = 0x1ff;     //标准ID
	TxHeader.ExtId = 0;
  TxHeader.IDE = CAN_ID_STD;  //标准�?
  TxHeader.RTR = CAN_RTR_DATA;//数据�?
  TxHeader.DLC = 8;	//数据长度8字节
	TxHeader.TransmitGlobalTime = DISABLE;

  uint32_t TxMailbox;

  data[2*id-2] = (Voltage>>8)&0xff;
  data[2*id-1] = Voltage&0xff;
  HAL_CAN_AddTxMessage(&hcan1,&TxHeader,data,&TxMailbox);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
  if(hcan == &hcan1){
    CAN_RxHeaderTypeDef rxHeader;
    extern GM6020_Statu GM_Statu[4];
    uint8_t rxData[8];
    if(HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&rxHeader,rxData) == HAL_OK){
      uint32_t id = rxHeader.StdId;
      switch (id){
        case 0x205:
        GM_Statu[0].MilliSec = HAL_GetTick();
        GM_Statu[0].Mechanical_Angle = (uint16_t)((rxData[0]<<8)|rxData[1]);
        GM_Statu[0].Speed_rpm = (int16_t)((rxData[2]<<8)|rxData[3]);
        GM_Statu[0].Current = (int16_t)((rxData[4]<<8)|rxData[5]);
        GM_Statu[0].Temperature = rxData[6];
        break;
        case 0x206:
        GM_Statu[2].MilliSec = HAL_GetTick();
        GM_Statu[1].Mechanical_Angle = (uint16_t)((rxData[0]<<8)|rxData[1]);
        GM_Statu[1].Speed_rpm = (int16_t)((rxData[2]<<8)|rxData[3]);
        GM_Statu[1].Current = (int16_t)((rxData[4]<<8)|rxData[5]);
        GM_Statu[1].Temperature = rxData[6];
        break;
        case 0x207:
        GM_Statu[2].MilliSec = HAL_GetTick();
        GM_Statu[2].Mechanical_Angle = (uint16_t)((rxData[0]<<8)|rxData[1]);
        GM_Statu[2].Speed_rpm = (int16_t)((rxData[2]<<8)|rxData[3]);
        GM_Statu[2].Current = (int16_t)((rxData[4]<<8)|rxData[5]);
        GM_Statu[2].Temperature = rxData[6];
        break;
        case 0x208:
        GM_Statu[3].MilliSec = HAL_GetTick();
        GM_Statu[3].Mechanical_Angle = (uint16_t)((rxData[0]<<8)|rxData[1]);
        GM_Statu[3].Speed_rpm = (int16_t)((rxData[2]<<8)|rxData[3]);
        GM_Statu[3].Current = (int16_t)((rxData[4]<<8)|rxData[5]);
        GM_Statu[3].Temperature = rxData[6];
        break;
      }
    }

  }

}

