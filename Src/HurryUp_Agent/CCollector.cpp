#include "CCollector.h"
#include "CMessage.h"

CCollector::CCollector()
{
    this->device = new CDevice();
    this->monitoring = new CMonitoring();
}

CCollector::~CCollector()
{
}

void CCollector::SetInterval(int time)
{
    int count = 0;
    while (1)
    {
        sleep(time);
        ST_INFO<ST_DEVICE_INFO> deviceInfo;
        deviceInfo.serialNumber = "";
        deviceInfo.timestamp = GetTimeStamp();
        deviceInfo.metaInfo = CollectorManager()->DeviceInstance()->getdeviceInfo();

        std::tstring jsDeviceInfo;
        core::WriteJsonToString(&deviceInfo, jsDeviceInfo);

        MessageManager()->PushSendMessage(DEVICE, jsDeviceInfo);

        if (count % time == 0) {
            ST_INFO<std::vector<ST_PROCESS_INFO>> processInfo;
            processInfo.serialNumber = "";
            processInfo.timestamp = GetTimeStamp();
            processInfo.metaInfo = CollectorManager()->DeviceInstance()->getProcessList();

            std::tstring jsProcessInfo;
            core::WriteJsonToString(&processInfo, jsProcessInfo);

            MessageManager()->PushSendMessage(PROCESS, jsProcessInfo);

            for (auto i : CollectorManager()->DeviceInstance()->getFdLists()) {
                ST_INFO<std::vector<ST_FD_INFO>> fdInfo;
                fdInfo.serialNumber = "";
                fdInfo.timestamp = GetTimeStamp();
                fdInfo.metaInfo = i.second;

                std::tstring jsFdInfo;
                core::WriteJsonToString(&fdInfo, jsFdInfo);

                MessageManager()->PushSendMessage(FILEDESCRIPTOR, jsFdInfo);
            }

            count = 0;
        }

        count++;
    }
}

CCollector* CCollector::GetInstance(void)
{
    static CCollector instance;
    return &instance;
}

void CCollector::init()
{
    std::future<void> deviceInfo = std::async(std::launch::async, &CDevice::collectAllData, this->device);
    std::future<void> monitoringStart = std::async(std::launch::async, &CMonitoring::StartMonitoring, this->monitoring);
    std::future<void> setInterval = std::async(std::launch::async, &CCollector::SetInterval, this, 30);
}

CDevice* CCollector::DeviceInstance(void)
{
    return this->device;
}

CMonitoring* CCollector::MonitoringInstance(void)
{
    return this->monitoring;
}
