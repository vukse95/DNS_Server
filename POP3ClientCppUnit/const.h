#ifndef _CONST_H_
#define _CONST_H_

#include "./kernelTypes.h"

//#define D_TESTING

const uint8 CH_AUTOMATE_TYPE_ID = 0x00;
const uint8 CL_AUTOMATE_TYPE_ID = 0x01;
const uint8 USER_AUTOMATE_TYPE_ID = 0x02;
const uint8 TEST_AUTOMATE_TYPE_ID = 0x03;

const uint8 CH_AUTOMATE_MBX_ID = 0x00;
const uint8 CL_AUTOMATE_MBX_ID = 0x01;
const uint8 USER_AUTOMATE_MBX_ID = 0x02;
const uint8 TEST_MBX_ID = 0x03;

//	Client states
enum ClStates {	FSM_Client_Idle,
				FSM_Client_Check_Local_Table,
				FSM_Client_Input_From_User, 
				FSM_Client_DNS_Request,
				FSM_Client_DNS_Recived };

// Channel messages
const uint16 MSG_Channel_Idle				= 0x0001;
const uint16 MSG_Channel_Server_Start		= 0x0002;
const uint16 MSG_Channel_Server_Recive		= 0x0003;
const uint16 MSG_Channel_Server_Send		= 0x0004;
const uint16 MSG_Channel_Server_Disconnect	= 0x0009;

const uint16 MSG_Channel_Switch				= 0x0005;

const uint16 MSG_Channel_Client_Start		= 0x0006;
const uint16 MSG_Channel_Client_Send		= 0x0007;
const uint16 MSG_Channel_Client_Recive		= 0x0008;

// Server(user) messages
const uint16 MSG_Server_idle					= 0x000a;
const uint16 MSG_Channel_To_Server_Request		= 0x000b;
const uint16 MSG_Server_To_Channel_Request_Sent	= 0x000c;
const uint16 MSG_Server_Check_Local_Table	    = 0x000d;
const uint16 MSG_Server_Root_Check			    = 0x000f;
const uint16 MSG_Server_To_Channel_Request_To_Root_Sent = 0x0010;
const uint16 MSG_Channel_To_Server_Request_From_Root_Recived = 0x0011;
const uint16 MSG_Server_Update_Table = 0x0012;


/*
const uint16 MSG_Mail = 0x0013;
const uint16 MSG_User_Save_Mail = 0x0015;
const uint16 MSG_User_Disconnected = 0x0014;
*/

#define ADRESS "localhost"
//#define ADRESS "mail.spymac.com"
//#define ADRESS "krtlab8"
#define PORT 110
#define SERVER_PORT 111

#define TIMER1_ID 0
#define TIMER1_COUNT 10
#define TIMER1_EXPIRED 0x20

#define TIMER2_ID 1
#define TIMER2_COUNT 10 //NOT SURE WHAT TIME - NOT IMPORTANT FOR NOW
#define TIMER2_EXPIRED 0x30

#define PARAM_DNS_REQUEST 0x01
#define PARAM_SERVER_TO_CHANNEL 0x02
#define PARAM_SERVER_TO_CHANNEL_CLIENT 0x03

#define ROOT_STATUS 1 // 0 - Local server, 1 - Root server


#endif //_CONST_H_