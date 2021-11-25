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

void CCollector::SetInterval()
{
    int count = 0;
    while (1)
    {
        {
            std::lock_guard<std::mutex> lock_guard(this->timeMutex);
            sleep(time);
        }
        core::Log_Info(TEXT("CCollector.cpp - [%s]"), TEXT("SetInterval"));

        ST_INFO<ST_DEVICE_INFO> deviceInfo;
        deviceInfo.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
        deviceInfo.timestamp = GetTimeStamp();
        deviceInfo.metaInfo = CollectorManager()->DeviceInstance()->getdeviceInfo();

        std::tstring jsDeviceInfo;
        core::WriteJsonToString(&deviceInfo, jsDeviceInfo);

        MessageManager()->PushSendMessage(DEVICE, jsDeviceInfo);

        ST_INFO<std::vector<ST_PROCESS_INFO>> processInfo;
        processInfo.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
        processInfo.timestamp = GetTimeStamp();
        processInfo.metaInfo = CollectorManager()->DeviceInstance()->getProcessList();

        std::tstring jsProcessInfo;
        core::WriteJsonToString(&processInfo, jsProcessInfo);

        MessageManager()->PushSendMessage(PROCESS, jsProcessInfo);

        for (auto i : CollectorManager()->DeviceInstance()->getFdLists()) {
            ST_INFO<std::vector<ST_FD_INFO>> fdInfo;
            fdInfo.serialNumber = CollectorManager()->DeviceInstance()->getSerialNumber();
            fdInfo.timestamp = GetTimeStamp();
            fdInfo.metaInfo = i.second;

            std::tstring jsFdInfo;
            core::WriteJsonToString(&fdInfo, jsFdInfo);

            MessageManager()->PushSendMessage(FILEDESCRIPTOR, jsFdInfo);
        }

        count++;

        if (count % 3 == 0) {
            this->DeviceInstance()->collectAllData();
            count = 0;
        }
    }
}

CCollector* CCollector::GetInstance(void)
{
    static CCollector instance;
    return &instance;
}

void CCollector::init()
{
    setTime(30);
    std::future<void> deviceInfo = std::async(std::launch::async, &CDevice::collectAllData, this->device);
    std::future<void> monitoringStart = std::async(std::launch::async, &CMonitoring::StartMonitoring, this->monitoring);
    std::future<void> setInterval = std::async(std::launch::async, &CCollector::SetInterval, this);
}

void CCollector::setTime(int _time)
{
    std::lock_guard<std::mutex> lock_guard(this->timeMutex);
    this->time = _time;
}

CDevice* CCollector::DeviceInstance(void)
{
    return this->device;
}

CMonitoring* CCollector::MonitoringInstance(void)
{
    return this->monitoring;
}
