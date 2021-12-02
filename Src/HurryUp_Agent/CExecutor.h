#pragma once
#include "stdafx.h"

//TODO :: 실행 관련을 관리하는 클래스
class CExecutor
{
private:
	int fileSocket;

	void Disconnect();
	int Connect(const char* ip, int port);
	bool ChangeGrant();
public:
	std::tstring fileName;
	std::tstring savePath;

	CExecutor();
	~CExecutor();

	void Init(std::tstring _fileName);
	bool DownloadFile();
	bool ExtractFile();
	bool RemoveFile();
	virtual int Execute() = 0;
};

