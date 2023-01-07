#pragma once

#include "GIL.h"

class NumAvailable : public GIL::DataSequence
{
public:
	NumAvailable() : DataSequence(nullptr) {}
	
	virtual void* Data(GIL::Parser::Project* Proj) override { return (void*)m_numAvailable--; }

	int m_numAvailable = 999;
};