#include "stdafx.h"
#include "CCommunication.h"
#include "CDevice.h"
#include "CMonitoring.h"
#include "CMatch.h"
#include "CCollector.h"

#include <cstdio>

int main(int argc, char* argv[])
{
    ST_ENV env;
	env.serverIP = argv[1];
	env.serverPort = argv[2];

#ifdef _DEBUG
	SetLogger("Agent-Log", core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR | core::LOG_DEBUG);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Debug Mode"));
#else
	SetLogger("Agent-Log", core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Release Mode"));
#endif
	//TODO :: 관리자 권한 확인 후 아닐 시 종료
	//TODO :: 유효하지 않은 IP 주소 일시 종료

	CommunicationManager()->Init(env.serverIP, env.serverPort);
	CommunicationManager()->Connect();
	CollectorManager()->init();

	std::future<void> CommunicateStart = std::async(std::launch::async, &CCommunication::Start, CommunicationManager());
	std::future<void> MatchStart = std::async(std::launch::async, &CMatch::MatchMessage, MatchManager());

	//DeviceTest();

	//ST_MONITOR_TARGET target;
	//target.processName = "process";
	//target.logPath = "/var/log/auth1.log";
	//std::cout << MonitoringManager()->AddMonitoringTarget(target) << std::endl;
	//MonitoringManager()->StartMonitoring();

	//DeviceProcesTest();

    return 0;
}