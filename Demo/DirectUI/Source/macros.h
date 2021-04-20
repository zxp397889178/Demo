#ifndef __DIRECTUI_MACROS_H_
#define __DIRECTUI_MACROS_H_

#ifdef _DEBUG
#define RT_ASSERT(cond, msg)																\
	{																						\
		if (!cond)																		\
		{																					\
			::MessageBox(HWND_DESKTOP, msg, _T("error"), MB_OK | MB_SYSTEMMODAL);			\
		}																					\
	}
#define RT_CLASS_ASSERT(pObject, class_name, tip)											\
	{																						\
		if (pObject)																		\
			RT_ASSERT((dynamic_cast<class_name*>(pObject)), tip)							\
	}
#else
#define RT_ASSERT(cond, msg)
#define RT_CLASS_ASSERT(pObject, class_name, tip)
#endif

#endif //__DIRECTUI_MACROS_H_