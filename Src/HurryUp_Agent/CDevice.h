#pragma once
#include "stdafx.h"

//TODO :: 장비를 정보를 수집하는 클래스
class CDevice
{
private:
	ST_DEVICE_INFO metaInfo;

	//SETTER	
	void setName(std::tstring _name)				{ this->metaInfo.name = _name; }
	void setOsName(std::tstring _osName)			{ this->metaInfo.osInfo.osName = _osName; }
	void setOsRelease(std::tstring _osRelease)		{ this->metaInfo.osInfo.osRelease = _osRelease; }
	void setModelNumber(std::tstring _modelNumber)	{ this->metaInfo.modelNumber = _modelNumber; }
	void setModuleCount(uint32_t _moduleCount)		{ this->metaInfo.moduleCount = _moduleCount; }

	void addNetworkInfo(ST_NETWORK_INTERFACE_INFO& _networkInfo);
	void addServiceInfo(ST_SERVICE_INFO& _serviceInfo);
	void addConnectInfo(std::tstring& _connectInfo);

	//FUNCTION
	void collectNameInfo();
	void collectModelName();
	void collectNetworkInfo();
	void collectOsInfo();
	void collectCpuInfo();
	void collectServiceInfo();
public:
	CDevice();
	~CDevice();

	//GETTER
	ST_DEVICE_INFO getMetaInfo(void)							{ return this->metaInfo; }

	std::tstring getName(void)									{ return this->metaInfo.name; }
	std::tstring getOsName(void)								{ return this->metaInfo.osInfo.osName; }
	std::tstring getOsRelease(void)								{ return this->metaInfo.osInfo.osRelease; }
	std::tstring getModelNumber(void)							{ return this->metaInfo.modelNumber; }
	uint32_t getModuleCount(void)								{ return this->metaInfo.moduleCount; }

	std::vector<std::tstring> getConnectionInfo(void)			{ return this->metaInfo.connectMethod; }
	std::vector<ST_NETWORK_INTERFACE_INFO> getNetworkInfo(void) { return this->metaInfo.networkInfo; }
	std::vector<ST_SERVICE_INFO> getServiceList(void)			{ return this->metaInfo.serviceList; }

	void collectAllData(void);

	//TODO :: 프로세스 목록 수집
	//TODO :: 파일 디스크립터 목록 수집
};

#ifdef _DEBUG
inline void DeviceTest()
{
	CDevice* device = new CDevice();
	device->collectAllData();

	ST_DEVICE_INFO deviceInfo = device->getMetaInfo();
	std::tstring jsDeviceInfo;

	core::WriteJsonToString(&deviceInfo, jsDeviceInfo);

	std::cout << jsDeviceInfo << std::endl;
}
#endif // _DEBUG