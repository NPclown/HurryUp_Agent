#pragma once
#include "stdafx.h"
#include <sys/inotify.h>
#include <fstream>

///TODO :: TCP 통신 싱글톤 클래스 구현
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024*(EVENT_SIZE+16))

struct ST_MONITORING_EVENT
{
	std::tstring processName;
	std::tstring orignalPath;
	std::tstring directoryPath;
	std::tstring fileName;
	std::ifstream fd;
	int wd;
	long long int size;
};

class CMonitoring
{
private:
	int fd;
	bool terminate;
	std::map<std::tstring, struct ST_MONITORING_EVENT*> monitoringLists;
	std::map<std::tstring, int> wdCountLists;
	std::map<int, std::tstring> wdKeyPathLists;
	std::mutex monitoringMutex;
public:
	CMonitoring();
	~CMonitoring();
	int AddMonitoringTarget(std::tstring processName, std::tstring logPath);
	int RemoveMonitoringTarget(std::tstring processName, std::tstring logPath);
	void StartMonitoring();
	void EndMonitoring();
};