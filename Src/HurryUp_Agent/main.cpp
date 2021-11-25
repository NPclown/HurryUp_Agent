#include "stdafx.h"
#include "CCommunication.h"
#include "CDevice.h"
#include "CMonitoring.h"
#include "CMatch.h"
#include "CCollector.h"

#include <cstdio>

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	SetLogger("Agent-Log", core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR | core::LOG_DEBUG);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Debug Mode"));
#else
	SetLogger("Agent-Log", core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Release Mode"));
#endif

	ST_ENV_INFO env;
	SetEnvironment(&env);

	CommunicationManager()->Init(env.serverIp, env.serverPort);
	CollectorManager()->DeviceInstance()->SetEnv(env.serialNumber, env.environment);
	CollectorManager()->MonitoringInstance()->SetEnv(env.serialNumber, env.environment);

	CommunicationManager()->Connect();
	std::future<void> CommunicateStart = std::async(std::launch::async, &CCommunication::Start, CommunicationManager());
	std::future<void> CollectorManagerInit = std::async(std::launch::async, &CCollector::init, CollectorManager());
	std::future<void> MatchStart = std::async(std::launch::async, &CMatch::MatchMessage, MatchManager());

    return 0;
}