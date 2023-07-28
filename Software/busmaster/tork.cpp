/* This file is generated by BUSMASTER */
/* VERSION [1.2] */
/* BUSMASTER VERSION [3.2.2] */
/* PROTOCOL [CAN] */

/* Start BUSMASTER include header */
#include <Windows.h>
#include <CANIncludes.h>
/* End BUSMASTER include header */


/* Start BUSMASTER global variable */
STCAN_MSG tx;
int torkBL;
int torkBR;
int torkMOT;
/* End BUSMASTER global variable */


/* Start BUSMASTER Function Prototype  */
GCC_EXTERN void GCC_EXPORT OnTimer_torkTmr_50( );
/* End BUSMASTER Function Prototype  */

/* Start BUSMASTER Function Wrapper Prototype  */
/* End BUSMASTER Function Wrapper Prototype  */


/* Start BUSMASTER generated function - OnTimer_torkTmr_50 */
void OnTimer_torkTmr_50( )
{
/* TODO */
torkBL= rand()%5 + 46;
torkBR= rand()%5 + 46;
torkMOT= rand()%5 + 92;
tx.id=0x40;
tx.dlc=8;
tx.data[0]=torkBL >> 8;
tx.data[1]=torkBL;
tx.data[2]=torkBR >> 8;
tx.data[3]=torkBR;
tx.data[4]=torkMOT >> 8;
tx.data[5]=torkMOT;
tx.data[6]=0;
tx.data[7]=0;
SendMsg(tx);
}/* End BUSMASTER generated function - OnTimer_torkTmr_50 */
