#include "CPolicy.h"

CPolicy::CPolicy(ST_POLICY_REQUEST _stPolicy)
{
	this->stPolicy = _stPolicy;	
	this->Init(_stPolicy.fileName);
}

CPolicy::~CPolicy()
{
}

bool CPolicy::Execute()
{
	bool result = true;
	sleep(1);

	if (!this->DownloadFile()) {
		core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("DownloadFile Fail"), errno);
		return false;
	}
	
	if (!this->ExtractFile()) {
		core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("ExtractFile Fail"), errno);
		return false;
	}

	DIR* dir = opendir(this->savePath.c_str());

	if (dir == NULL)
	{
		core::Log_Debug(TEXT("CPolicy.cpp - [%s]"), TEXT("Directory Open Fail"));
		return false;
	}

	struct dirent* de = NULL;

	//TODO :: cppcore 파일시스템을 뒤져보는 함수로 대체 가능

	std::regex re("P-U-[0-9]{2}(.sh)$");
	while ((de = readdir(dir)) != NULL)
	{
		if (std::regex_match(de->d_name, re)) {
			core::Log_Info(TEXT("CPolicy.cpp - [%s] : %s"), TEXT("Exec Script Name."), TEXT(de->d_name));

			std::tstring result = Exec(INSPECTION_COMMAND, this->savePath.c_str(), de->d_name);
			if (Split(result, "\n")[0] != std::tstring("0")) {
				core::Log_Error(TEXT("CPolicy.cpp - [%s] : %s"), TEXT("Exec Command Error."), TEXT(result.c_str()));
				result = false;
			}
		}
	}

	//if (!this->RemoveFile()) {
	//	core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("RemoveFile Fail"), errno);
	//	return false;
	//}

	return result;
}
