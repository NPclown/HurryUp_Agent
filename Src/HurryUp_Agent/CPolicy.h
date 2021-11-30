#pragma once

#include "CExecutor.h"

//TODO :: 정책 관리하는 클래스
class CPolicy : public ::CExecutor
{
private:
	ST_POLICY_REQUEST stPolicy;
public:
	CPolicy(ST_POLICY_REQUEST _stPolicy);
	~CPolicy();
	int Execute();
};

