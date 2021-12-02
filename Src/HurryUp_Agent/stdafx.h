#pragma once
#include "protocol.h"
#include "CUtils.h"
#include <future>
#include <iostream>
#include <regex>

#include <unistd.h>
#include <ifaddrs.h>
#include <linux/if_link.h>  
#include <netdb.h>
#include <mutex>
#include "CEnvironment.h"


#define BUFFER_SIZE 1024
#define TAR_EXT (".tar.gz")
#define DIRPATH TEXT("/var/log/hurryup/")
#define AGENT_DIRPATH TEXT("/var/log/hurryup/agent/")
#define ENV_PATH TEXT("/var/log/hurryup/agent/env.json")

//COMMAND
#define SERVICE_COMMAND TEXT("service --status-all")
#define OSNAME_COMMAND TEXT("cat /etc/os-release | grep ^NAME")
#define OSVERSION_COMMAND TEXT("cat /etc/os-release | grep ^VERSION=")
#define MODLENAME_COMMAND TEXT("cat /proc/cpuinfo | grep Model")
#define CHMOD_COMMAND TEXT("chmod -R 755 %s > /dev/null 2>&1; echo $?")
#define TAR_COMMAND TEXT("tar -zxvf %s -C %s  > /dev/null 2>&1; echo $?")
#define RM_COMMAND TEXT("rm -rf %s  > /dev/null 2>&1; echo $?")
#define EXE_COMMAND TEXT("cd %s; sudo %s > /dev/null 2>&1; echo $?")