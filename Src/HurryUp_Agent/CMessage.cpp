#include "CMessage.h"

CMessage::CMessage()
{
}

CMessage* CMessage::GetInstance(void)
{
	static CMessage instance;
	return &instance;
}

void CMessage::PushSendMessage(int protocol, std::tstring metaInfo)
{
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);
	ST_PACKET_INFO* stPakcetInfo = new ST_PACKET_INFO(protocol, metaInfo);
	sendMessage.push(stPakcetInfo);
}

void CMessage::PushReceiveMessage(int protocol, std::tstring metaInfo)
{
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);
	ST_PACKET_INFO* stPakcetInfo = new ST_PACKET_INFO(protocol, metaInfo);
	receiveMessage.push(stPakcetInfo);
}

ST_PACKET_INFO* CMessage::PopSendMessage()
{
	std::lock_guard<std::mutex> lock_guard(sendMessageMutex);
	
	if (sendMessage.empty())
		return NULL;
	
	ST_PACKET_INFO* stPakcetInfo = sendMessage.front();
	sendMessage.pop();

	return stPakcetInfo;
}

ST_PACKET_INFO* CMessage::PopReceiveMessage()
{
	std::lock_guard<std::mutex> lock_guard(receiveMessageMutex);

	if (receiveMessage.empty())
		return NULL;

	ST_PACKET_INFO* stPakcetInfo = receiveMessage.front();
	receiveMessage.pop();

	return stPakcetInfo;
}
