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
enum ClStates {	FSM_Cl_Ready, 
				FSM_Cl_Connecting, 
				FSM_Cl_Authorising,
				FSM_Cl_User_Check,
				FSM_Cl_Pass_Check,
				FSM_Cl_Mail_Check,
				FSM_Cl_Receiving,
				FSM_Cl_Deleting,
				FSM_Cl_Reseting,
				FSM_Cl_Disconnecting };

// channel messages
const uint16 MSG_Channel_Idle				= 0x0001;
const uint16 MSG_Channel_Server_Start		= 0x0002;
const uint16 MSG_Channel_Server_Recive		= 0x0003;
const uint16 MSG_Channel_Server_Send		= 0x0004;
const uint16 MSG_Channel_Server_Disconnect	= 0x0009;

const uint16 MSG_Channel_Switch				= 0x0005;

const uint16 MSG_Channel_Client_Start		= 0x0006;
const uint16 MSG_Channel_Client_Send		= 0x0007;
const uint16 MSG_Channel_Client_Recive		= 0x0008;

// client messages
const uint16 MSG_Cl_Connection_Reject	= 0x000a;
const uint16 MSG_Cl_Connection_Accept	= 0x000b;
const uint16 MSG_User_Name_Password		= 0x000c;
const uint16 MSG_MSG					= 0x000d;
const uint16 MSG_Cl_Disconected			= 0x000f;

// user messages
const uint16 MSG_Set_All				= 0x0010;
const uint16 MSG_User_Connected			= 0x0011;
const uint16 MSG_User_Connecton_Fail	= 0x0012;
const uint16 MSG_Mail					= 0x0013;
const uint16 MSG_User_Save_Mail			= 0x0015;
const uint16 MSG_User_Disconnected		= 0x0014;


#define ADRESS "localhost"
//#define ADRESS "mail.spymac.com"
//#define ADRESS "krtlab8"
#define PORT 110

#define TIMER1_ID 0
#define TIMER1_COUNT 10
#define TIMER1_EXPIRED 0x20

#define PARAM_DATA 0x01
#define PARAM_Name 0x02
#define PARAM_Pass 0x03


#endif //_CONST_H_