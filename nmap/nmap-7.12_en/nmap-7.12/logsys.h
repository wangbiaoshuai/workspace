#ifndef _LOGSYS_H
#define _LOGSYS_H

/////////////////////////////////////////////////////////////
//

#ifdef  LOG_PRINTF

#define log_debug(fmt, ...) do{ fprintf(stderr, "NAC DEBUG> "fmt"\n", ##__VA_ARGS__); }while(0)
#define log_info(fmt, ...)  do{ fprintf(stderr, "NAC +INFO> "fmt"\n", ##__VA_ARGS__); }while(0)
#define log_warn(fmt, ...)  do{ fprintf(stderr, "NAC +WARN> "fmt"\n", ##__VA_ARGS__); }while(0)
#define log_error(fmt, ...) do{ fprintf(stderr, "NAC ERROR> "fmt"\n", ##__VA_ARGS__); }while(0)

#else

#include <syslog.h>
#define log_debug(fmt, ...) do{ syslog(LOG_DEBUG|LOG_USER,   "NAC DEBUG> "fmt"\n", ##__VA_ARGS__); }while(0)
#define log_info(fmt, ...)  do{ syslog(LOG_INFO|LOG_USER,    "NAC +INFO> "fmt"\n", ##__VA_ARGS__); }while(0)
#define log_warn(fmt, ...)  do{ syslog(LOG_WARNING|LOG_USER, "NAC +WARN> "fmt"\n", ##__VA_ARGS__); }while(0)
#define log_error(fmt, ...) do{ syslog(LOG_ERR|LOG_USER,     "NAC ERROR> "fmt"\n", ##__VA_ARGS__); }while(0)

#endif

/////////////////////////////////////////////////////////////
//

#ifdef  LOG_LEVEL_INFO

#   define log_debug(fmt, ...)

#elif   LOG_LEVEL_WARN

#   define log_debug(fmt, ...)
#   define log_info(fmt, ...)

#elif   LOG_LEVEL_ERROR

#   define log_debug(fmt, ...)
#   define log_info(fmt, ...)
#   define log_warn(fmt, ...)

#endif

#endif
