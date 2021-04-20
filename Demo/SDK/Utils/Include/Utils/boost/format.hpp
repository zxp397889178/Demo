// ----------------------------------------------------------------------------
// format.hpp :  primary header
// ----------------------------------------------------------------------------

//  Copyright Samuel Krempp 2003. Use, modification, and distribution are
//  subject to the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/format for library home page


// ----------------------------------------------------------------------------

#ifndef BOOST_FORMAT_HPP
#define BOOST_FORMAT_HPP

#include <vector>
#include <string>
#include <assert.h>


// BOOST øÚº‹Ã·»°
#ifndef BOOST_MSVC
#define BOOST_MSVC
#endif

#ifndef BOOST_IO_STD
#define BOOST_IO_STD ::std::
#endif

#ifndef BOOST_NOEXCEPT_OR_NOTHROW
#define BOOST_NOEXCEPT_OR_NOTHROW
#endif

#ifndef BOOST_FALLTHROUGH
#define BOOST_FALLTHROUGH ((void)0)
#endif

#ifndef BOOST_USE_FACET
#define BOOST_USE_FACET(Type, loc) std::use_facet< Type >(loc)
#endif

#ifndef BOOST_HAS_FACET
#define BOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#endif

#ifndef BOOST_ASSERT
#define BOOST_ASSERT(expr) assert(expr)
#endif

namespace tiny_boost{
	template<class E>
	inline void throw_exception(E const & e)
	{
		throw e;
	}
}

namespace tiny_boost{namespace io{namespace detail{
	typedef BOOST_IO_STD locale locale_t;
}}}

namespace tiny_boost {
	namespace io {
		template<class Tr>
		class CompatTraits
		{        // general case : be transparent
		public:
			typedef Tr  compatible_type;
		};

		template<class Alloc>
		class CompatAlloc
		{        // general case : be transparent
		public:
			typedef Alloc  compatible_type;
		};

	} //N.S. io
} // N.S. boost

//#include <boost/detail/workaround.hpp>
//#include <boost/config.hpp>

#ifndef BOOST_NO_STD_LOCALE
#include <locale>
#endif

// ***   Compatibility framework
//#include <boost/format/detail/compat_workarounds.hpp>

#ifdef BOOST_NO_LOCALE_ISIDIGIT
#include <cctype>  // we'll use the non-locale  <cctype>'s std::isdigit(int)
#endif

// ****  Forward declarations ----------------------------------
#include "boost/format/format_fwd.hpp"     // basic_format<Ch,Tr>, and other frontends
#include "boost/format/internals_fwd.hpp"  // misc forward declarations for internal use

// ****  Auxiliary structs (stream_format_state<Ch,Tr> , and format_item<Ch,Tr> )
#include "boost/format/internals.hpp"    

// ****  Format  class  interface --------------------------------
#include "boost/format/format_class.hpp"

// **** Exceptions -----------------------------------------------
#include "boost/format/exceptions.hpp"

// **** Implementation -------------------------------------------
#include "boost/format/format_implementation.hpp"   // member functions
#include "boost/format/group.hpp"                   // class for grouping arguments
#include "boost/format/feed_args.hpp"               // argument-feeding functions
#include "boost/format/parsing.hpp"                 // format-string parsing (member-)functions

// **** Implementation of the free functions ----------------------
#include "boost/format/free_funcs.hpp"


// *** Undefine 'local' macros :
//#include "boost/format/detail/unset_macros.hpp"
#undef BOOST_MSVC
#undef BOOST_IO_STD 
#undef BOOST_NOEXCEPT_OR_NOTHROW
#undef BOOST_FALLTHROUGH 
#undef BOOST_USE_FACET
#undef BOOST_HAS_FACET
#undef BOOST_ASSERT
#endif // BOOST_FORMAT_HPP
