#include "CPolicy.h"

CPolicy::CPolicy(ST_POLICY_REQUEST _stPolicy)
{
	this->stPolicy = _stPolicy;	
	this->Init(_stPolicy.policyName);
}

CPolicy::~CPolicy()
{
}

bool CPolicy::Execute()
{
	if (!this->DownloadFile()) {
		core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("DownloadFile Fail"), errno);
		return false;
	}
	
	if (!this->ExtractFile()) {
		core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("ExtractFile Fail"), errno);
		return false;
	}

	//명령어 실행
	std::tstring result = Exec(EXE_COMMAND, this->savePath.c_str(), stPolicy.command.c_str());

	if (Split(result, "\n")[0] != std::tstring("0")) {
		core::Log_Error(TEXT("CService.cpp - [%s] : %s"), TEXT("Exec Command Error."), TEXT(result.c_str()));
		return false;
	}

	if (!this->RemoveFile()) {
		core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("RemoveFile Fail"), errno);
		return false;
	}

	return true;
}
