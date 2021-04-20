#ifndef __view_ax_inner_h__
#define __view_ax_inner_h__

//#define DCHECK_NE(val1, val2)        DCHECK_OP(NE, !=, val1, val2)
#define DCHECK_NE(val1, val2)

// #define DCHECK(condition) \
// 	LAZY_STREAM(LOG_STREAM(DCHECK), DCHECK_IS_ON() && !(condition)) \
// 	<< "Check failed: " #condition ". "
#define DCHECK(condition)

//#define NOTREACHED() DCHECK(false)
#define NOTREACHED()

// #define COMPILE_ASSERT(expr, msg) \
//	typedef CompileAssert<(bool(expr))> msg[bool(expr)?1:-1]
#define COMPILE_ASSERT(expr, msg)

#include "ax_host.h"
#include "scoped_comptr.h"
#include "scoped_ptr.h"
#include "flash10t.tlh"
using namespace ShockwaveFlashObjects;


#endif