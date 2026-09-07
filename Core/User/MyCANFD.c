/*
 * MyCANFD.c
 *
 *  Created on: 2026年9月4日
 *      Author: Hxqy
 */
#include "MyCANFD.h"
FDCAN_TxHeaderTypeDef FDCAN_TxHeader;
FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
FDCAN_TxHeaderTypeDef FDCAN_TxHeader_TEST;
uint8_t Date_TEXT[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t can3_rxbuf[64]={0};
uint8_t CANFD_MODE=0;	/*如果值为0，运行SYSTICK里的程序，如果值为1运行其他程序*/
/**
 * @brief CANFD初始化函数
 * 
 */
void MyCANFD_Init(void)
{
    

     FDCAN_Config();
     CANFD_MODE=0;

}
void FDCAN_Config(void)
{
     FDCAN_FilterTypeDef FDCAN_FilterInitStructure;
     FDCAN_FilterInitStructure.FilterConfig=FDCAN_FILTER_TO_RXFIFO0;
     FDCAN_FilterInitStructure.FilterID1=0x0000;
     FDCAN_FilterInitStructure.FilterID2=0x7FF;
     FDCAN_FilterInitStructure.FilterIndex=0;
     FDCAN_FilterInitStructure.FilterType= FDCAN_FILTER_RANGE;
     FDCAN_FilterInitStructure.IdType= FDCAN_STANDARD_ID;

     if (HAL_FDCAN_ConfigFilter(&hfdcan3, &FDCAN_FilterInitStructure) != HAL_OK)
       {
         Error_Handler();
       }
     FDCAN_FilterInitStructure.IdType=FDCAN_EXTENDED_ID;
     FDCAN_FilterInitStructure.FilterIndex=1;
     FDCAN_FilterInitStructure.FilterType=FDCAN_FILTER_RANGE;
     FDCAN_FilterInitStructure.FilterConfig=FDCAN_FILTER_TO_RXFIFO0;
     FDCAN_FilterInitStructure.FilterID1=0x00000000;
     FDCAN_FilterInitStructure.FilterID2=0x1FFFFFFF;
     if (HAL_FDCAN_ConfigFilter(&hfdcan3, &FDCAN_FilterInitStructure) != HAL_OK)
            {
              Error_Handler();
            }
     if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
       {
         Error_Handler();
       }

       /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
       if (HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
       {
         Error_Handler();
       }


       FDCAN_TxHeader.Identifier=0x000000000;
       FDCAN_TxHeader.IdType=FDCAN_STANDARD_ID;;
       FDCAN_TxHeader.TxFrameType=FDCAN_DATA_FRAME;
       FDCAN_TxHeader.DataLength=FDCAN_DLC_BYTES_8;
       FDCAN_TxHeader.ErrorStateIndicator=FDCAN_ESI_PASSIVE;
       FDCAN_TxHeader.BitRateSwitch=FDCAN_BRS_ON;
       FDCAN_TxHeader.FDFormat=FDCAN_FD_CAN;
       FDCAN_TxHeader.TxEventFifoControl=FDCAN_NO_TX_EVENTS;
       FDCAN_TxHeader.MessageMarker=0;

       /* Configure and enable Tx Delay Compensation, required for BRS mode.
               TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
               TdcFilter default recommended value: 0 */
         HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan3, hfdcan3.Init.DataPrescaler * hfdcan3.Init.DataTimeSeg1, 0);
         HAL_FDCAN_EnableTxDelayCompensation(&hfdcan3);
         HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_WARNING | FDCAN_IT_ERROR_PASSIVE, 0);

         /* Start the FDCAN module */
         if (HAL_FDCAN_Start(&hfdcan3) != HAL_OK)
         {
           Error_Handler();
         }

}
void MyCANFD_Transmit_Test()
{


     FDCAN_TxHeader_TEST.BitRateSwitch=FDCAN_BRS_OFF;
     FDCAN_TxHeader_TEST.DataLength=FDCAN_DLC_BYTES_8;
     FDCAN_TxHeader_TEST.ErrorStateIndicator=FDCAN_ESI_PASSIVE;
     FDCAN_TxHeader_TEST.FDFormat=FDCAN_FD_CAN;
     FDCAN_TxHeader_TEST.Identifier= 0x001;
     FDCAN_TxHeader_TEST.IdType=FDCAN_STANDARD_ID;
     FDCAN_TxHeader_TEST.MessageMarker=0;
     FDCAN_TxHeader_TEST.TxEventFifoControl=FDCAN_NO_TX_EVENTS ;
     FDCAN_TxHeader_TEST.TxFrameType=FDCAN_DATA_FRAME;

      // 2. 将报文添加到发送FIFO队列
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3,&FDCAN_TxHeader_TEST,Date_TEXT) != HAL_OK)
    {
        // 添加失败，进行错误处理
        Error_Handler();
    }

}
void MyCANFD_Transmit(uint32_t ID,uint8_t *data,uint32_t DataLength,uint32_t ID_Type)
{


	 FDCAN_TxHeader.BitRateSwitch=FDCAN_BRS_OFF;
	 FDCAN_TxHeader.DataLength=DataLength;
	 FDCAN_TxHeader.ErrorStateIndicator=FDCAN_ESI_PASSIVE;
	 FDCAN_TxHeader.FDFormat=FDCAN_FD_CAN;
	 FDCAN_TxHeader.Identifier=ID;
	 FDCAN_TxHeader.IdType=ID_Type;
	 FDCAN_TxHeader.MessageMarker=0;
	 FDCAN_TxHeader.TxEventFifoControl=FDCAN_NO_TX_EVENTS ;
	 FDCAN_TxHeader.TxFrameType=FDCAN_DATA_FRAME;

      // 2. 将报文添加到发送FIFO队列
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3,&FDCAN_TxHeader,data) != HAL_OK)
    {
        // 添加失败，进行错误处理
        Error_Handler();
    }

}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
    if(hfdcan->Instance == FDCAN3)
    {
      /* Retrieve Rx messages from RX FIFO0 */
      if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &FDCAN_RxHeader, can3_rxbuf) != HAL_OK)
      {
        Error_Handler();
      }

      CANFD_MODE=1;

    }


   }

}
/**
  * @brief FDCAN 错误状态回调（HAL库自动调用）
  * @param hfdcan        FDCAN句柄
  * @param ErrorStatusITs 触发中断的错误标志组合（例如 FDCAN_IT_BUS_OFF）
  */
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
    /* 检查是否是本FDCAN3触发的Bus-Off中断 */
    if (hfdcan->Instance == FDCAN3)
    {
        if (ErrorStatusITs & FDCAN_IT_BUS_OFF)
        {
            /* 核心恢复操作：清除 INIT 位，让硬件自动等待总线空闲后恢复 */
            CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);

            /* 清除 Bus-Off 中断挂起标志（使用正确的宏和标志位） */
            __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_IR_BO);
        }
    }
}
