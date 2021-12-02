#include "CMonitoring.h"
#include "CMessage.h"

CMonitoring::CMonitoring()
{
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s]"), TEXT("Init"));

	this->fd = inotify_init();

	if (this->fd == -1)
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %d"), TEXT("Inotify Init Error"), errno);

	this->terminate = false;
}

CMonitoring::~CMonitoring()
{
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s]"), TEXT("End"));

	(void)close(fd);
}

int CMonitoring::AddMonitoringTarget(std::tstring processName, std::tstring logPath)
{
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("MonitoringTarget Add Start"), TEXT(logPath.c_str()));

	sleep(0);
	std::lock_guard<std::mutex> lock_guard(monitoringMutex);
	std::tstring originalPath = logPath;

	if (!core::PathFileExistsA(originalPath.c_str())) {
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("File Not Exists"), TEXT(originalPath.c_str()));
		return -1;
	}

	std::tstring directoryPath = core::ExtractDirectory(originalPath);
	std::tstring fileName = core::ExtractFileName(originalPath);

	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("GetDirectoryPath Result"), TEXT(directoryPath.c_str()));
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("GetFilename Result"), TEXT(fileName.c_str()));


	if (directoryPath == "" || fileName == "")
	{
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Path Not Valid."), TEXT(originalPath.c_str()));
		return -1;
	}

	int wd = wdCountLists.count(directoryPath) ? wdCountLists[directoryPath] : -1;
	ST_MONITORING_EVENT* monitoringEvent = monitoringLists.count(originalPath) ? monitoringLists[originalPath] : NULL;

	if (wd != -1 && monitoringEvent == NULL)
	{
		core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Directory Path Already Being Watched."), TEXT(directoryPath.c_str()));
		wdCountLists[directoryPath]++;
	}

	if (wd == -1)
	{
		wd = inotify_add_watch(fd, directoryPath.c_str(), IN_MODIFY | IN_MOVE);
		if (wd == -1)
		{
			core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %d"), TEXT("Watching Add/Update Error Code"), errno);
			return -1;
		}
		wdCountLists.insert(std::pair<std::tstring, int>(TEXT(directoryPath), 1));
		wdKeyPathLists.insert(std::pair<int, std::tstring>(wd, TEXT(directoryPath)));
		core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Directory Path Start New Watch."), TEXT(directoryPath.c_str()));
	}

	if (monitoringEvent == NULL)
	{
		ST_MONITORING_EVENT* monitoringEvent = new ST_MONITORING_EVENT();
		monitoringEvent->processName = processName;
		monitoringEvent->orignalPath = originalPath;
		monitoringEvent->directoryPath = directoryPath;
		monitoringEvent->fileName = fileName;
		monitoringEvent->fd = std::ifstream(originalPath);
		monitoringEvent->wd = wd;
		monitoringEvent->size = FindFileEndPosition(monitoringEvent->fd);

		monitoringLists.insert(std::pair<std::tstring, struct ST_MONITORING_EVENT*>(TEXT(originalPath), monitoringEvent));
	}

	core::Log_Info(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("MonitoringTarget Add Complete"), TEXT(logPath.c_str()));
	return 0;
}

int CMonitoring::RemoveMonitoringTarget(std::tstring processName, std::tstring logPath)
{
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("MonitoringTarget Remove Start"), TEXT(logPath.c_str()));
	sleep(0);
	std::lock_guard<std::mutex> lock_guard(monitoringMutex);

	std::tstring originalPath = logPath;

	if (!core::PathFileExistsA(originalPath.c_str())) {
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("File Not Exists"), TEXT(originalPath.c_str()));
		return -1;
	}

	std::tstring directoryPath = core::ExtractDirectory(originalPath);
	std::tstring fileName = core::ExtractFileName(originalPath);

	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("GetDirectoryPath Result"), TEXT(directoryPath.c_str()));
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("GetFilename Result"), TEXT(fileName.c_str()));

	if (directoryPath == "" || fileName == "")
	{
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Path Not Valid."), TEXT(originalPath.c_str()));
		return -1;
	}

	ST_MONITORING_EVENT* monitoringEvent = monitoringLists.count(originalPath) ? monitoringLists[originalPath] : NULL;

	if (monitoringEvent != NULL)
	{
		monitoringEvent->fd.close();

		if (wdCountLists[monitoringEvent->directoryPath] > 1)
		{
			core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Remove Target From The List."), TEXT(originalPath.c_str()));
			wdCountLists[monitoringEvent->directoryPath]--;
		}
		else if (wdCountLists[monitoringEvent->directoryPath] == 1)
		{
			core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Stop Directory Path Monitoring"), TEXT(monitoringEvent->directoryPath.c_str()));
			int result = inotify_rm_watch(fd, monitoringEvent->wd);

			if (result == -1)
			{
				core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %d"), TEXT("Watching Remove Error Code"), errno);
				return -1;
			}

			wdCountLists.erase(monitoringEvent->directoryPath);
			wdKeyPathLists.erase(monitoringEvent->wd);
		}

		free(monitoringEvent);
		monitoringLists.erase(originalPath);
	}
	else
	{
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Target Does Not Exist In The List"), TEXT(originalPath.c_str()));
		return -1;
	}

	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("MonitoringTarget Remove Complete"), TEXT(logPath.c_str()));
	return 0;
}

void CMonitoring::StartMonitoring()
{
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s]"), TEXT("Working Monitoring In Thread"));

	char buffer[BUF_LEN];
	while (!terminate) {
		sleep(0);

		int length = read(fd, buffer, BUF_LEN);
		int i = 0;

		if (length < 0) {
			core::Log_Warn(TEXT("CMonitoring.cpp - [%s]"), TEXT("Monitoring Error"));
		}

		while (i < length) {
			struct inotify_event* event = (struct inotify_event*)&buffer[i];

			if (event->len)
			{
				if (event->mask & IN_MOVE)
				{
					if (!(event->mask & IN_ISDIR))
					{
						// vi, nano editor가 저장과 동시 종료시에는 수정된 내용이 반영되지만
						// 파일을 열어놓은 상태에서 저장하고, 나중에 종료하면 연결이 끊어진다. (조치 필요)

						std::tstring directoryPath = TEXT(wdKeyPathLists[event->wd]);
						std::tstring fullPath = TEXT(directoryPath) + TEXT("/") + TEXT(event->name);

						ST_MONITORING_EVENT* monitoringEvent = monitoringLists.count(fullPath) ? monitoringLists[fullPath] : NULL;

						if (monitoringEvent != NULL) {
							core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("File Descriptor Update"), TEXT(fullPath.c_str()));
							monitoringEvent->fd.close();
							monitoringEvent->fd = std::ifstream(monitoringEvent->orignalPath);
						}
					}
				}

				if (event->mask & IN_MODIFY)
				{
					if (!(event->mask & IN_ISDIR))
					{
						std::tstring directoryPath = TEXT(wdKeyPathLists[event->wd]);
						std::tstring fullPath = TEXT(directoryPath) + TEXT("/") + TEXT(event->name);
						std::tstring message;

						ST_MONITORING_EVENT* monitoringEvent = monitoringLists.count(fullPath) ? monitoringLists[fullPath] : NULL;

						if (monitoringEvent != NULL) {
							long long int re_size = monitoringEvent->size;
							monitoringEvent->fd.seekg(0, std::ios::end);

							long long int size = monitoringEvent->fd.tellg();
							message.resize(size - re_size);

							monitoringEvent->fd.seekg(re_size);
							monitoringEvent->size = size;
							monitoringEvent->fd.read(&message[0], size - re_size);
							core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s, %d -> %d"), TEXT("File Size"), TEXT(fullPath.c_str()), re_size, size);
							core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %s, %s"), TEXT("FileModify Content"), TEXT(fullPath.c_str()), TEXT(message.c_str()));
							
							ST_INFO<ST_MONITOR_INFO> stMonitoringInfo;
							stMonitoringInfo.serialNumber = EnvironmentManager()->GetSerialNumber();
							stMonitoringInfo.timestamp = GetTimeStamp();
							stMonitoringInfo.metaInfo.environment = EnvironmentManager()->GetEnvironment();
							stMonitoringInfo.metaInfo.logData.processName = monitoringEvent->processName;
							stMonitoringInfo.metaInfo.logData.logPath = monitoringEvent->orignalPath;
							stMonitoringInfo.metaInfo.logData.changeData = message;

							std::tstring jsMoniotringInfo;
							core::WriteJsonToString(&stMonitoringInfo, jsMoniotringInfo);
							MessageManager()->PushSendMessage(MONITORING_LOG, jsMoniotringInfo);
						}
					}
				}
			}

			i += EVENT_SIZE + event->len;
		}
	}
}

void CMonitoring::EndMonitoring()
{
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s]"), TEXT("Termiate Monitoring In Thread"));
	terminate = true;
}
