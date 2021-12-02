#pragma once
#include "stdafx.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "CMessage.h"

//TODO :: TCP 통신 싱글톤 클래스 구현
class CCommunication
{
private:	
	int clientSocket;
	int connected;
	struct sockaddr_in serverAddress;
	std::tstring messageBuffers;
	std::mutex connectionMutex;

	CCommunication();

	bool Live();
public:
	static CCommunication* GetInstance();

	void Init();
	void Connect();
	void Start();
	void Send();
	void Recv();
	void Disconnect();
};

inline CCommunication* CommunicationManager()
{
	return CCommunication::GetInstance();
}