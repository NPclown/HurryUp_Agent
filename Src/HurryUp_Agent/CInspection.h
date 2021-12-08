#pragma once
#include "CExecutor.h"

//TODO :: 정책 관리하는 클래스
class CInspection : public ::CExecutor
{
private:
	ST_INSPECTION_REQUEST stInspection;
public:
	CInspection(ST_INSPECTION_REQUEST _stInspection);
	~CInspection();
	bool Execute();
};

