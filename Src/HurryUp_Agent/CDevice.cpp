#include "CDevice.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <dirent.h>

void CDevice::addNetworkInfo(ST_NETWORK_INTERFACE_INFO& _networkInfo)
{
	for (auto it : this->deviceInfo.networkInfo)
		if (it == _networkInfo) return;

	this->deviceInfo.networkInfo.push_back(_networkInfo);
}

void CDevice::addServiceInfo(ST_SERVICE_INFO& _serviceInfo)
{
	for (auto it : this->deviceInfo.serviceList)
		if (it == _serviceInfo) return;

	this->deviceInfo.serviceList.push_back(_serviceInfo);
}

void CDevice::addConnectInfo(std::tstring& _connectInfo)
{
	for (auto it : this->deviceInfo.connectMethod)
		if (it == _connectInfo) return;

	this->deviceInfo.connectMethod.push_back(_connectInfo);
}

void CDevice::collectNameInfo()
{
	//TODO :: HOSTNAME 확인하는 방법
	// 1. hostname 명령어
	// 2. 파일로 확인 
	// redHat 계열 -> /etc/sysconfig/network | grep HOSTNAME
	// ubuntu -> /etc/hostname
	// /proc/sys/kernel/hostname
	std::tstring deviceName = ReadContent("/proc/sys/kernel/hostname");
	this->setName(deviceName);
}

void CDevice::collectModelName()
{
	std::tstring modelName_raw = Exec(MODLENAME_COMMAND);
	std::tstring modelName;
	core::Split(modelName_raw, ":", &modelName);

	this->setmodelName(core::Trim(modelName, " \n\r"));
}

void CDevice::collectNetworkInfo()
{
	// Connection Method => pinout 커맨드 이용
		// 일단은 임시로 Wifi + Bluetooth만 조사 => 추후 추가예정
	//std::tstring temp = exec("sudo pinout | grep -e \"Wi - fi\" -e \"Bluetooth\" | awk \'{print $1}\'");
	//std::cout << ":: size :: " << temp.length() << '\n';

	//std::vector<std::tstring> methods = split(temp, '\n');

	//this->deviceInfo.connectMethod = methods;

	// API version => 함수 새로 구현
	// Command version => ifconfig, ip addr 관련 => 기존 함수 사용
	// File version	 => /sys/class/net 사용

	std::vector < ST_NETWORK_INTERFACE_INFO > result;

	// ifaddr 사용
	struct ifaddrs* ifaddr;
	int family, s;
	char host[NI_MAXHOST];

	// 에러 핸들링
	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	std::map<std::string, ST_NETWORK_INTERFACE_INFO*> checker;
	// 링크드 리스트 형식으로 되어있음. 하위 내용은 일단 출력 형식으로 구현.
	for (struct ifaddrs* ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;

		std::string temp = ifa->ifa_name;

		if (checker.find(temp) == checker.end())
		{
			auto nInfo = new ST_NETWORK_INTERFACE_INFO;
			nInfo->if_name = temp;

			checker[temp] = nInfo;
		}

		auto nowNetInfo = checker[temp];


		family = ifa->ifa_addr->sa_family;


		if (family == AF_INET || family == AF_INET6) {
			s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET) ? sizeof(struct sockaddr_in) :
				sizeof(struct sockaddr_in6),
				host, NI_MAXHOST,
				NULL, 0, NI_NUMERICHOST);

			if (s != 0) {
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
				exit(EXIT_FAILURE);
			}

			std::string hostTemp = host;
			if (family == AF_INET)	nowNetInfo->inet_addr = hostTemp;
			if (family == AF_INET6) nowNetInfo->inet6_addr = hostTemp;

			//printf("\t\taddress: <%s>\n", host);

		}
		else if (family == AF_PACKET && ifa->ifa_data != NULL) {
			auto s = (sockaddr_ll*)ifa->ifa_addr;
			char mac[] = "00:13:a9:1f:b0:88";

			auto a = s->sll_addr;

			sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);

			std::string macAddr = mac;
			nowNetInfo->m_addr = macAddr;

		}
	}

	freeifaddrs(ifaddr);

	this->deviceInfo.networkInfo.clear();
	for (auto it : checker)
	{
		ST_NETWORK_INTERFACE_INFO temp;
		temp.if_name = it.second->if_name;
		temp.m_addr = it.second->m_addr;
		temp.inet_addr = it.second->inet_addr;
		temp.inet6_addr = it.second->inet6_addr;

		this->deviceInfo.networkInfo.push_back(temp);
	}
}

void CDevice::collectOsInfo()
{
	// API version => <sys/utsname.h>
		// Command => lsb_release -a
		// File version => cat /etc/os-release 혹은 cat /proc/version

		// osName, release가 포함됨.

	ST_OS_INFO _osInfo;


	std::string osName_raw = Exec(OSNAME_COMMAND);
	std::string osName;

	core::Split(osName_raw, "=", &osName);

	std::string osRelease_raw = Exec(OSVERSION_COMMAND);
	std::string osRelease;

	core::Split(osRelease_raw, "=", &osRelease);

	_osInfo.osName = core::Trim(osName.c_str(), " \"\n\r");
	_osInfo.osRelease = core::Trim(osRelease.c_str(), " \"\n\r");

	this->deviceInfo.osInfo = _osInfo;
}

void CDevice::collectCpuInfo()
{
	// Command version => lscpu 혹은 sudo dmidecode --type processor
// File version => cat /proc/cpuinfo

// arch, core 수, vendor ID
}

void CDevice::collectServiceInfo()
{
	core::Log_Debug(TEXT("CDevice.cpp - [%s]"), TEXT("Get ServiceInfo Start"));

	// serviceName, isActive 반환

	std::string serviceList_raw = Exec(SERVICE_COMMAND);
	std::string serviceList = std::regex_replace(serviceList_raw, std::regex(" \\[ "), "");
	serviceList = std::regex_replace(serviceList, std::regex(" \\]  "), "");
	std::vector<std::string> temp = Split(serviceList, "\n");

	this->deviceInfo.serviceList.clear();
	for (auto it : temp)
	{
		if (it == "")
			continue;
		ST_SERVICE_INFO _sInfo;
		char initialChar = it[0];

		_sInfo.serviceName = it.substr(1);
		_sInfo.isActive = false;

		if (initialChar == '+') _sInfo.isActive = true;

		this->deviceInfo.serviceList.push_back(_sInfo);
	}

	core::Log_Debug(TEXT("CDevice.cpp - [%s]"), TEXT("Get ServiceInfo End"));
}

CDevice::CDevice()
{
	this->deviceInfo.name = "";
	this->deviceInfo.osInfo.osName = "";
	this->deviceInfo.osInfo.osRelease = "";
	this->deviceInfo.modelName = "";
	this->deviceInfo.moduleCount = 0;
	this->deviceInfo.networkInfo.clear();
	this->deviceInfo.serviceList.clear();
	this->deviceInfo.connectMethod.clear();
	this->processInfo.clear();
	this->fdInfo.clear();
}

CDevice::~CDevice()
{
}

void CDevice::collectAllData(void)
{
	core::Log_Debug(TEXT("CDevice.cpp - [%s]"), TEXT("collectAllData"));
	this->collectNameInfo();
	this->collectModelName();
	this->collectNetworkInfo();
	this->collectOsInfo();
	this->collectCpuInfo();
	this->collectProcessInfo();
	this->collectServiceInfo();
}


void CDevice::collectProcessInfo()
{
	core::Log_Debug(TEXT("CDevice.cpp - [%s]"), TEXT("Get ProcessList Start"));

	std::tstring path = TEXT("/proc");

	int i = 0;

	//TODO :: cppcore 파일시스템을 뒤져보는 함수로 대체 가능

	DIR* dir = opendir(path.c_str());
	if (dir == NULL)
	{
		core::Log_Debug(TEXT("CMonitoring.cpp - [%s]"), TEXT("Directory Open Fail"));
		return ;
	}

	this->processInfo.clear();
	this->fdInfo.clear();

	struct dirent* de = NULL;

	while ((de = readdir(dir)) != NULL)
	{
		if (strtol(de->d_name, NULL, 10) > 0) {
			ST_PROCESS_INFO pinfo;
			std::tstring next;

			std::tstring path("/proc/" + TEXT(std::string(de->d_name)));
			std::ifstream status(path + "/status");
			std::tstring buffer;

			while (buffer.find("Name:") == std::tstring::npos)
				std::getline(status, buffer);
			core::Split(buffer, ":", &next);
			pinfo.name = core::Trim(next.c_str(), " \t");

			while (buffer.find("State:") == std::tstring::npos)
				std::getline(status, buffer);
			core::Split(buffer, ":", &next);
			pinfo.state = core::Trim(next.c_str(), " \t");

			while (buffer.find("Pid:") == std::tstring::npos)
				std::getline(status, buffer);
			core::Split(buffer, ":", &next);
			pinfo.pid = strtol(core::Trim(next.c_str(), " \t").c_str(), NULL, 10);

			while (buffer.find("PPid:") == std::tstring::npos)
				std::getline(status, buffer);
			core::Split(buffer, ":", &next);
			pinfo.ppid = strtol(core::Trim(next.c_str(), " \t").c_str(), NULL, 10);

			status.close();

			std::ifstream cmdLine(path + "/cmdline");
			std::getline(cmdLine, buffer);
			pinfo.cmdline = core::Trim(buffer.c_str(), " \t").c_str();
			cmdLine.close();

			std::ifstream timeInfo(path + "/sched");

			while (buffer.find("se.exec_start") == std::tstring::npos)
				std::getline(timeInfo, buffer);
			timeInfo.close();

			core::Split(buffer, ":", &next);
			time_t curr_time = strtol(core::Trim(next.c_str(), " \t").c_str(), NULL, 10);
			pinfo.startTime = std::asctime(std::localtime(&curr_time));

			i++;
			if (collectFdInfo(de->d_name) != 0)
				this->processInfo.push_back(pinfo);
		}
	}

	closedir(dir);
	core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %d"), TEXT("Get ProcessList End"), i);
}

int CDevice::collectFdInfo(std::tstring pid)
{
	core::Log_Debug(TEXT("CDevice.cpp - [%s]"), TEXT("Get ProcessFileDescriptorList Start"));

	std::tstring path = TEXT("/proc/") + TEXT(pid) + TEXT("/fd");
	std::vector<ST_FD_INFO> fdLists;

	if (!core::PathFileExistsA(path.c_str())) {
		core::Log_Warn(TEXT("CMonitoring.cpp - [%s] : %s"), TEXT("Process Is Not Valid."), TEXT(path.c_str()));
		return -1;
	}

	int i = 0;
	DIR* dir = opendir(path.c_str());

	if (dir == NULL)
	{
		core::Log_Debug(TEXT("CMonitoring.cpp - [%s]"), TEXT("Directory Open Fail"));
		return -1;
	}

	struct dirent* de = NULL;

	//TODO :: cppcore 파일시스템을 뒤져보는 함수로 대체 가능

	while ((de = readdir(dir)) != NULL)
	{
		if (strtol(de->d_name, NULL, 10) > 0) {
			char buf[1024];
			ST_FD_INFO pinfo;
			std::tstring linkPath = TEXT(path) + TEXT("/") + TEXT(de->d_name);

			std::regex re(".*(\\.log)$");
			int length = readlink(linkPath.c_str(), buf, sizeof(buf));
			buf[length] = '\0';

			if (std::regex_match(buf, re)) {
				pinfo.pid = strtol(pid.c_str(), NULL, 10);
				pinfo.fdName = de->d_name;
				pinfo.realPath = core::MakeFormalPath(buf);

				fdLists.push_back(pinfo);
				i++;
			}
		}
	}

	if (i != 0) {
		this->fdInfo.insert({ pid, fdLists });
		core::Log_Debug(TEXT("CMonitoring.cpp - [%s] : %d"), TEXT("Get ProcessFileDescriptorList End"), i);
	}

	return i;
}
