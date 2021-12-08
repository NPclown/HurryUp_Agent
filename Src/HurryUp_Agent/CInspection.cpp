#include "CInspection.h"
#include "CMessage.h"

CInspection::CInspection(ST_INSPECTION_REQUEST _stInspection)
{
	this->stInspection = _stInspection;
	this->Init(_stInspection.fileName);
}

CInspection::~CInspection()
{
}

bool CInspection::Execute()
{
	bool finalResult = true;

 	if (!this->DownloadFile()) {
		core::Log_Warn(TEXT("CInspection.cpp - [%s] : %d"), TEXT("DownloadFile Fail"), errno);
		return false;
	}

	if (!this->ExtractFile()) {
		core::Log_Warn(TEXT("CInspection.cpp - [%s] : %d"), TEXT("ExtractFile Fail"), errno);
		return false;
	}

	DIR* dir = opendir(this->savePath.c_str());

	if (dir == NULL)
	{
		core::Log_Debug(TEXT("CInspection.cpp - [%s]"), TEXT("Directory Open Fail"));
		return false;
	}

	struct dirent* de = NULL;

	//TODO :: cppcore 파일시스템을 뒤져보는 함수로 대체 가능


	std::regex re("U-[0-9]{2}(.sh)$");
	while ((de = readdir(dir)) != NULL)
	{
		if (std::regex_match(de->d_name, re)) {
			std::tstring result = Exec(INSPECTION_COMMAND, this->savePath.c_str(),de->d_name);

			if (Split(result, "\n")[0] != std::tstring("0")) {
				core::Log_Error(TEXT("CPolicy.cpp - [%s] : %s"), TEXT("Exec Command Error."), TEXT(result.c_str()));
				return false;
			}

			std::tstring errMessage;
			ST_INSPECTION_RESULT stInspectionResult;
			core::ReadJsonFromFile(&stInspectionResult, std::tstring(this->savePath + core::ExtractFileNameWithoutExt(de->d_name) + ".json").c_str(), &errMessage);

			finalResult = finalResult && stInspectionResult.result;

			ST_INSPECTION_INFO stInspectionInfo;
			stInspectionInfo.inspectionIdx = this->stInspection.inspectionIdx;
			stInspectionInfo.inspectionName = this->stInspection.inspectionName;
			stInspectionInfo.result = stInspectionResult;
			stInspectionInfo.resultIdx = this->stInspection.resultIdx;

			std::tstring jsMessage;
			core::WriteJsonToString(&stInspectionInfo, jsMessage);

			MessageManager()->PushSendMessage(INSPECTION_RESULT, jsMessage);
		}
	}

	if (!this->RemoveFile()) {
		core::Log_Warn(TEXT("CPolicy.cpp - [%s] : %d"), TEXT("RemoveFile Fail"), errno);
		return false;
	}

	return finalResult;
}
