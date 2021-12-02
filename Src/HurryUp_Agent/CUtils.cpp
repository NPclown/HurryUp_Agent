#include "CUtils.h"
#include <fstream>

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

std::vector<std::tstring> Split(std::tstring input, std::tstring delimiter)
{
	std::vector<std::tstring> result;
	std::tstring temp = input;
	std::tstring next = "";

	while (1) {
		std::tstring r = core::Split(temp, delimiter, &next);
		result.push_back(r);

		if (r == temp)
			break;

		temp = next;
	}

	return result;
}

long long int FindFileEndPosition(std::ifstream& file)
{
	if (file.is_open()) {
		file.seekg(0, std::ios::end);
		long long int size = file.tellg();
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

std::string GeneratorStringNumber()
{
	srand(time(NULL));
	std::string serial;

	for (int i = 0; i < 10; i++) {
		serial += std::to_string(rand() % 10);
	}
		
	return serial;
}

std::tstring Exec(const char* _command, ...)
{
	va_list ap;
	char command[BUFFER_SIZE];

	va_start(ap, _command);
	vsprintf(command, _command, ap);
	va_end(ap);

	std::array<char, 256> buffer;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
	std::tstring result;

	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}

	return result;
}
