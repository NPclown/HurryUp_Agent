#pragma once
#include "stdafx.h"

class CEnvironment : public core::IFormatterObject
{
private:
    std::tstring serverIp;
    std::tstring serverPort;
    std::tstring serverFilePort;
    std::tstring serialNumber;
    std::tstring environment;

    CEnvironment(void);
    ~CEnvironment(void);
public:
    static CEnvironment* GetInstance(void);
    void Init();
    const char* GetServerIp();
    int GetServerPort();
    int GetServerFilePort();
    std::tstring GetSerialNumber();
    std::tstring GetEnvironment();

    bool operator== (const CEnvironment& t)
    {
        return this->serverIp == t.serverIp &&
            this->serverPort == t.serverPort &&
            this->serverFilePort == t.serverFilePort &&
            this->serialNumber == t.serialNumber &&
            this->environment == t.environment;
    }

    void OnSync(core::IFormatter& formatter)
    {
        formatter
            + core::sPair(TEXT("server_ip"), serverIp)
            + core::sPair(TEXT("server_port"), serverPort)
            + core::sPair(TEXT("server_file_port"), serverFilePort)
            + core::sPair(TEXT("serial_number"), serialNumber)
            + core::sPair(TEXT("environment"), environment)
            ;
    }
};

inline CEnvironment* EnvironmentManager()
{
    return CEnvironment::GetInstance();
}