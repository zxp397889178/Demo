/*!
 * \file Gesture.h
 * \date 2018/03/27 21:10
 *
 * \author Racv
 *
 * \brief WINDOWS��������
 *
 * ΪDIRECTUI�ṩWINDOWS���ػ���֧��
 *
 * \note
 */

#pragma once
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <boost/function.hpp>

//NOTE:���̰߳�ȫ����ʹ��util�ĵ���������������!
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

//���ƶ���������ע�ắ�����ṩ��Ӧ��Ӧ
struct GestureAction
{
	boost::function<bool(CWindowUI* touch, const POINT& test)> HitTestFunc;		//����ֵ, ture:���иÿؼ�
	boost::function<bool(int offsetx)> XScrollFunc;								//����ֵ, ture:�����Ӿ����� X��false������
	boost::function<bool(int offsety)> YScrollFunc;								//����ֵ, ture:�����Ӿ����� Y��false������
};

class CUI_API CGestureSubject
{
public:
	bool RegisterCtrl(CContainerUI* key, const GestureAction& ga);
	bool UnregisterCtrl(CContainerUI* key);

	//test �ͻ�������
	CContainerUI* HitTest(CWindowUI* pWnd, POINT& test, GestureAction& cur_ga) const;

private:
	//KEY:���ڼ��������ؼ���VALUE:���ƶ���
	std::map<CContainerUI*, GestureAction> gestures_;
};

namespace gesture
{
	//��������֧�֣�ƽ�Ƽ�����
	class CUI_API CBasicGestureSupport
	{
	public:
		virtual ~CBasicGestureSupport();

	protected:
		void InitGesture(CContainerUI* pGrid);
		void UninitGesture();

		//������������
		virtual void SetHitRegion(const RECT& region);

		//pan

		//��ӦXƽ���¼� �޹��������
		virtual bool OnNotXSrcoll(int offsetx);

		//��ӦXƽ���¼� �й�����, ����ֵ�Ƿ񴥶�����
		virtual void OnHaveXSrcoll(int offsetx);

		//��ӦXƽ���¼� �й��������޷����������
		virtual bool OnXScrollEndFeedback(int offsetx);

		//��ӦYƽ���¼� �޹��������
		virtual bool OnNotYSrcoll(int offsetx);

		//��ӦYƽ���¼� �й�����
		virtual void OnHaveYSrcoll(int offsety);

		//��ӦYƽ���¼� �й��������޷����������
		virtual bool OnYScrollEndFeedback(int offsety);

		//zoom

	protected:
		CBasicGestureSupport();

		virtual CContainerUI*  GetCurGrid();

	private:
		bool XScroll(int offsetx);

		bool YScroll(int offsety);

		//���в���
		bool HitTestFunc(CWindowUI* touch, const POINT& test);

	private:
		CContainerUI* m_pGrid = nullptr;
		RECT m_rcHitRegion = RECT{};
	};

	//֧��ƽ�ƺͻ������ƵĿؼ�
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
