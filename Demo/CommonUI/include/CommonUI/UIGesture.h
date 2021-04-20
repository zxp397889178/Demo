/*!
 * \file Gesture.h
 * \date 2018/03/27 21:10
 *
 * \author Racv
 *
 * \brief WINDOWS触控手势
 *
 * 为DIRECTUI提供WINDOWS触控基本支持
 *
 * \note
 */

#pragma once
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <boost/function.hpp>

//NOTE:非线程安全，不使用util的单件，加锁花销大!
template<typename T>
class singleton
{
public:
	template<typename... Args>
	static T* instance(Args&&... args)
	{
		if (nullptr == instance_)
			instance_ = new T(std::forward<Args>(args)...);

		return instance_;
	}

	static T* get_instance()
	{
		if (nullptr == instance_)
			throw std::logic_error("not init.");

		return instance_;
	}

	static void destroy_instance()
	{
		delete instance_;
		instance_ = nullptr;
	}

private:
	singleton();
	virtual ~singleton();
	singleton(const singleton&);
	singleton& operator=(const singleton&);

private:
	static T* instance_;
};

template<typename T>
T* singleton<T>::instance_ = nullptr;

LRESULT CUI_API DecodeGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//手势动作反馈，注册函数以提供对应响应
struct GestureAction
{
	boost::function<bool(CWindowUI* touch, const POINT& test)> HitTestFunc;		//返回值, ture:命中该控件
	boost::function<bool(int offsetx)> XScrollFunc;								//返回值, ture:启用视觉反馈 X，false不启用
	boost::function<bool(int offsety)> YScrollFunc;								//返回值, ture:启用视觉反馈 Y，false不启用
};

class CUI_API CGestureSubject
{
public:
	bool RegisterCtrl(CContainerUI* key, const GestureAction& ga);
	bool UnregisterCtrl(CContainerUI* key);

	//test 客户端坐标
	CContainerUI* HitTest(CWindowUI* pWnd, POINT& test, GestureAction& cur_ga) const;

private:
	//KEY:用于检测点的区域控件，VALUE:手势动作
	std::map<CContainerUI*, GestureAction> gestures_;
};

namespace gesture
{
	//基本手势支持，平移及滑动
	class CUI_API CBasicGestureSupport
	{
	public:
		virtual ~CBasicGestureSupport();

	protected:
		void InitGesture(CContainerUI* pGrid);
		void UninitGesture();

		//设置命中区域
		virtual void SetHitRegion(const RECT& region);

		//pan

		//响应X平移事件 无滚动条情况
		virtual bool OnNotXSrcoll(int offsetx);

		//响应X平移事件 有滚动条, 返回值是否触动反馈
		virtual void OnHaveXSrcoll(int offsetx);

		//响应X平移事件 有滚动条且无法滚动的情况
		virtual bool OnXScrollEndFeedback(int offsetx);

		//响应Y平移事件 无滚动条情况
		virtual bool OnNotYSrcoll(int offsetx);

		//响应Y平移事件 有滚动条
		virtual void OnHaveYSrcoll(int offsety);

		//响应Y平移事件 有滚动条且无法滚动的情况
		virtual bool OnYScrollEndFeedback(int offsety);

		//zoom

	protected:
		CBasicGestureSupport();

		virtual CContainerUI*  GetCurGrid();

	private:
		bool XScroll(int offsetx);

		bool YScroll(int offsety);

		//命中测试
		bool HitTestFunc(CWindowUI* touch, const POINT& test);

	private:
		CContainerUI* m_pGrid = nullptr;
		RECT m_rcHitRegion = RECT{};
	};

	//支持平移和滑动手势的控件
	class CGestureLayout : 
		public CLayoutUI,
		public CBasicGestureSupport
	{
		UI_OBJECT_DECLARE(CGestureLayout, _T("GestureLayout"));
	public:
		virtual void Init();
		virtual void OnDestroy();
	};
}
