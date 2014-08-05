#ifndef __BUILD_FLAGS_H
#define __BUILD_FLAGS_H


#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL         0
#endif /* DEBUG_LEVEL */

#ifndef DEBUG_ERRORS
#define DEBUG_ERRORS        1
#endif

#define DEBUG_GLOBAL_LEVEL  (REPORT_LEVEL_WARNING + DEBUG_LEVEL)
#define ERROR_GLOBAL_LEVEL  (REPORT_LEVEL_MAJOR + DEBUG_LEVEL)
#define STACK_OVERFLOW_DETECTION 1

#if (DEBUG_LEVEL > 0)
#define DEBUG
#define DEBUG_FSL_OS_MALLOC

#else
#define DISABLE_SANITY_CHECKS
#define DISABLE_ASSERTIONS
#endif /* (DEBUG_LEVEL > 0) */

#define LS2085A
#define AIOP
#define SOC_PPC_CORE
/*#define SYS_SMP_SUPPORT*/
#define SIMULATOR
#define DEBUG_NO_MC


#endif /* __BUILD_FLAGS_H */
