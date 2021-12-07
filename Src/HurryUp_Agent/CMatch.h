#pragma once
#include "stdafx.h"

class CMatch
{
private:
    void ReqDeviceInfo();
	void ReqProcessInfo();
	void ReqFileDescriptorInfo();
	void ReqMonitoring(std::tstring data);
	void ReqChangeInterval(std::tstring data);
	void ReqPolicy(std::tstring data);
	void ReqInspection(std::tstring data);
	CMatch();
public:
	static CMatch* GetInstance(void);
	void MatchMessage();
};

inline CMatch* MatchManager()
{
	return CMatch::GetInstance();
}
