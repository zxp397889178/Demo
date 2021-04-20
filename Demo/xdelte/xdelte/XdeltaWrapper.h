#pragma once

class CXdeltaWrapper : public IXdeltaWrapper
{
public:
	CXdeltaWrapper() {}
	~CXdeltaWrapper() {}
public:
	bool CreatePatch(XdeltaModel* pModel);
	bool ApplyPatch(XdeltaModel* pModel);
};