#include "CCommunication.h"

CCommunication::CCommunication()
{
	clientSocket = -1;
	connected = -1;
	memset(&serverAddress, 0, sizeof(serverAddress));
}

bool CCommunication::Live()
{
	return clientSocket != -1 && connected != -1;
}

CCommunication* CCommunication::GetInstance()
{
	static CCommunication instance;
	return &instance;
}

void CCommunication::Init()
{
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(EnvironmentManager()->GetServerIp());
	serverAddress.sin_port = htons((uint16_t)EnvironmentManager()->GetServerPort());
}

void CCommunication::Connect()
{
	std::lock_guard<std::mutex> lock_guard(this->connectionMutex);
	close(clientSocket);

	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (clientSocket == -1) {
		core::Log_Warn(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Socket Create Fail"), errno);
		close(clientSocket);
	}

	connected = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	if (!Live())
	{
		core::Log_Warn(TEXT("CCommunication.cpp - [%s] : %s"), TEXT("Connected Fail"), TEXT(inet_ntoa(serverAddress.sin_addr)));
		connected = -1;
		close(clientSocket);
	}
}

void CCommunication::Start()
{
	//TODO :: cppcore로 대체 필요
	std::future<void> CommunicateSend = std::async(std::launch::async, &CCommunication::Send, this);
	std::future<void> CommunicateRecv = std::async(std::launch::async, &CCommunication::Recv, this);
}

void CCommunication::Send()
{
	core::Log_Debug(TEXT("CCommunication.cpp - [%s]"), TEXT("Working Send In Thread"));
	ST_PACKET_INFO* stPacketInfo;

	while (1) {
		sleep(0);

		while (!Live()) {
			sleep(5);
			this->Connect();
		}

		stPacketInfo = MessageManager()->PopSendMessage();

		if (stPacketInfo == NULL)
			continue;

		std::tstring jsPacketInfo;
		core::WriteJsonToString(stPacketInfo, jsPacketInfo);

		std::tstring message = TEXT("BOBSTART") + jsPacketInfo + TEXT("BOBEND");
		int result = write(clientSocket, message.c_str(), message.length());

		if (result == -1)
			core::Log_Warn(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Send Error Code"), errno);
		else
		{
			core::Log_Debug(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Send Complete"), result);
			core::Log_Debug(TEXT("CCommunication.cpp - [%s] : %s"), TEXT("Send Message"), TEXT(message.c_str()));
		}

		delete stPacketInfo;
	}
}

void CCommunication::Recv()
{
	core::Log_Debug(TEXT("CCommunication.cpp - [%s]"), TEXT("Working Recv In Thread"));
	int messageLength;
	char message[BUFFER_SIZE + 1];

	while (1) {
		while (!Live()) {
			sleep(5);
			this->Connect();
		}

		messageLength = read(clientSocket, &message, BUFFER_SIZE);
		core::Log_Debug(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Read Complete"), messageLength);
		core::Log_Debug(TEXT("CCommunication.cpp - [%s] : %s"), TEXT("Recieve Message"), message);

		// 서버가 종료되었을 경우
		if (messageLength == 0)
		{
			Disconnect();
		}
		// 수신 받을 읽기에 문제가 생긴 경우, 보통 읽을 메세지가 없는 경우
		else if (messageLength < 0)
		{
			core::Log_Warn(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Read Error Code"), errno);
			core::Log_Debug(TEXT("CCommunication.cpp - [%s] : %s"), TEXT("Remain MessageBuffer"), TEXT(messageBuffers.c_str()));
		}
		// 메세지를 정상적으로 수신했을 경우
		else
		{
			message[messageLength] = 0;
			messageBuffers += message;

			while (1)
			{
				size_t start_location = messageBuffers.find("BOBSTART");
				size_t end_location = messageBuffers.find("BOBEND");
				core::Log_Debug(TEXT("CCommunication.cpp - [%s] : %d, %d"), TEXT("Find Position(Start, End)"), start_location, end_location);

				if (start_location == (size_t)-1 || end_location == (size_t)-1)
					break;

				ST_PACKET_INFO stPacketInfo;
				core::ReadJsonFromString(&stPacketInfo, messageBuffers.substr(start_location + 8, end_location - (start_location + 8)));

				MessageManager()->PushReceiveMessage(stPacketInfo.protocol, stPacketInfo.data);
				messageBuffers = messageBuffers.substr(end_location + 6);
			}
		}
		memset(message, 0, sizeof(message));
	}
}

void CCommunication::Disconnect()
{
	connected = -1;
	close(clientSocket);
}
