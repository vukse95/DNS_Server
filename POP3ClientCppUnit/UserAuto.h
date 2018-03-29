#ifndef _User_AUTO_H_
#define _User_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>
#include <cstring>
#include <fstream>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"



class UserAuto : public FiniteStateMachine {
	
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
	enum	UserStates {FSM_Server_Idle_State, 
						FSM_Server_Get_Request_State,
						FSM_Server_Check_Local_Table_State,
						FSM_Server_Root_Check_State,
						FSM_Server_No_DNS_State,
						FSM_Server_Send_Request_State,
						FSM_Server_Recive_Request_State,
						FSM_Server_Update_Table_State,
						FSM_Server_Pass_Request_To_Channel_State };

	
public:
	UserAuto();
	~UserAuto();
	
	void Initialize();
	
	void FSM_Server_Idle();
	void FSM_Server_Get_Request();
	void FSM_Server_Check_Local_Table();
	void FSM_Server_Request_In_Table_Found();
	void FSM_Server_Root_Check();
	void FSM_Server_No_DNS();

	void FSM_Server_Send_Request();
	void FSM_Server_Recive_Request();

	void FSM_Server_Update_Table();
	void FSM_Server_Pass_Request_To_Channel();

	void Start();
	
protected:
	
	char DNSRequest[256];
	char DNSIPAddress[256]; // IP hasn't got 256 char...
	
};

#endif /* _User_AUTO_H */