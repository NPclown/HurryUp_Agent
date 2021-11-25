#include "CMatch.h"
#include "CMessage.h"
#include "CCollector.h"

CMatch* CMatch::GetInstance(void)
{
    return nullptr;
}

void CMatch::MatchMessage()
{
	core::Log_Debug(TEXT("CMessage.cpp - [%s]"), TEXT("Working MatchReceiveMessage In Thread"));
	ST_PACKET_INFO* stPacketInfo;

	while (1) {
		sleep(0);

		std::future<void> result;

		stPacketInfo = MessageManager()->PopReceiveMessage();

		if (stPacketInfo == NULL)
			continue;

		//TODO :: 메시지 매칭 기능 추후 완성 필요
		//TODO :: 스레드로 대체 필요
		switch (stPacketInfo->protocol) {
		case DEVICE:
			result = std::async(std::launch::async, &CMatch::ReqDeviceInfo, this);
			break;
		case PROCESS:
			result = std::async(std::launch::async, &CMatch::ReqProcessInfo, this);
			result = std::async(std::launch::async, &CMatch::ReqFileDescriptorInfo, this);
			break;
		case MONITORING_REQUEST:
			result = std::async(std::launch::async, &CMatch::ReqMonitoring, this, stPacketInfo->data);
			break;
		case CHANGE_INTERVAL_REQUEST:
			result = std::async(std::launch::async, &CMatch::ReqChangeInterval, this, stPacketInfo->data);
			break;
		default:
			core::Log_Warn(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Protocol Code Not Exisit"), stPacketInfo->protocol);
		}

		delete stPacketInfo;
	}
}

void CMatch::ReqDeviceInfo()
{
	ST_INFO<ST_DEVICE_INFO> info;
	info.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
	info.timestamp = GetTimeStamp();
	info.metaInfo = CollectorManager()->DeviceInstance()->getdeviceInfo();

	std::tstring jsInfo;
	core::WriteJsonToString(&info, jsInfo);

	MessageManager()->PushSendMessage(DEVICE, jsInfo);
}

void CMatch::ReqProcessInfo()
{
	ST_INFO<std::vector<ST_PROCESS_INFO>> info;
	info.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
	info.timestamp = GetTimeStamp();
	info.metaInfo = CollectorManager()->DeviceInstance()->getProcessList();

	std::tstring jsInfo;
	core::WriteJsonToString(&info, jsInfo);

	MessageManager()->PushSendMessage(PROCESS, jsInfo);
}

void CMatch::ReqFileDescriptorInfo()
{
	for (auto i : CollectorManager()->DeviceInstance()->getFdLists()) {
		ST_INFO<std::vector<ST_FD_INFO>> info;
		info.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
		info.timestamp = GetTimeStamp();
		info.metaInfo = i.second;

		std::tstring jsInfo;
		core::WriteJsonToString(&info, jsInfo);

		MessageManager()->PushSendMessage(FILEDESCRIPTOR, jsInfo);
	}
}

void CMatch::ReqMonitoring(std::tstring data)
{
	ST_MONITOR_REQUEST info;

	core::ReadJsonFromString(&info, data);

	int result;
	if (info.activate)
		result = CollectorManager()->MonitoringInstance()->AddMonitoringTarget(info.processName, info.logPath);
	else
		result = CollectorManager()->MonitoringInstance()->RemoveMonitoringTarget(info.processName, info.logPath);

	ST_RESPONSE_INFO<ST_MONITOR_REQUEST> message;
	message.requestProtocol = MONITORING_REQUEST;
	message.requestInfo = info;
	if (info.activate)
		message.result = result == 0 ? true : false;
	else
		message.result = result == 0 ? false : true;
	message.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
	message.timestamp = GetTimeStamp();

	std::tstring jsMessage;
	core::WriteJsonToString(&info, jsMessage);

	MessageManager()->PushSendMessage(RESPONSE, jsMessage);
}

void CMatch::ReqChangeInterval(std::tstring data)
{
}
