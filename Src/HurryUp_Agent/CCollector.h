#pragma once
#include "stdafx.h"
#include "CDevice.h"
#include "CModule.h"
#include "CMonitoring.h"

//TODO :: 데이터 수집 클래스 구현
class CCollector
{
private:
	CDevice *device;
	//TODO :: 모듈 관련 클래스 구현 필요
	CMonitoring *monitoring;

	CCollector();
	~CCollector();
public:
	static CCollector* GetInstance(void);

	void init();

	CDevice* DeviceInstance(void);
	CMonitoring* MonitoringInstance(void);
};

inline CCollector* CollectorManager()
{
	return CCollector::GetInstance();
}