#ifndef _CH_AUTO_H_
#define _CH_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"


class ChAuto : public FiniteStateMachine {
	
	// for FSM
	StandardMessage StandardMsgCoding;
	
	MessageInterface *GetMessageInterface(uint32 id);
	void	SetDefaultHeader(uint8 infoCoding);
	void	SetDefaultFSMData();
	void	NoFreeInstances();
	void	Reset();
	uint8	GetMbxId();
	uint8	GetAutomate();
	uint32	GetObject();
	void	ResetData();
	
	// FSM States
	enum ChStates {
		FSM_Channel_Idle_State,
		FSM_Channel_Server_Start,
		FSM_Channel_Server,
		FSM_Channel_Server_Disconnect_State,
		FSM_Channel_Client_Start,
		FSM_Channel_Client,
		FSM_Channel_Switch_State };

	void FSM_Channel_Idle();
	void FSM_Channel_Start_Server();
	void FSM_Channel_Server_Recive();
	void FSM_Channel_Server_Send();
	void FSM_Channel_Server_Disconnect();

	void FSM_Channel_Client_Connect();
	void FSM_Channel_Client_Send();
	void FSM_Channel_Client_Recive();

	void FSM_Channel_Switch();
		
public:
	ChAuto();
	~ChAuto();
	
	//bool FSMMsg_2_NetMsg();
	void NetMsg_2_FSMMsg(const char* apBuffer, uint16 anBufferLength);

	void Initialize();

protected:
	static DWORD WINAPI ClientListener(LPVOID);
	
	SOCKET m_Socket;
	HANDLE m_hThread;
	DWORD m_nThreadID;
	uint16 m_nMaxMsgSize;

	// Server
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;

	char ServerInput[256];
	char ServerOutput[256];

};

#endif /* _CH_AUTO_H */