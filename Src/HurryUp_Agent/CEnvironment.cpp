#include "CEnvironment.h"

CEnvironment::CEnvironment(void)
{
	this->serverIp = "127.0.0.1";
	this->serverPort = "55555";
	this->serverFilePort = "2031";
	this->serialNumber = "None";
	this->environment = "None";
}

CEnvironment::~CEnvironment(void)
{
}

CEnvironment* CEnvironment::GetInstance(void)
{
	static CEnvironment instance;
	return &instance;
}

void CEnvironment::Init()
{
	if (!core::PathFileExistsA(ENV_PATH)) {
		core::Log_Error(TEXT("CUtils.cpp - [%s] : %s"), TEXT("Env.json Not Exisit"), ENV_PATH);
		exit(1);
	}

	std::tstring errMessage;
	core::ReadJsonFromFile(this, ENV_PATH, &errMessage);

	if (errMessage != "") {
		core::Log_Error(TEXT("CUtils.cpp - [%s] : %s"), TEXT("ReadJsonFromFile Error"), TEXT(errMessage.c_str()));
		exit(1);
	}

	if (CEnvironment() == *this) {
		core::Log_Error(TEXT("CUtils.cpp - [%s]"), TEXT("env.json Type Error"));
		exit(1);
	}
}

const char* CEnvironment::GetServerIp()
{
	return this->serverIp.c_str();
}

int CEnvironment::GetServerPort()
{
	return atoi(this->serverPort.c_str());
}

int CEnvironment::GetServerFilePort()
{
	return atoi(this->serverFilePort.c_str());
}

std::tstring CEnvironment::GetSerialNumber()
{
	return this->serialNumber;
}

std::tstring CEnvironment::GetEnvironment()
{
	return this->environment;
}
