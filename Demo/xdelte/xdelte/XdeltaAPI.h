#pragma once

class XdeltaModel
{
public:
	XdeltaModel() : szSrcFile(NULL), szDestFile(NULL), szPatchFile(NULL){}
public:
	LPCWSTR szSrcFile;
	LPCWSTR szDestFile;
	LPCWSTR szPatchFile;
};

//interface
class IXdeltaWrapper
{
public:
	virtual bool CreatePatch(XdeltaModel* pModel) = 0;
	virtual bool ApplyPatch(XdeltaModel* pModel) = 0;
};

class CXdeltaWrapperFactory
{
public:
	static IXdeltaWrapper* CreateXdeltaWrapper();
};
