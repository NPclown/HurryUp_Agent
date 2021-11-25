#pragma once
#include "stdafx.h"

//TODO :: 장비를 정보를 수집하는 클래스
class CDevice
{
private:
	ST_DEVICE_INFO deviceInfo;
	std::vector<ST_PROCESS_INFO> processInfo;
	std::map<std::tstring, std::vector<ST_FD_INFO>> fdInfo;

	//SETTER	
	void setName(std::tstring _name)				{ this->deviceInfo.name = _name; }
	void setOsName(std::tstring _osName)			{ this->deviceInfo.osInfo.osName = _osName; }
	void setOsRelease(std::tstring _osRelease)		{ this->deviceInfo.osInfo.osRelease = _osRelease; }
	void setModelNumber(std::tstring _modelNumber)	{ this->deviceInfo.modelNumber = _modelNumber; }
	void setModuleCount(uint32_t _moduleCount)		{ this->deviceInfo.moduleCount = _moduleCount; }

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
	void collectProcessInfo();
	void collectFdInfo(std::tstring pid);

public:
	CDevice();
	~CDevice();

	//GETTER
	ST_DEVICE_INFO getdeviceInfo(void)										{ return this->deviceInfo; }

	std::tstring getName(void)												{ return this->deviceInfo.name; }
	std::tstring getOsName(void)											{ return this->deviceInfo.osInfo.osName; }
	std::tstring getOsRelease(void)											{ return this->deviceInfo.osInfo.osRelease; }
	std::tstring getModelNumber(void)										{ return this->deviceInfo.modelNumber; }
	uint32_t getModuleCount(void)											{ return this->deviceInfo.moduleCount; }

	std::vector<std::tstring> getConnectionInfo(void)						{ return this->deviceInfo.connectMethod; }
	std::vector<ST_NETWORK_INTERFACE_INFO> getNetworkInfo(void)				{ return this->deviceInfo.networkInfo; }
	std::vector<ST_SERVICE_INFO> getServiceList(void)						{ return this->deviceInfo.serviceList; }

	std::vector<ST_PROCESS_INFO> getProcessList(void)						{ return this->processInfo; }
	std::map<std::tstring, std::vector<ST_FD_INFO>> getFdLists(void)		{ return this->fdInfo; }

	void collectAllData(void);
};

#ifdef _DEBUG
inline void DeviceTest()
{
	CDevice* device = new CDevice();
	device->collectAllData();

	ST_DEVICE_INFO deviceInfo = device->getdeviceInfo();
	std::tstring jsDeviceInfo;

	core::WriteJsonToString(&deviceInfo, jsDeviceInfo);

	std::cout << jsDeviceInfo << std::endl;
}

inline void DeviceProcesTest()
{
	CDevice* device = new CDevice();
	device->collectAllData();

	ST_INFO<std::vector<ST_PROCESS_INFO>> info;
	info.metaInfo = device->getProcessList();
	std::tstring jsMetaInfo;

	core::WriteJsonToString(&info, jsMetaInfo);

	std::cout << jsMetaInfo << std::endl;


	for (auto i : device->getFdLists()) {
		ST_INFO<std::vector<ST_FD_INFO>> _info;
		_info.metaInfo = i.second;

		std::tstring jsMetaInfo;

		core::WriteJsonToString(&_info, jsMetaInfo);

		std::cout << jsMetaInfo << std::endl;
	}
}
#endif // _DEBUG