#include "stdafx.h"
#include "XdeltaAPI.h"
#include "XdeltaWrapper.h"

IXdeltaWrapper* CXdeltaWrapperFactory::CreateXdeltaWrapper()
{
	return new CXdeltaWrapper();
}
