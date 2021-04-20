#include "stdafx.h"
#include "ActionManager.h"

static std::multimap<BaseObject*, CAction*> g_mapTargetToActionElement;
typedef std::multimap<BaseObject*, CAction*>::iterator MAPAction;
typedef std::pair<BaseObject*, CAction*> MAPActionPair;
typedef std::pair<MAPAction, MAPAction> MAPActionElementPair;//用于批量查找

static std::multimap<CAction*, BaseObject*> g_mapActionToTargetElement;
typedef std::multimap<CAction*, BaseObject*>::iterator MAPTargetElement;
typedef std::pair<CAction*, BaseObject*> MAPTargetPair;
typedef std::pair<MAPTargetElement, MAPTargetElement> MAPTargetElementPair;//用于批量查找

static std::map<CWindowUI*, WindowAnimation*> g_mapCloseWindowAnimation;
typedef std::map<CWindowUI*, WindowAnimation*>::iterator MAPCloseWindowAnimation;
typedef std::pair<CWindowUI*, WindowAnimation*> MAPCloseWindowAnimationPair;

void AddAnimationPair(CAction* pAction, BaseObject* pTarget)
{
	if (pAction && pTarget)
	{
		{
			bool bFindTarget = false;
			MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
			if (pActionElement != g_mapTargetToActionElement.end())
			{
				MAPActionElementPair ActionElementRange = g_mapTargetToActionElement.equal_range(pTarget);
				for (pActionElement = ActionElementRange.first; pActionElement != ActionElementRange.second; ++pActionElement)
				{
					CAction* pFindAction = pActionElement->second;
					if (pFindAction && pFindAction == pAction)
					{
						bFindTarget = true;
					}
				}
			}
			if (!bFindTarget)
				g_mapTargetToActionElement.insert(MAPActionPair(pTarget, pAction));
		}

		{
			bool bFindAction = false;
			MAPTargetElement pTargetElement = g_mapActionToTargetElement.find(pAction);
			if (pTargetElement != g_mapActionToTargetElement.end())
			{
				MAPTargetElementPair TargetElementRange = g_mapActionToTargetElement.equal_range(pAction);
				for (pTargetElement = TargetElementRange.first; pTargetElement != TargetElementRange.second; ++pTargetElement)
				{
					BaseObject* pFindTarget = pTargetElement->second;
					if (pFindTarget && pFindTarget == pTarget)
					{
						bFindAction = true;
					}
				}
			}
			if (!bFindAction)
				g_mapActionToTargetElement.insert(MAPTargetPair(pAction, pTarget));
		}
	}
}

void RemoveAnimation(CAction* pAction)
{

	if (pAction && !g_mapActionToTargetElement.empty())
	{
		MAPTargetElement pTargetElement = g_mapActionToTargetElement.find(pAction);
		if (pTargetElement != g_mapActionToTargetElement.end())
		{
			MAPTargetElementPair TargetElementRange = g_mapActionToTargetElement.equal_range(pAction);
			for (pTargetElement = TargetElementRange.first; pTargetElement != TargetElementRange.second; ++pTargetElement)
			{
				BaseObject* pTarget = pTargetElement->second;
				if (pTarget)
				{
					MAPActionElementPair ActionElementRange = g_mapTargetToActionElement.equal_range(pTarget);
					for (MAPAction pActionElement = ActionElementRange.first; pActionElement != ActionElementRange.second; ++pActionElement)
					{
						CAction* pSubAction = pActionElement->second;
						if (pSubAction == pAction)
						{
							g_mapTargetToActionElement.erase(pActionElement);
							break;
						}
					}
				}
			}
			g_mapActionToTargetElement.erase(pAction);
		}
	}
}

bool IsActioning(BaseObject* pTarget)
{
	if (pTarget && !g_mapTargetToActionElement.empty())
	{
		MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
		while(pActionElement != g_mapTargetToActionElement.end())
		{
			CAction* pAction = pActionElement->second;
			if (pAction)
			{
				return true;
			}
			pActionElement = g_mapTargetToActionElement.find(pTarget);
		}
	}
	return false;
}

void ActionManager::addViewAnimation( ViewAnimation* pAction, CViewUI* pView, bool bStopOther /*= true*/ )
{
	if (bStopOther)
		removeViewAnimationByTarget(pView, true);

	addViewAnimationPair(pAction, pView);
	if (pAction && pView)
	{
		pAction->startWithTarget(pView);
	}
}

void ActionManager::addViewAnimationPair( ViewAnimation* pAction, CViewUI* pTarget )
{
	AddAnimationPair(pAction, pTarget);
}

void ActionManager::removeViewAnimation( ViewAnimation* pAction )
{
	RemoveAnimation(pAction);
}

void ActionManager::removeViewAnimationByTarget( CViewUI* pTarget, bool bStop /*= false*/ )
{
	if (pTarget && !g_mapTargetToActionElement.empty())
	{
		MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
		while(pActionElement != g_mapTargetToActionElement.end())
		{
			CAction* pAction = pActionElement->second;
			if (pAction)
			{
				if (bStop)
				{
					//若是析构引起的移除，不能再对绑定控件进行操作
					if (pAction->GetState() != CAction::ActionState_Finished && pAction->GetState() != CAction::ActionState_Stop)
					{
						pAction->SetState(CAction::ActionState_Stop);
					}
				}
				delete pAction;
			}
			pActionElement = g_mapTargetToActionElement.find(pTarget);
		}
	}
}

bool ActionManager::isViewAcioning( CViewUI* pTarget )
{
	return IsActioning(pTarget);
}

void ActionManager::addWindowAnimation( WindowAnimation* pAction, CWindowUI* pView, bool bStopOther /*= true*/ )
{
	if (bStopOther)
		removeWindowAnimationByTarget(pView, true);

	addWindowAnimationPair(pAction, pView);
	if (pAction && pView)
	{
		(static_cast<CAction*>(pAction))->startWithTarget(pView);
	}
}

void ActionManager::addWindowAnimationPair( WindowAnimation* pAction, CWindowUI* pTarget )
{
	AddAnimationPair(pAction, pTarget);
}

void ActionManager::removeWindowAnimation( WindowAnimation* pAction )
{
	RemoveAnimation(pAction);
}

void ActionManager::removeWindowAnimationByTarget( CWindowUI* pTarget, bool bStop /*= false*/ )
{
	if (pTarget && !g_mapTargetToActionElement.empty())
	{
		MAPAction pActionElement = g_mapTargetToActionElement.find(pTarget);
		while(pActionElement != g_mapTargetToActionElement.end())
		{
			CAction* pAction = pActionElement->second;
			if (pAction)
			{
				if (bStop)
				{
					//若是析构引起的移除，不能再对绑定控件进行操作
					if (pAction->GetState() != CAction::ActionState_Finished && pAction->GetState() != CAction::ActionState_Stop)
					{
						pAction->SetState(CAction::ActionState_Stop);
					}
				}
				delete pAction;
			}
			pActionElement = g_mapTargetToActionElement.find(pTarget);
		}
	}
}

bool ActionManager::isWindowAcioning( CWindowUI* pTarget )
{
	return IsActioning(pTarget);
}

ActionManager* g_pActionManager = NULL;
ActionManager::ActionManager()
{
	g_pActionManager = this;
}

ActionManager::~ActionManager()
{
	removeAllActions();
	g_pActionManager = NULL;
}

ActionManager* ActionManager::GetInstance()
{
	if (!g_pActionManager)
		g_pActionManager = new ActionManager;
	return g_pActionManager;
}

void ActionManager::ReleaseInstance()
{
	if (g_pActionManager)
		delete g_pActionManager;
	g_pActionManager = NULL;
}

void ActionManager::removeAllActions()
{
	if (!g_mapTargetToActionElement.empty())
	{
		CAction* pAction = NULL;
		std::set<CAction*> setActions;
		for (MAPTargetElement it = g_mapActionToTargetElement.begin(); it != g_mapActionToTargetElement.end(); ++it)
		{
			pAction = it->first;
			setActions.insert(pAction);
		}

		for (MAPCloseWindowAnimation it = g_mapCloseWindowAnimation.begin(); it != g_mapCloseWindowAnimation.end(); ++it)
		{
			pAction = it->second;
			setActions.insert(pAction);
		}

		g_mapTargetToActionElement.clear();
		g_mapActionToTargetElement.clear();
		g_mapCloseWindowAnimation.clear();

		for (std::set<CAction*>::iterator it = setActions.begin(); it != setActions.end(); ++it)
		{
			pAction = *it;
			if (pAction->IsFinishedOfRelease())
			{
				delete pAction;
			}			
		}
		setActions.clear();


	}
}

bool ActionManager::isAction( CAction* pAction )
{
	MAPTargetElement pTargetElement = g_mapActionToTargetElement.find(pAction);
	return pTargetElement != g_mapActionToTargetElement.end();
}

void ActionManager::addCloseWindowAnimation( WindowAnimation* pWindowAnimation, CWindowUI* pWindow )
{
	if (pWindowAnimation && pWindow)
	{
		pWindowAnimation->SetAnimationState(WindowAnimation::WindowAnimationState_Close);

		MAPCloseWindowAnimation it = g_mapCloseWindowAnimation.find(pWindow);
		if (it != g_mapCloseWindowAnimation.end())
			delete it->second;

		g_mapCloseWindowAnimation.insert(MAPCloseWindowAnimationPair(pWindow, pWindowAnimation));
	}
}

WindowAnimation* ActionManager::getCloseWindowAnimation( CWindowUI* pWindow, bool bRemove /*= true*/ )
{
	WindowAnimation* pWindowAnimation = NULL;
	MAPCloseWindowAnimation it = g_mapCloseWindowAnimation.find(pWindow);
	if (it != g_mapCloseWindowAnimation.end())
	{
		pWindowAnimation = it->second;
		if (bRemove)
		{
			g_mapCloseWindowAnimation.erase(it);
		}
	}
	return pWindowAnimation;
}