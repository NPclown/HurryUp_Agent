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
	int size;
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

	CMonitoring();
	~CMonitoring();
public:
	static CMonitoring* GetInstance(void);
	int AddMonitoringTarget(ST_MONITOR_TARGET target);
	int RemoveMonitoringTarget(ST_MONITOR_TARGET target);
	void StartMonitoring();
	void EndMonitoring();
};

inline CMonitoring* MonitoringManager()
{
	return CMonitoring::GetInstance();
}