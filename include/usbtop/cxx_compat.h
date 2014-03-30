#ifndef CXX_COMPAT_H
#define CXX_COMPAT_H

#ifdef __GNUC__
	#if GCC_VERSION >= 40700
		#define __STD_EMPLACE_SUPPORT
	#endif
#endif

#endif
