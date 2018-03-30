#include <stdio.h>
#include <conio.h>
#include <time.h>

#include "const.h"
#include "UserAuto.h"

bool g_ProgramEnd = false;

#define StandardMessageCoding 0x00

UserAuto::UserAuto() : FiniteStateMachine(USER_AUTOMATE_TYPE_ID, USER_AUTOMATE_MBX_ID, 0, 9, 1) {
}

UserAuto::~UserAuto() {
}


uint8 UserAuto::GetAutomate() {
	return USER_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the UserAuto with the mailbox. */
uint8 UserAuto::GetMbxId() {
	return USER_AUTOMATE_MBX_ID;
}

uint32 UserAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *UserAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void UserAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void UserAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void UserAuto::NoFreeInstances() {
	printf("[%d] UserAuto::NoFreeInstances()\n", GetObjectId());
}

void UserAuto::Reset() {
	printf("[%d] UserAuto::Reset()\n", GetObjectId());
}


void UserAuto::Initialize() {
	SetState(FSM_Server_Idle_State);
	
	//intitialization message handlers
	InitEventProc(FSM_Server_Idle_State, MSG_Server_idle, (PROC_FUN_PTR)&UserAuto::FSM_Server_Idle);
	InitEventProc(FSM_Server_Get_Request_State, MSG_Channel_To_Server_Request, (PROC_FUN_PTR)&UserAuto::FSM_Server_Get_Request);
	InitEventProc(FSM_Server_Check_Local_Table_State, MSG_Server_Check_Local_Table, (PROC_FUN_PTR)&UserAuto::FSM_Server_Check_Local_Table);
	//InitEventProc(FSM_Server_Check_Local_Table_State, MSG_Server_To_Channel_Request_Sent, (PROC_FUN_PTR)&UserAuto::FSM_Server_Request_In_Table_Found);
	InitEventProc(FSM_Server_Root_Check_State, MSG_Server_Root_Check, (PROC_FUN_PTR)&UserAuto::FSM_Server_Root_Check);
	InitEventProc(FSM_Server_No_DNS_State, MSG_Server_To_Channel_Request_Sent, (PROC_FUN_PTR)&UserAuto::FSM_Server_No_DNS);
	InitEventProc(FSM_Server_Send_Request_State, MSG_Server_To_Channel_Request_To_Root_Sent, (PROC_FUN_PTR)&UserAuto::FSM_Server_Send_Request);
	InitEventProc(FSM_Server_Recive_Request_State, MSG_Channel_To_Server_Request_From_Root_Recived, (PROC_FUN_PTR)&UserAuto::FSM_Server_Recive_Request);
	InitEventProc(FSM_Server_Update_Table_State, MSG_Server_Update_Table, (PROC_FUN_PTR)&UserAuto::FSM_Server_Update_Table);
	InitEventProc(FSM_Server_Pass_Request_To_Channel_State, MSG_Server_To_Channel_Request_Sent, (PROC_FUN_PTR)&UserAuto::FSM_Server_Pass_Request_To_Channel);
}

void UserAuto::Start(){

	// Set Server to IDLE
	PrepareNewMessage(0x00, MSG_Server_idle);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);

	// Set Channel to IDLE
	PrepareNewMessage(0x00, MSG_Channel_Idle);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);

	//SetState(FSM_User_Data_Input);
}

void UserAuto::FSM_Server_Idle(){
	
	SetState(FSM_Server_Get_Request_State);
}

void UserAuto::FSM_Server_Get_Request(){

	uint8* buffer = GetParam(PARAM_DNS_REQUEST);
	uint16 size = buffer[2];

	// Get data from Channel
	memcpy(DNSRequest, buffer + 4, size);
	DNSRequest[size] = 0;

	SetState(FSM_Server_Check_Local_Table_State);

	PrepareNewMessage(0x00, MSG_Server_Check_Local_Table);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);

}

void UserAuto::FSM_Server_Check_Local_Table(){

	// Check in table
	string line;
	string search;
	ifstream tableFile;
	size_t pos;
	int foundFlag = 0;


	// Null terminate char array
	for (int i = 0; i < strlen(DNSRequest); i++)
	{
		if (DNSRequest[i] == '\r')
		{
			DNSRequest[i] = '\0';
		}
	}

	// Copy from char* to String
	//search.copy(DNSRequest, strlen(DNSRequest));
	search = DNSRequest;

	tableFile.open("table.txt");

	if (tableFile.is_open())
	{
		while (tableFile.good())
		{
			getline(tableFile, line); // get line from file
			pos = line.find(search); // search
			if (pos != string::npos) // string::npos is returned if string is not found
			{
				cout << "Found!";
				getline(tableFile, line); // hop to next line to get IP address
				strcpy(DNSIPAddress, line.c_str()); // Copy from string to char* array
				foundFlag = 1;
				break;
			}
		}
		tableFile.close();
	}
	else
	{
		printf("Can't open table file!!");
	}
	if (foundFlag)
	{
		// IP found in local table, skip sending part
		PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_Sent);
		SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		AddParam(PARAM_SERVER_TO_CHANNEL, strlen(DNSIPAddress), (uint8*)DNSIPAddress);
		SendMessage(CH_AUTOMATE_MBX_ID);

		SetState(FSM_Server_Idle_State);
	}
	else
	{
		PrepareNewMessage(0x00, MSG_Server_Root_Check);
		SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(USER_AUTOMATE_MBX_ID);

		SetState(FSM_Server_Root_Check_State);
	}
}

void UserAuto::FSM_Server_Root_Check() {
	if (ROOT_STATUS)
	{
		// Root server
		PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_Sent);
		SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(USER_AUTOMATE_MBX_ID);

		SetState(FSM_Server_No_DNS_State);
	}
	else
	{
		// Local server
		PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_To_Root_Sent);
		SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(USER_AUTOMATE_MBX_ID);

		SetState(FSM_Server_Send_Request_State);
	}
}

void UserAuto::FSM_Server_No_DNS() {
	// TODO: NO IP WRITE TO DNSIPAddress (0.0.0.0)
	PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_Sent);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_SERVER_TO_CHANNEL, strlen(DNSIPAddress), (uint8*)DNSIPAddress);
	SendMessage(CH_AUTOMATE_MBX_ID);

	SetState(FSM_Server_Idle_State);
}

void UserAuto::FSM_Server_Send_Request() {

	// Send request to client app
	PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_To_Root_Sent);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_SERVER_TO_CHANNEL_CLIENT, strlen(DNSRequest), (uint8*)DNSRequest);
	SendMessage(CH_AUTOMATE_MBX_ID);

	SetState(FSM_Server_Recive_Request_State);
}

void UserAuto::FSM_Server_Recive_Request() {

	// Get response from client app (root server)
	uint8* buffer = GetParam(PARAM_SERVER_TO_CHANNEL_CLIENT);
	uint16 size = buffer[2];

	// Get data from Channel
	memcpy(DNSIPAddress, buffer + 4, size);
	DNSIPAddress[size] = 0;

	SetState(FSM_Server_Update_Table_State);

	PrepareNewMessage(0x00, MSG_Server_Update_Table);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);
}

void UserAuto::FSM_Server_Update_Table() {

	// Append to file Domain Name and IP Address
	ofstream tableFile;

	tableFile.open("table.txt");

	if (tableFile.is_open())
	{
		tableFile << DNSRequest << endl;
		tableFile << DNSIPAddress << endl;
		tableFile.close();
	}

	PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_Sent);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);

	SetState(FSM_Server_Pass_Request_To_Channel_State);
}

void UserAuto::FSM_Server_Pass_Request_To_Channel() {
	PrepareNewMessage(0x00, MSG_Server_To_Channel_Request_Sent);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_SERVER_TO_CHANNEL, strlen(DNSIPAddress), (uint8*)DNSIPAddress);
	SendMessage(CH_AUTOMATE_MBX_ID);

	SetState(FSM_Server_Idle_State);

	// Needs MSG FOR IDLE TO BE SET 
}
