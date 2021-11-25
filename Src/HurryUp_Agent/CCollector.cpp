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
    while (1)
    {
        sleep(time);
        ST_INFO<ST_DEVICE_INFO> info;
        info.serialNumber = "";
        info.timestamp = GetTimeStamp();
        info.metaInfo = CollectorManager()->DeviceInstance()->getdeviceInfo();

        std::tstring jsInfo;
        core::WriteJsonToString(&info, jsInfo);

        MessageManager()->PushSendMessage(DEVICE, jsInfo);
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
    std::future<void> setInterval = std::async(std::launch::async, &CCollector::SetInterval, this, 5);
}

CDevice* CCollector::DeviceInstance(void)
{
    return this->device;
}

CMonitoring* CCollector::MonitoringInstance(void)
{
    return this->monitoring;
}
