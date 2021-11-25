#pragma once
#include "stdafx.h"

struct sockaddr_ll
{
    uint16_t sll_family;
    uint16_t sll_protocol;
    uint32_t sll_ifindex;
    uint16_t sll_hatype;
    uint8_t sll_pkttype;
    uint8_t sll_halen;
    uint8_t sll_addr[8];
};

void SetLogger(std::tstring _name, DWORD _inputOption);
std::tstring exec(const char* cmd);
std::tstring ReadContent(const char* path);
std::vector<std::tstring> split(std::string input, char delimiter);
int FindFileEndPosition(std::ifstream& file);
std::tstring ColumnSplit(std::tstring input, std::tstring delimiter);
std::string GetTimeStamp();



