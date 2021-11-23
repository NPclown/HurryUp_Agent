#pragma once
#include "protocol.h"
#include "CUtils.h"
#include <future>
#include <iostream>
#include <regex>

#include <ifaddrs.h>
#include <linux/if_link.h>  
#include <netdb.h>

struct ST_ENV
{
	std::tstring serverIP;
	std::tstring serverPort;
};
