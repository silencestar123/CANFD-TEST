/*
 * MyCANFD.h
 *
 *  Created on: 2026年9月4日
 *      Author: Hxqy
 */

#ifndef USER_MYCANFD_H_
#define USER_MYCANFD_H_

#include "fdcan.h"
#include "stdint.h"

void MyCANFD_Init(void);
void FDCAN_Config(void);
void MyCANFD_Transmit_Test();
void MyCANFD_Transmit(uint32_t ID,uint8_t *data,uint32_t DataLength,uint32_t ID_Type);

extern  uint8_t CANFD_MODE;	/*如果值为0，运行SYSTICK里的程序，如果值为1运行其他程序*/
extern FDCAN_TxHeaderTypeDef FDCAN_TxHeader;
extern FDCAN_RxHeaderTypeDef FDCAN_RxHeader;
extern uint8_t can3_rxbuf[64];
#endif /* USER_MYCANFD_H_ */
