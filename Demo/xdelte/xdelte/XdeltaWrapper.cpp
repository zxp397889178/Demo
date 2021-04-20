#include "stdafx.h"
#include "XdeltaAPI.h"
#include "XdeltaWrapper.h"
#include "Util.h"

#define NOT_MAIN 1
#define XD3_WIN32 1
#define EXTERNAL_COMPRESSION 0

#include "xdelta3.c"

bool CXdeltaWrapper::CreatePatch(XdeltaModel * pModel)
{
	bool bRet = false;
	if(pModel->szSrcFile && pModel->szDestFile && pModel->szPatchFile)
	{
		if(0 != wcscmp(pModel->szSrcFile, pModel->szDestFile))
		{
			std::string strSrcFile = Xdelte::Util::Str2Utf8(pModel->szSrcFile);
			std::string strPatchFile = Xdelte::Util::Str2Utf8(pModel->szPatchFile);
			std::string strDestFile = Xdelte::Util::Str2Utf8(pModel->szDestFile);
			int argc = 7;
			char* argv[8] = {"xdelta","-f","-e","-s",
				(char *)(strSrcFile.c_str()),
				(char *)(strPatchFile.c_str()),
				(char *)(strDestFile.c_str()),
				"\0"
			};
			if( EXIT_SUCCESS == xd3_main_cmdline(argc,argv))
				bRet = true;
		}	
	}

	return bRet;
}

bool CXdeltaWrapper::ApplyPatch(XdeltaModel * pModel)
{
	bool bRet = false;
	if(pModel->szSrcFile && pModel->szDestFile && pModel->szPatchFile)
	{
		if(0 != wcscmp(pModel->szSrcFile, pModel->szDestFile))
		{
			std::string strSrcFile = Xdelte::Util::Str2Utf8(pModel->szSrcFile);
			std::string strPatchFile = Xdelte::Util::Str2Utf8(pModel->szPatchFile);
			std::string strDestFile = Xdelte::Util::Str2Utf8(pModel->szDestFile);
			int argc = 7;
			char* argv[8]={"xdelta","-f","-d","-s",
				(char *)(strSrcFile.c_str()),
				(char *)(strPatchFile.c_str()),
				(char *)(strDestFile.c_str()),
				"\0"
			};
			if(EXIT_SUCCESS == xd3_main_cmdline(argc,argv))
				bRet = true;
		}
	}
	
	return bRet;
}
