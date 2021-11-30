#include "CExecutor.h"
#include <arpa/inet.h>

#define TMP_PATH TEXT("/tmp/hurryup/")
#define TMP_POLICY_PATH TEXT("/tmp/hurryup/")

CExecutor::CExecutor()
{
	CheckDirectory(TMP_PATH);
	CheckDirectory(TMP_POLICY_PATH);
}

CExecutor::~CExecutor()
{
}

int CExecutor::Connect(char* ip, int port)
{
	struct sockaddr_in servaddr;

	if ((this->fileSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		core::Log_Warn(TEXT("CExecutor.cpp - [%s] : %d"), TEXT("Socket Create Fail"), errno);
		return -1;
	}

	bzero((char*)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);

	if (connect(this->fileSocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		core::Log_Warn(TEXT("CExecutor.cpp - [%s] : %d"), TEXT("Connect Fail"), errno);
		return -1;
	}

	return 0;
}

void CExecutor::Disconnect()
{
	close(this->fileSocket);
}

bool CExecutor::ChangeGrant()
{
	if (this->savePath == "")
		return false;

	std::tstring result = Exec(CHMOD_COMMAND, this->savePath.c_str());

	if (Split(result, "\n")[0] != std::tstring("0")) {
		core::Log_Error(TEXT("CService.cpp - [%s] : %s"), TEXT("Exec Command Error."), TEXT(result.c_str()));
		return false;
	}

	return true;
}

void CExecutor::Init(std::tstring _fileName)
{
	this->fileName = _fileName + TAR_EXT;
	this->savePath = "";
}

bool CExecutor::DownloadFile()
{
	size_t fileSize;
	FILE* file;

	//TODO :: 환경변수 클래스로 처리
	// 파일서버와 연결
	if (Connect("192.168.181.134", 2031) != 0) {
		return false;
	}

	// 다운 받을 정책 정보 전송
	send(this->fileSocket, this->fileName.c_str(), 511, 0);

	this->savePath = TMP_POLICY_PATH + GeneratorStringNumber() +"/";
	CheckDirectory(this->savePath);
	
	std::tstring filePath = this->savePath + this->fileName;
	file = fopen(filePath.c_str(), "wb");

	int nbyte = BUFFER_SIZE;
	char buffer[BUFFER_SIZE];

	while (nbyte) {
		nbyte = recv(this->fileSocket, buffer, BUFFER_SIZE, 0);
		fwrite(buffer, sizeof(char), nbyte, file);
	}

	fclose(file);
	Disconnect();

	return true;
}

bool CExecutor::ExtractFile()
{
	if (this->savePath == "")
		return false;

	std::tstring tarPath = this->savePath + this->fileName;
	std::tstring result = Exec(TAR_COMMAND, tarPath.c_str(), this->savePath.c_str());

	if (Split(result, "\n")[0] != std::tstring("0")) {
		core::Log_Error(TEXT("CService.cpp - [%s] : %s"), TEXT("Exec Command Error."), TEXT(result.c_str()));
		return false;
	}

	// 파일 권한 변경
	if (!ChangeGrant()) {
		return false;
	}

	return true;
}

bool CExecutor::RemoveFile()
{
	if (this->savePath == "")
		return false;

	std::tstring result = Exec(RM_COMMAND, this->savePath.c_str());

	if (Split(result, "\n")[0] != std::tstring("0")) {
		core::Log_Error(TEXT("CService.cpp - [%s] : %s"), TEXT("Exec Command Error."), TEXT(result.c_str()));
		return false;
	}

	return true;
}


