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
int tempFL;
int tempFR;
int tempBL;
int tempBR;
int pressFL;
int pressFR;
int pressBL;
int pressBR;

/* End BUSMASTER global variable */


/* Start BUSMASTER Function Prototype  */
GCC_EXTERN void GCC_EXPORT OnTimer_tireSend_100( );
/* End BUSMASTER Function Prototype  */

/* Start BUSMASTER Function Wrapper Prototype  */
/* End BUSMASTER Function Wrapper Prototype  */


/* Start BUSMASTER generated function - OnTimer_tireSend_100 */
void OnTimer_tireSend_100( )
{
/* TODO */
tempFL= rand()%5 + 90;
tempFR= rand()%5 + 90;
tempBL= rand()%5 + 90;
tempBR= rand()%5 + 90;

pressFL= rand()%2 + 4;
pressFR= rand()%2 + 4;
pressBL= rand()%2 + 4;
pressBR= rand()%2 + 4;

tx.id=0x45;
tx.dlc=5;

tx.data[0]=1;
tx.data[1]=tempFL >> 8;
tx.data[2]=tempFL;
tx.data[3]=pressFL >> 8;
tx.data[4]=pressFL;
SendMsg(tx);

tx.data[0]=2;
tx.data[1]=tempFR >> 8;
tx.data[2]=tempFR;
tx.data[3]=pressFR >> 8;
tx.data[4]=pressFR;
SendMsg(tx);

tx.data[0]=3;
tx.data[1]=tempBL >> 8;
tx.data[2]=tempBL;
tx.data[3]=pressBL >> 8;
tx.data[4]=pressBL;
SendMsg(tx);

tx.data[0]=4;
tx.data[1]=tempBR >> 8;
tx.data[2]=tempBR;
tx.data[3]=pressBR >> 8;
tx.data[4]=pressBR;
SendMsg(tx);


SendMsg(tx);
}/* End BUSMASTER generated function - OnTimer_tireSend_100 */
