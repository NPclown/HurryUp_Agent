#pragma once
#include "stdafx.h"

class CMatch
{
private:
    void ReqDeviceInfo();
	void ReqProcessInfo();
	void ReqFileDescriptorInfo();
	void ReqMonitoringActivate();
	void ReqMonitoringInactivate();
	void ReqPolicyActivate();
	void ReqPolicyInactivate();
	void ReqInspectionActivate();
	CMatch();
public:
	static CMatch* GetInstance(void);
	void MatchMessage();
};

inline CMatch* MatchManager()
{
	return CMatch::GetInstance();
}
