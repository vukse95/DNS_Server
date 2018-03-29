#include <stdio.h>

#include "const.h"
#include "ChAuto.h"

#define StandardMessageCoding 0x00

ChAuto::ChAuto() : FiniteStateMachine(CH_AUTOMATE_TYPE_ID, CH_AUTOMATE_MBX_ID, 1, 8, 3) {
}

ChAuto::~ChAuto() {
}

uint8 ChAuto::GetAutomate() {
	return CH_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the ChAuto with the mailbox. */
uint8 ChAuto::GetMbxId() {
	return CH_AUTOMATE_MBX_ID;
}

uint32 ChAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *ChAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void ChAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void ChAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void ChAuto::NoFreeInstances() {
	printf("[%d] ChAuto::NoFreeInstances()\n", GetObjectId());
}

void ChAuto::Reset() {
	printf("[%d] ChAuto::Reset()\n", GetObjectId());
}


void ChAuto::Initialize() {
	SetState(FSM_Channel_Idle_State);

	//intitialization message handlers
	InitEventProc(FSM_Channel_Idle_State, MSG_Channel_Idle, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Idle);

	InitEventProc(FSM_Channel_Server_Start, MSG_Channel_Server_Start, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Start_Server);
	InitEventProc(FSM_Channel_Server, MSG_Channel_Server_Send, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Server_Send);
	InitEventProc(FSM_Channel_Server, MSG_Channel_Server_Recive, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Server_Recive);
	InitEventProc(FSM_Channel_Server_Disconnect_State, MSG_Channel_Server_Disconnect, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Server_Disconnect);

	InitEventProc(FSM_Channel_Switch_State, MSG_Channel_Switch, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Switch);

	InitEventProc(FSM_Channel_Switch_State, MSG_Channel_Server_Send, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Server_Send);

	InitEventProc(FSM_Channel_Switch_State, MSG_Server_To_Channel_Request_To_Root_Sent, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Client_Connect);

	//InitEventProc(FSM_Channel_Client_Start, MSG_Channel_Client_Start, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Client_Connect);
	//InitEventProc(FSM_Channel_Client, MSG_Channel_Client_Send, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Client_Send);
	//InitEventProc(FSM_Channel_Client, MSG_Channel_Client_Recive, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Client_Recive);
}

void ChAuto::FSM_Channel_Idle() {
	//Start Server

	PrepareNewMessage(0x00, MSG_Channel_Server_Start);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);

	SetState(FSM_Channel_Server_Start);
}

void ChAuto::FSM_Channel_Start_Server() {

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERVER_PORT);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Bind done");


	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);
	new_socket = accept(s, (struct sockaddr *)&client, &c);
	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
	}

	puts("Connection accepted");

	SetState(FSM_Channel_Server);

	PrepareNewMessage(0x00, MSG_Channel_Server_Recive);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);
}

void ChAuto::FSM_Channel_Server_Recive() {
	if (recv(c, ServerInput, strlen(ServerInput), 0))
	{
		printf("Error reciving request from clinet!");
	}

	PrepareNewMessage(0x00, MSG_Channel_To_Server_Request);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_DNS_REQUEST, strlen(ServerInput), (uint8*)ServerInput);
	SendMessage(USER_AUTOMATE_MBX_ID);

	SetState(FSM_Channel_Switch_State);
}

void ChAuto::FSM_Channel_Switch() {

}

void ChAuto::FSM_Channel_Server_Send() {

}

void ChAuto::FSM_Channel_Server_Disconnect() {
	closesocket(s);
	WSACleanup();
}

void ChAuto::FSM_Channel_Client_Connect() {

	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)
	{
		return;
	}

	/* Try to resolve the server name. */
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));

	unsigned int addr = inet_addr(ADRESS);
	if (addr != INADDR_NONE)
	{
		server_addr.sin_addr.s_addr = addr;
		server_addr.sin_family = AF_INET;
	}
	else
	{
		hostent* hp = gethostbyname(ADRESS);
		if (hp != 0)
		{
			memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
			server_addr.sin_family = hp->h_addrtype;
		}
		else
		{
			return;
		}
	}
	server_addr.sin_port = htons(PORT);

	/* Create the socket. */
	m_Socket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET) {
		return;
	}

	/* Try to reach the server. */
	if (connect(m_Socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		/* Here some additional cleanup should be done. */
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return;
	}



	/* Then, start the thread that will listen on the the newly created socket. */
	m_hThread = CreateThread(NULL, 0, ClientListener, (LPVOID)this, 0, &m_nThreadID);
	if (m_hThread == NULL) {
		/* Cannot create thread.*/
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return;
	}
}


void ChAuto::NetMsg_2_FSMMsg(const char* apBuffer, uint16 anBufferLength) {

	PrepareNewMessage(0x00, MSG_Channel_To_Server_Request_From_Root_Recived);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_SERVER_TO_CHANNEL_CLIENT, anBufferLength, (uint8 *)apBuffer);
	SendMessage(USER_AUTOMATE_MBX_ID);

	// KIll client

}

DWORD ChAuto::ClientListener(LPVOID param) {
	ChAuto* pParent = (ChAuto*)param;
	int nReceivedBytes = 0;
	char* buffer = new char[255];

	//nReceivedBytes = recv(pParent->m_Socket, buffer, 255, 0);
	if (nReceivedBytes < 0) {
		DWORD err = WSAGetLastError();
	}
	else {
		//pParent->FSM_Ch_Connecting_Sock_Connection_Acccept();

		/* Receive data from the network until the socket is closed. */
		do {
			nReceivedBytes = recv(pParent->m_Socket, buffer, 255, 0);
			if (nReceivedBytes == 0)
			{
				printf("Disconnected from server!\n");
				pParent->FSM_Channel_Server_Disconnect();
				break;
			}
			if (nReceivedBytes < 0) {
				printf("error\n");
				DWORD err = WSAGetLastError();
				break;
			}
			pParent->NetMsg_2_FSMMsg(buffer, nReceivedBytes);

			Sleep(1000);

		} while (1);

	}

	delete[] buffer;
	return 1;
}