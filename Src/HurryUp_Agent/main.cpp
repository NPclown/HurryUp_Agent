#include "stdafx.h"
#include "CCommunication.h"
#include "CMatch.h"
#include "CCollector.h"
#include "CPolicy.h"
#include <cstdio>

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	SetLogger("Agent-Log", core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR | core::LOG_DEBUG);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Debug Mode (v2.0)"));
#else
	SetLogger("Agent-Log", core::LOG_INFO | core::LOG_WARN | core::LOG_ERROR);
	core::Log_Info(TEXT("main.cpp - [%s]"), TEXT("Program is Release Mode"));
#endif
	EnvironmentManager()->Init();
	CommunicationManager()->Init();

	CommunicationManager()->Connect();
	std::future<void> CommunicateStart = std::async(std::launch::async, &CCommunication::Start, CommunicationManager());
	std::future<void> CollectorManagerInit = std::async(std::launch::async, &CCollector::init, CollectorManager());
	std::future<void> MatchStart = std::async(std::launch::async, &CMatch::MatchMessage, MatchManager());
    return 0;
}