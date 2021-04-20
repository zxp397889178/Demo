#pragma once

namespace UCSDK
{
    /*
    Wiki：
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BGET.5D.2Fserver.2Ftime_.E8.8E.B7.E5.8F.96.E6.9C.8D.E5.8A.A1.E5.99.A8.E6.97.B6.E9.97.B4
    描述：获取服务器时间
    功能说明：获取服务器时间
    访问权限：
    */
    class IGetServerTime
    {
    public:
        virtual bool Excute() = 0;
		virtual time_t GetTime() = 0;//服务器时间
        virtual const char* GetReturnCode() = 0;
        virtual const wchar_t* GetReturnMessage() = 0;
    public:
        virtual DWORD GetHttpCode() = 0;
        virtual const char* GetResult() = 0;
        virtual const char* GetHttpHeader() = 0;
        virtual void Release() = 0;
    };

    /*
    返回Code列表:
    UC/REQUIRE_ARGUMENT 缺少参数
    */
	typedef class IGetServerTime	GetServerTime;
}