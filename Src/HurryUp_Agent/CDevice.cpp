#include "CDevice.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void CDevice::addNetworkInfo(ST_NETWORK_INTERFACE_INFO& _networkInfo)
{
	for (auto it : this->metaInfo.networkInfo)
		if (it == _networkInfo) return;

	this->metaInfo.networkInfo.push_back(_networkInfo);
}

void CDevice::addServiceInfo(ST_SERVICE_INFO& _serviceInfo)
{
	for (auto it : this->metaInfo.serviceList)
		if (it == _serviceInfo) return;

	this->metaInfo.serviceList.push_back(_serviceInfo);
}

void CDevice::addConnectInfo(std::tstring& _connectInfo)
{
	for (auto it : this->metaInfo.connectMethod)
		if (it == _connectInfo) return;

	this->metaInfo.connectMethod.push_back(_connectInfo);
}

void CDevice::collectNameInfo()
{
	//TODO :: HOSTNAME 확인하는 방법
	// 1. hostname 명령어
	// 2. 파일로 확인 
	// redHat 계열 -> /etc/sysconfig/network | grep HOSTNAME
	// ubuntu -> /etc/hostname
	// /proc/sys/kernel/hostname
	//std::string deviceName = exec("hostname");
	std::tstring deviceName = ReadContent("/proc/sys/kernel/hostname");
	this->setName(deviceName);
}

void CDevice::collectModelName()
{
	std::tstring modelName_raw = exec("cat /proc/cpuinfo | grep Model");
	std::tstring modelName;
	core::Split(modelName_raw, ":", &modelName);

	this->setModelNumber(core::Trim(modelName, " \n\r"));
}

void CDevice::collectNetworkInfo()
{
	// Connection Method => pinout 커맨드 이용
		// 일단은 임시로 Wifi + Bluetooth만 조사 => 추후 추가예정
	//std::tstring temp = exec("sudo pinout | grep -e \"Wi - fi\" -e \"Bluetooth\" | awk \'{print $1}\'");
	//std::cout << ":: size :: " << temp.length() << '\n';

	//std::vector<std::tstring> methods = split(temp, '\n');

	//this->metaInfo.connectMethod = methods;

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
			struct rtnl_link_stats* stats = (rtnl_link_stats*)ifa->ifa_data;

			auto s = (sockaddr_ll*)ifa->ifa_addr;
			char mac[] = "00:13:a9:1f:b0:88";

			auto a = s->sll_addr;

			sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);

			std::string macAddr = mac;
			nowNetInfo->m_addr = macAddr;

		}
	}

	freeifaddrs(ifaddr);

	this->metaInfo.networkInfo.clear();
	for (auto it : checker)
	{
		ST_NETWORK_INTERFACE_INFO temp;
		temp.if_name = it.second->if_name;
		temp.m_addr = it.second->m_addr;
		temp.inet_addr = it.second->inet_addr;
		temp.inet6_addr = it.second->inet6_addr;

		this->metaInfo.networkInfo.push_back(temp);
	}
}

void CDevice::collectOsInfo()
{
	// API version => <sys/utsname.h>
		// Command => lsb_release -a
		// File version => cat /etc/os-release 혹은 cat /proc/version

		// osName, release가 포함됨.

	ST_OS_INFO _osInfo;

	std::string osName_raw = exec("cat /etc/os-release | grep ^NAME");
	std::string osName;
	std::cout << osName_raw << std::endl;

	core::Split(osName_raw, "=", &osName);

	std::string osRelease_raw = exec("cat /etc/os-release | grep ^VERSION=");
	std::string osRelease;
	std::cout << osRelease_raw << std::endl;

	core::Split(osRelease_raw, "=", &osRelease);

	_osInfo.osName = core::Trim(osName.c_str(), " \"\n\r");
	_osInfo.osRelease = core::Trim(osRelease.c_str(), " \"\n\r");

	this->metaInfo.osInfo = _osInfo;
}

void CDevice::collectCpuInfo()
{
	// Command version => lscpu 혹은 sudo dmidecode --type processor
// File version => cat /proc/cpuinfo

// arch, core 수, vendor ID
}

void CDevice::collectServiceInfo()
{
	// serviceName, isActive 반환

	std::string serviceList_raw = exec("service --status-all");
	std::string serviceList = std::regex_replace(serviceList_raw, std::regex(" \\[ "), "");
	serviceList = std::regex_replace(serviceList, std::regex(" \\]  "), "");
	std::vector<std::string> temp = split(serviceList, '\n');

	this->metaInfo.serviceList.clear();
	for (auto it : temp)
	{
		ST_SERVICE_INFO _sInfo;
		char initialChar = it[0];

		_sInfo.serviceName = it.substr(1);
		_sInfo.isActive = false;

		if (initialChar == '+') _sInfo.isActive = true;

		this->metaInfo.serviceList.push_back(_sInfo);
	}
}

CDevice::CDevice()
{
	this->metaInfo.name = "";
	this->metaInfo.osInfo.osName = "";
	this->metaInfo.osInfo.osRelease = "";
	this->metaInfo.modelNumber = "";
	this->metaInfo.moduleCount = 0;
	this->metaInfo.networkInfo.clear();
	this->metaInfo.serviceList.clear();
	this->metaInfo.connectMethod.clear();
}

CDevice::~CDevice()
{
}

void CDevice::collectAllData(void)
{
	this->collectNameInfo();
	this->collectModelName();
	this->collectNetworkInfo();
	this->collectOsInfo();
	this->collectCpuInfo();
	this->collectServiceInfo();
}
