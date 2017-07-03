#include "base/nDebugger.h"
#include "base/nSys.h"

namespace Tidy
{
	std::string nDebugger::backTrace()
	{
		std::string backTraceResult;
		return backTrace(backTraceResult, NULL);
	}

	static void *getMcontextEip(ucontext_t *uc);
	std::string& nDebugger::backTrace(std::string& backTraceResult, ucontext_t *uc)
	{
		const int BACK_TRACE_SIZE = 100;

		void* backTraceBuf[BACK_TRACE_SIZE];
		char** backTraceFuncs;

		int bufSize = backtrace(backTraceBuf, BACK_TRACE_SIZE);
		int start = 0;
		if (uc != NULL && getMcontextEip(uc) != NULL) {
			backTraceBuf[2] = getMcontextEip(uc);
			start = 2;
		}
		backTraceFuncs = backtrace_symbols(backTraceBuf, bufSize);

		size_t nameSize = 256;
		char* name = (char*)malloc(nameSize);

		for (int i = start; i < bufSize; ++ i){
			char* beginName = NULL;
			char* beginOffset = NULL;
			char* endOffset = NULL;

			for (char* p = backTraceFuncs[i]; *p; ++p){
				if (*p == '(')
					beginName = p;
				else if (*p == '+')
					beginOffset = p;
				else if (*p == ')' && beginOffset)
				{
					endOffset = p;
					break;
				}
			}

			if (beginName && beginOffset && endOffset && beginName < beginOffset)
			{
				*beginName++ = '\0';
				*beginOffset++ = '\0';
				*endOffset++ = '\0';

				int status;
				char* ret = abi::__cxa_demangle(beginName, name, 
						&nameSize, &status);

				if (status == 0)
				{
					name = ret;
					backTraceResult += name;
				}
				else
					backTraceResult += name;
			}
			else
				backTraceResult += backTraceFuncs[i];

			backTraceResult += '\n';
		}

		free(name);
		free(backTraceFuncs);
		return backTraceResult;
	}

	// from redis
	static void *getMcontextEip(ucontext_t *uc) 
	{
#if defined(__FreeBSD__)
		return (void*) uc->uc_mcontext.mc_eip;
#elif defined(__dietlibc__)
		return (void*) uc->uc_mcontext.eip;
#elif defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
#if __x86_64__
		return (void*) uc->uc_mcontext->__ss.__rip;
#elif __i386__
		return (void*) uc->uc_mcontext->__ss.__eip;
#else
		return (void*) uc->uc_mcontext->__ss.__srr0;
#endif
#elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)
#if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)
		return (void*) uc->uc_mcontext->__ss.__rip;
#else
		return (void*) uc->uc_mcontext->__ss.__eip;
#endif
#elif defined(__i386__)
		return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
#elif defined(__X86_64__) || defined(__x86_64__)
		return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
#elif defined(__ia64__) /* Linux IA64 */
		return (void*) uc->uc_mcontext.sc_ip;
#else
		return NULL;
#endif
	}
};
