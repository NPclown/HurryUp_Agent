#pragma once
#include "stdafx.h"

//TODO :: 메시지 관리 싱글톤 클래스 구현
class CMessage
{
private:
	std::queue<ST_PACKET_INFO*> receiveMessage;
	std::queue<ST_PACKET_INFO*> sendMessage;
	std::mutex receiveMessageMutex;
	std::mutex sendMessageMutex;

	CMessage();
public:
	static CMessage* GetInstance(void);
	void PushSendMessage(int protocol, std::tstring metaInfo);
	void PushReceiveMessage(int protocol, std::tstring metaInfo);
	ST_PACKET_INFO* PopSendMessage();
	ST_PACKET_INFO* PopReceiveMessage();
};

inline CMessage* MessageManager()
{
	return CMessage::GetInstance();
}
