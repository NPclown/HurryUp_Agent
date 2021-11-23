#include "CCollector.h"

CCollector::CCollector()
{
    this->device = new CDevice();
    this->monitoring = new CMonitoring();
}

CCollector::~CCollector()
{
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
}

CDevice* CCollector::DeviceInstance(void)
{
    return this->device;
}

CMonitoring* CCollector::MonitoringInstance(void)
{
    return this->monitoring;
}
