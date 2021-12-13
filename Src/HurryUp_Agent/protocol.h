#pragma once
#include <cppcore.h>
// TODO :: 서버간 통신에 필요한 프로토콜 구현

enum PROTOCOL {
    // 자발적인 전송을 수행하는 프로토콜 + 요청에 데이터가 들어가 있지 않는 경우
    DEVICE,                 //장치 정보 요청
    MODULE,                 //모듈 정보 요청
    PROCESS,                //프로세스 목록 요청
    FILEDESCRIPTOR,         //프로세스 파일디스크립터 목록 응답
    MONITORING_LOG,         //모니터링 로그 결과
    INSPECTION_RESULT,      //점검 수행 결과

    // 요청에 데이터가 들어가 있는 경우
    MONITORING_REQUEST,     //특정 파일 모니터링 요청
    POLICY_REQUEST,         //정책 실행 요청
    INSPECTION_REQUEST,     //점검 요청
    CHANGE_INTERVAL_REQUEST,//정보 수집 인터벌 변경 요청

    RESPONSE                //요청에 대한 응답 
};

// 메시지 전송에 필요한 메인 구조
struct ST_PACKET_INFO : public core::IFormatterObject
{
    int protocol;
    std::tstring data;

    ST_PACKET_INFO(void)
    {}
    ST_PACKET_INFO(int _protocol, std::tstring _data)
        : protocol(_protocol), data(_data)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("protocol"), protocol)
            + core::sPair(TEXT("data"), data)
            ;
    }
};

template <typename META_INFO>
struct ST_INFO : public core::IFormatterObject
{
    std::tstring serialNumber;
    std::tstring timestamp;
    META_INFO metaInfo;

    ST_INFO(void)
    {}
    ST_INFO(std::tstring _serialNumber, std::tstring _timestamp, META_INFO _metaInfo)
        : serialNumber(_serialNumber), timestamp(_timestamp), metaInfo(_metaInfo)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("serial_number"), serialNumber)
            + core::sPair(TEXT("timestamp"), timestamp)
            + core::sPair(TEXT("metainfo"), metaInfo)
            ;
    }
};

struct ST_NETWORK_INTERFACE_INFO : public core::IFormatterObject
{
    std::tstring if_name;
    std::tstring m_addr;
    std::tstring inet_addr;
    std::tstring inet6_addr;

    ST_NETWORK_INTERFACE_INFO(void)
    {}
    ST_NETWORK_INTERFACE_INFO(std::tstring _if_name, std::tstring _m_addr, std::tstring _inet_addr, std::tstring _inet6_addr)
        : if_name(_if_name), m_addr(_m_addr), inet_addr(_inet_addr), inet6_addr(_inet6_addr)
    {}

    bool operator== (const ST_NETWORK_INTERFACE_INFO& t)
    {
        return this->if_name == t.if_name &&
            this->m_addr == t.m_addr &&
            this->inet_addr == t.inet_addr &&
            this->inet6_addr == t.inet6_addr;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("if_name"), if_name)
            + core::sPair(TEXT("mac_addr"), m_addr)
            + core::sPair(TEXT("inet_addr"), inet_addr)
            + core::sPair(TEXT("inet6_addr"), inet6_addr)
            ;
    }
};

struct ST_OS_INFO : public core::IFormatterObject
{
    std::tstring osName;
    std::tstring osRelease;

    ST_OS_INFO(void)
    {}
    ST_OS_INFO(std::tstring _osName, std::tstring _osRelease)
        : osName(_osName), osRelease(_osRelease)
    {}

    bool operator== (const ST_OS_INFO& t)
    {
        return this->osName == t.osName &&
            this->osRelease == t.osRelease;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("os_name"), osName)
            + core::sPair(TEXT("os_release"), osRelease)
            ;
    }
};

struct ST_SERVICE_INFO : public core::IFormatterObject
{
    std::tstring serviceName;
    bool isActive;

    ST_SERVICE_INFO(void)
    {}
    ST_SERVICE_INFO(std::tstring _serviceName, bool _isActive)
        : serviceName(_serviceName), isActive(_isActive)
    {}

    bool operator== (const ST_SERVICE_INFO& t)
    {
        return this->serviceName == t.serviceName &&
            this->isActive == t.isActive;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("service_name"), serviceName)
            + core::sPair(TEXT("isactive"), isActive)
            ;
    }
};

struct ST_DEVICE_INFO : public core::IFormatterObject
{
    std::tstring name;
    ST_OS_INFO osInfo;
    std::tstring modelName;
    std::vector<std::tstring> connectMethod;
    std::vector<ST_NETWORK_INTERFACE_INFO> networkInfo;

    uint32_t moduleCount = 0;

    std::vector<ST_SERVICE_INFO> serviceList;

    // template property; => TODO :: 템플릿화 구현 필요 (전상현 멘토님 11월 1주차 멘토링 참고)

    ST_DEVICE_INFO(void)
    {}
    ST_DEVICE_INFO(std::tstring _name, ST_OS_INFO _osInfo, std::tstring _modelName, std::vector<std::tstring> _connectMethod, std::vector<ST_NETWORK_INTERFACE_INFO> _networkInfo, uint32_t _moduleCount, std::vector<ST_SERVICE_INFO> _serviceList)
        : name(_name), osInfo(_osInfo), modelName(_modelName), connectMethod(_connectMethod), networkInfo(_networkInfo), moduleCount(_moduleCount), serviceList(_serviceList)
    {}

    ST_DEVICE_INFO& operator= (const ST_DEVICE_INFO& t)
    {
        this->name = t.name;
        this->osInfo = t.osInfo;
        this->modelName = t.modelName;
        this->connectMethod = t.connectMethod;
        this->moduleCount = t.moduleCount;

        this->networkInfo.clear();
        for (auto it : t.networkInfo)
            this->networkInfo.push_back(it);

        this->serviceList.clear();
        for (auto it : t.serviceList)
            this->serviceList.push_back(it);

        return *this;
    }

    bool operator== (const ST_DEVICE_INFO& t)
    {
        return this->name == t.name &&
            this->osInfo == t.osInfo &&
            this->modelName == t.modelName &&
            this->connectMethod == t.connectMethod &&
            this->moduleCount == t.moduleCount;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("name"), name)
            + core::sPair(TEXT("os_info"), osInfo)
            + core::sPair(TEXT("model_name"), modelName)
            + core::sPair(TEXT("connect_method"), connectMethod)
            + core::sPair(TEXT("network_info"), networkInfo)
            + core::sPair(TEXT("module_count"), moduleCount)
            + core::sPair(TEXT("service_list"), serviceList)
            ;
    }
};

struct ST_PROCESS_INFO : public core::IFormatterObject
{
    int pid;
    int ppid;
    std::tstring name;
    std::tstring state;
    std::tstring cmdline;
    std::tstring startTime;

    ST_PROCESS_INFO(void)
    {}
    ST_PROCESS_INFO(int _pid, int _ppid, std::tstring _name, std::tstring _state, std::tstring _cmdline, std::tstring _startTime)
        : pid(_pid), ppid(_ppid), name(_name), state(_state), cmdline(_cmdline), startTime(_startTime)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("pid"), pid)
            + core::sPair(TEXT("ppid"), ppid)
            + core::sPair(TEXT("name"), name)
            + core::sPair(TEXT("state"), state)
            + core::sPair(TEXT("cmdline"), cmdline)
            + core::sPair(TEXT("start_time"), startTime)
            ;
    }
};

struct ST_FD_INFO : public core::IFormatterObject
{
    int pid;
    std::string fdName;
    std::string realPath;

    ST_FD_INFO(void)
    {}
    ST_FD_INFO(int _pid, std::string _fdName, std::string _realPath)
        : pid(_pid), fdName(_fdName), realPath(_realPath)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("pid"), pid)
            + core::sPair(TEXT("fd_name"), fdName)
            + core::sPair(TEXT("real_path"), realPath)
            ;
    }
};

struct ST_LOG_INFO : public core::IFormatterObject
{
    std::string processName;
    std::string logPath;
    std::string changeData;

    ST_LOG_INFO(void)
    {}
    ST_LOG_INFO(std::string _processName, std::string _logPath, std::string _changeData)
        : processName(_processName), logPath(_logPath), changeData(_changeData)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("process_name"), processName)
            + core::sPair(TEXT("log_path"), logPath)
            + core::sPair(TEXT("change_data"), changeData)
            ;
    }
};

struct ST_MONITOR_INFO : public core::IFormatterObject
{
    std::string environment;
    ST_LOG_INFO logData;

    ST_MONITOR_INFO(void)
    {}
    ST_MONITOR_INFO(std::string _environment, ST_LOG_INFO _logData)
        : environment(_environment), logData(_logData)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("environment"), environment)
            + core::sPair(TEXT("log_data"), logData)
            ;
    }
};

struct ST_INSPECTION_RESULT : public core::IFormatterObject
{
    std::tstring title;
    std::tstring ref;
    bool result;
    std::vector<std::tstring> detail;
    std::tstring timestamp;

    ST_INSPECTION_RESULT(void)
    {}
    ST_INSPECTION_RESULT(std::tstring _title, std::tstring _ref, bool _result, std::vector<std::tstring> _detail, std::tstring _timestamp)
        : title(_title), ref(_ref), result(_result), detail(_detail), timestamp(_timestamp)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("title"), title)
            + core::sPair(TEXT("ref"), ref)
            + core::sPair(TEXT("detail"), detail)
            + core::sPair(TEXT("result"), result)
            + core::sPair(TEXT("timestamp"), timestamp)
            ;
    }
};


struct ST_INSPECTION_INFO : public core::IFormatterObject
{
    int inspectionIdx;
    int resultIdx;
    std::tstring inspectionName;
    ST_INSPECTION_RESULT result;

    ST_INSPECTION_INFO(void)
    {}
    ST_INSPECTION_INFO(int _inspectionIdx, int _resultIdx, std::tstring _inspectionName, ST_INSPECTION_RESULT _result)
        : inspectionIdx(_inspectionIdx), resultIdx(_resultIdx), inspectionName(_inspectionName), result(_result)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("inspection_idx"), inspectionIdx)
            + core::sPair(TEXT("result_idx"), resultIdx)
            + core::sPair(TEXT("inspection_name"), inspectionName)
            + core::sPair(TEXT("inspection_result"), result)
            ;
    }
};

struct ST_MONITOR_REQUEST : public core::IFormatterObject
{
    std::string processName;
    std::string logPath;
    bool activate;

    ST_MONITOR_REQUEST(void)
    {}
    ST_MONITOR_REQUEST(std::string _processName, std::string _logPath, bool _activate)
        : processName(_processName), logPath(_logPath), activate(_activate)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("process_name"), processName)
            + core::sPair(TEXT("log_path"), logPath)
            + core::sPair(TEXT("activate"), activate)
            ;
    }
};

struct ST_POLICY_REQUEST : public core::IFormatterObject
{
    std::tstring policyName;
    std::tstring policyID;
    std::tstring policyDescription;
    std::tstring command;
    bool activate;
    bool isFile;

    ST_POLICY_REQUEST(void)
    {}
    ST_POLICY_REQUEST(std::tstring _policyName, std::tstring _policyID, std::tstring _policyDescription, std::tstring _command, bool _activate, bool _isFile)
        : policyName(_policyName), policyID(_policyID), policyDescription(_policyDescription), command(_command), activate(_activate), isFile(_isFile)
    {}

    ST_POLICY_REQUEST& operator= (const ST_POLICY_REQUEST& t)
    {
        this->policyName = t.policyName;
        this->policyID = t.policyID;
        this->policyDescription = t.policyDescription;
        this->command = t.command;
        this->activate = t.activate;
        this->isFile = t.isFile;

        return *this;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("policy_name"), policyName)
            + core::sPair(TEXT("policy_id"), policyID)
            + core::sPair(TEXT("policy_description"), policyDescription)
            + core::sPair(TEXT("command"), command)
            + core::sPair(TEXT("activate"), activate)
            + core::sPair(TEXT("isfile"), isFile)
            ;
    }
};

//TODO :: 점검 정책에 맞게 변경 필요
struct ST_INSPECTION_REQUEST : public core::IFormatterObject
{
    int ticketIdx;
    int level;
    std::tstring fileName;

    ST_INSPECTION_REQUEST(void)
    {}
    ST_INSPECTION_REQUEST(int _ticketIdx, int _level, std::tstring _fileName)
        : ticketIdx(_ticketIdx), level(_level), fileName(_fileName)
    {}

    ST_INSPECTION_REQUEST& operator= (const ST_INSPECTION_REQUEST& t)
    {
        this->ticketIdx = t.ticketIdx;
        this->level = t.level;
        this->fileName = t.fileName;

        return *this;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("ticket_idx"), ticketIdx)
            + core::sPair(TEXT("level"), level)
            + core::sPair(TEXT("filename"), fileName)
            ;
    }
};

template <typename REQUEST_INFO, typename DETAIL_INFO>
struct ST_RESPONSE_INFO : public core::IFormatterObject
{
    bool result;
    int requestProtocol;
    REQUEST_INFO requestInfo;
    DETAIL_INFO detail;

    ST_RESPONSE_INFO(void)
    {}
    ST_RESPONSE_INFO(DETAIL_INFO _detail, REQUEST_INFO _requestInfo, bool _result, int _requestProtocol)
        : detail(_detail), requestInfo(_requestInfo), result(_result), requestProtocol(_requestProtocol)
    {}

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("result"), result)
            + core::sPair(TEXT("detail"), detail)
            + core::sPair(TEXT("request_protocol"), requestProtocol)
            + core::sPair(TEXT("request_info"), requestInfo)
            ;
    }
};