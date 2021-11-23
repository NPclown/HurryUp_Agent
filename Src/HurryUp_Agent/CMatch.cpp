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
		default:
			core::Log_Warn(TEXT("CCommunication.cpp - [%s] : %d"), TEXT("Protocol Code Not Exisit"), stPacketInfo->protocol);
		}

		delete stPacketInfo;
	}
}

void CMatch::ReqDeviceInfo()
{
	ST_INFO<ST_DEVICE_INFO> info;
	info.serialNumber = "";
	info.timestamp = GetTimeStamp();
	info.metaInfo = CollectorManager()->DeviceInstance()->getdeviceInfo();

	std::tstring jsInfo;
	core::WriteJsonToString(&info, jsInfo);

	MessageManager()->PushSendMessage(DEVICE, jsInfo);
}

void CMatch::ReqProcessInfo()
{
	ST_INFO<std::vector<ST_PROCESS_INFO>> info;
	info.serialNumber = "";
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
		info.serialNumber = "";
		info.timestamp = GetTimeStamp();
		info.metaInfo = i.second;

		std::tstring jsInfo;
		core::WriteJsonToString(&info, jsInfo);

		MessageManager()->PushSendMessage(FILEDESCRIPTOR, jsInfo);
	}
}
