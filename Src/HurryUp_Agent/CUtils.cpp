#include "CUtils.h"
#include <fstream>

#define DIRPATH TEXT("/var/log/hurryup/")
#define AGENT_DIRPATH TEXT("/var/log/hurryup/agent/")
#define ENV_PATH TEXT("/var/log/hurryup/agent/env.json")

void SetEnvironment(ST_ENV_INFO* env)
{
	if (!core::PathFileExistsA(ENV_PATH)) {
		core::Log_Error(TEXT("CUtils.cpp - [%s] : %s"), TEXT("Env.json Not Exisit"), ENV_PATH);
		exit(1);
	}

	std::tstring errMessage;
	core::ReadJsonFromFile(env, ENV_PATH, &errMessage);

	if (errMessage != "") {
		core::Log_Error(TEXT("CUtils.cpp - [%s] : %s"), TEXT("ReadJsonFromFile Error"), TEXT(errMessage.c_str()));
		exit(1);
	}

	if (ST_ENV_INFO() == *env) {
		core::Log_Error(TEXT("CUtils.cpp - [%s]"), TEXT("env.json Type Error"));
		exit(1);
	}
}

void SetLogger(std::tstring _name, DWORD _inputOption)
{
	CheckDirectory(DIRPATH);
	CheckDirectory(AGENT_DIRPATH);

	std::tstring strModuleFile = core::GetFileName();
	std::tstring strModuleDir = core::ExtractDirectory(strModuleFile);
	std::tstring strModuleName = core::ExtractFileNameWithoutExt(strModuleFile);
	std::tstring strLogFile = AGENT_DIRPATH + strModuleName + TEXT(".log");


	core::ST_LOG_INIT_PARAM_EX init;
	init.strLogFile = strLogFile;
	init.strID = TEXT(_name);
	init.dwInputFlag = _inputOption;
	init.dwOutputFlag = core::LOG_OUTPUT_FILE | core::LOG_OUTPUT_CONSOLE | core::LOG_OUTPUT_DBGWND;
	init.dwMaxFileSize = 10 * 1000 * 1000;
	init.dwMaxFileCount = 10;
	init.nLogRotation = core::LOG_ROTATION_SIZE;
	core::InitLog(init);
}

std::tstring exec(const char* cmd)
{
	std::array<char, 256> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	return result;
}

std::tstring ReadContent(const char* path)
{
	std::ifstream openFile(path);

	std::tstring content;
	if (openFile.is_open())
	{
		std::tstring line;
		while (getline(openFile, line)) {
			content += line;
			std::cout << line << std::endl;
		}
		return content;
	}
	return "";
}

std::vector<std::string> split(std::string input, char delimiter)
{
	std::vector<std::string> answer;
	std::stringstream ss(input);
	std::string temp;

	while (getline(ss, temp, delimiter)) {
		answer.push_back(temp);
	}

	return answer;
}

int FindFileEndPosition(std::ifstream& file)
{
	if (file.is_open()) {
		file.seekg(0, std::ios::end);
		int size = file.tellg();
		return size;
	}
	else {
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s]"), TEXT("File Open Fail"));
		return -1;
	}
}

std::string GetTimeStamp()
{
	time_t curTime = time(NULL);
	char timestamp[20];
	struct tm* a = localtime(&curTime);

	sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d", 1900 + a->tm_year, a->tm_mon + 1, a->tm_mday, a->tm_hour, a->tm_min, a->tm_sec);
	return timestamp;
}

void CheckDirectory(std::tstring _path)
{
	if (core::PathFileExistsA(_path.c_str()) == 0)
		core::CreateDirectory(_path.c_str());
}