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


#define BUFFER_SIZE 1024
#define TAR_EXT (".tar.gz")

//COMMAND
#define CHMOD_COMMAND TEXT("chmod -R 755 %s > /dev/null 2>&1; echo $?")
#define TAR_COMMAND TEXT("tar -zxvf %s -C %s  > /dev/null 2>&1; echo $?")
#define RM_COMMAND TEXT("rm -rf %s  > /dev/null 2>&1; echo $?")
#define EXE_COMMAND TEXT("cd %s; sudo %s > /dev/null 2>&1; echo $?")