#ifdef DEBUG
    #include <stdio.h>

    #define CONSOLE_COLOR_BLACK() printf("\033[1;30m")
    #define CONSOLE_COLOR_RED() printf("\033[1;31m")
    #define CONSOLE_COLOR_GREEN() printf("\033[1;32m")
    #define CONSOLE_COLOR_YELLOW() printf("\033[1;33m")
    #define CONSOLE_COLOR_MAGENTA() printf("\033[1;35m")
    #define CONSOLE_COLOR_CYAN() printf("\033[1;36m")
    #define CONSOLE_COLOR_WHITE() printf("\033[1;37m")
    #define CONSOLE_COLOR_BLUE()  printf("\033[1;34m")
    #define CONSOLE_COLOR_RESET() printf("\033[1;0m")
    #define CONSOLE_COLOR_DEFAULT() printf("\033[1;39m")

    #define LOG_DEBUG(...)  ({\
        CONSOLE_COLOR_BLUE();\
        printf( "DEBUG " );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
    })
    #define LOG_INFO(...)  ({\
        CONSOLE_COLOR_WHITE();\
        printf( "INFO " );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
    })
    #define LOG_WARN(...) ({\
        CONSOLE_COLOR_YELLOW();\
        printf( "WARNING " );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
    })
    #define LOG_ERROR(...)  ({\
        CONSOLE_COLOR_RED();\
        printf( "ERROR " );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
    })
    #define DEBUG_ASSERT(expr) { if(!expr){ *(int*)0 = 0;} }
    #define DEBUG_ASSERT_LOG(expr, ...) ({\
        CONSOLE_COLOR_RED();\
        printf( "ASSERTION FAILED " );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
        DEBUG_ASSERT(expr);\
    })
    #define LOG_UNIMPLEMENTED(...) ({\
        CONSOLE_COLOR_RED();\
        printf( "UNIMPLEMENTED ERROR " );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
        DEBUG_ASSERT(false);\
    })
#else
    #define CONSOLE_COLOR_BLACK()
    #define CONSOLE_COLOR_RED()
    #define CONSOLE_COLOR_GREEN()
    #define CONSOLE_COLOR_YELLOW()
    #define CONSOLE_COLOR_MAGENTA()
    #define CONSOLE_COLOR_CYAN()
    #define CONSOLE_COLOR_WHITE()
    #define CONSOLE_COLOR_BLUE()
    #define CONSOLE_COLOR_RESET()
    #define CONSOLE_COLOR_DEFAULT()
    #define LOG_DEBUG(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)
    #define LOG_UNIMPLEMENTED(...)
    #define DEBUG_ASSERT(expr)
    #define DEBUG_ASSERT_LOG(expr, ...)
#endif
