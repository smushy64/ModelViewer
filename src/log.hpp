/**
 * Description:  Simple Logging implementation
 *               Call INIT_CONSOLE() before anything else
 *               String formatting utilizes C-style formatting
 *               Includes stdio header
 *               MSVC: Includes <windows.h>
 *               IMPORTANT: MSVC: only outputs to Visual Studio debug output window!
 * Author:       Alicia Amarilla (smushy)
 * File Created: November 05, 2022
*/

#ifndef SMUSHY_DEBUG_LOGGING
#define SMUSHY_DEBUG_LOGGING 1

/// log nothing
#define LOG_LEVEL_NONE 0
/// log only errors
#define LOG_LEVEL_ERROR 1
/// log warnings and errors
#define LOG_LEVEL_WARN 2
/// log everything including info and debug messages
#define LOG_LEVEL_INFO 3

#if defined(DEBUG) || defined(_DEBUG)
    #include <stdio.h>

    #if __GNUG__
        #pragma GCC diagnostic ignored "-Wunused-variable"
    #endif

    /// @brief Current debug log level
    static inline int __DEBUG_LOG_LEVEL = LOG_LEVEL_INFO;
    /// @brief set logging level
    #define SET_LOG_LEVEL(level) __DEBUG_LOG_LEVEL = level
    /// @brief compare level against current log level
    #define LOG_LEVEL_VALID(level) __DEBUG_LOG_LEVEL >= level

    #ifdef _MSC_VER
        #include <windows.h>
        void winprintf( const char* format, ... ) {
            char buff[1024];
            va_list argptr;
            __crt_va_start(argptr, format);
            vsprintf_s( buff, format, argptr );
            __crt_va_end(argptr);
            
            OutputDebugStringA( buff );
        }
        #define printf winprintf
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
    #else
        /// @brief Set console text color to black
        #define CONSOLE_COLOR_BLACK() printf("\033[1;30m")
        /// @brief Set console text color to red
        #define CONSOLE_COLOR_RED() printf("\033[1;31m")
        /// @brief Set console text color to green
        #define CONSOLE_COLOR_GREEN() printf("\033[1;32m")
        /// @brief Set console text color to yellow
        #define CONSOLE_COLOR_YELLOW() printf("\033[1;33m")
        /// @brief Set console text color to magenta
        #define CONSOLE_COLOR_MAGENTA() printf("\033[1;35m")
        /// @brief Set console text color to cyan
        #define CONSOLE_COLOR_CYAN() printf("\033[1;36m")
        /// @brief Set console text color to white
        #define CONSOLE_COLOR_WHITE() printf("\033[1;37m")
        /// @brief Set console text color to blue
        #define CONSOLE_COLOR_BLUE()  printf("\033[1;34m")
        /// @brief Reset console text color
        #define CONSOLE_COLOR_RESET() printf("\033[1;0m")
        /// @brief Set console text color to default color
        #define CONSOLE_COLOR_DEFAULT() printf("\033[1;39m")
    #endif

    /// @brief Print debug-level log
    #define LOG_DEBUG(...)  \
        do{if(LOG_LEVEL_VALID(LOG_LEVEL_INFO)) {\
            CONSOLE_COLOR_BLUE();\
            printf( "[DEBUG] " );\
            CONSOLE_COLOR_RESET();\
            printf(__VA_ARGS__);\
            printf("\n");\
        }}  while(0)

    /// @brief Print info-level log
    #define LOG_INFO(...)  \
        do{if(LOG_LEVEL_VALID(LOG_LEVEL_INFO)) {\
            CONSOLE_COLOR_WHITE();\
            printf( "[INFO] " );\
            CONSOLE_COLOR_RESET();\
            printf(__VA_ARGS__);\
            printf("\n");\
        }}  while(0)

    /// @brief Print warning-level log
    #define LOG_WARN(...) \
        do{if(LOG_LEVEL_VALID(LOG_LEVEL_WARN)) {\
            CONSOLE_COLOR_YELLOW();\
            printf( "[WARNING] " );\
            CONSOLE_COLOR_RESET();\
            printf(__VA_ARGS__);\
            printf("\n");\
        }}  while(0)

    /// @brief Print error-level log
    #define LOG_ERROR(...)  \
        do{if(LOG_LEVEL_VALID(LOG_LEVEL_ERROR)) {\
            CONSOLE_COLOR_RED();\
            printf( "[ERROR | %s(%d)] ", __FILE__, __LINE__ );\
            CONSOLE_COLOR_RESET();\
            printf(__VA_ARGS__);\
            printf("\n");\
        }}  while(0)
    
    /// @brief Assert that given condition returns true. If condition returns false, program is crashed
    #define DEBUG_ASSERT(condition) do{ if(!(condition)){ *(int*)0 = 0;} } while(0)
    /// @brief Assert that given condition returns true. If condition returns false, program is crashed after printing log message.
    #define DEBUG_ASSERT_LOG(condition, ...) \
        do{if( !(condition) ) {\
            CONSOLE_COLOR_RED();\
            printf( "[ASSERTION FAILED | %s(%d)] ", __FILE__, __LINE__ );\
            CONSOLE_COLOR_RESET();\
            printf(__VA_ARGS__);\
            printf("\n");\
            DEBUG_ASSERT(false);\
        }} while(0)\

    /// @brief Crash the program and print unimplemented feature description.
    #define DEBUG_UNIMPLEMENTED(...) \
        CONSOLE_COLOR_RED();\
        printf( "[UNIMPLEMENTED ERROR | %s(%d)] ", __FILE__, __LINE__ );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
        DEBUG_ASSERT(false)

    /// @brief Crash the program and print reason for crash.
    #define PANIC(...) \
        CONSOLE_COLOR_RED();\
        printf( "[PANIC ERROR | %s(%d)] ", __FILE__, __LINE__ );\
        CONSOLE_COLOR_RESET();\
        printf(__VA_ARGS__);\
        printf("\n");\
        DEBUG_ASSERT(false)

    #ifdef _MSC_VER
        #define INIT_CONSOLE() do {\
            static HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );\
            DWORD dwMode = 0;\
            GetConsoleMode( hConsole, &dwMode );\
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;\
            SetConsoleMode( hConsole, dwMode );\
            LOG_INFO("Output debug console initialized!");\
        } while(0)
    #else
        #ifdef WINDOWS
            #define INIT_CONSOLE() do{\
                static HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );\
                DWORD dwMode = 0;\
                GetConsoleMode( hConsole, &dwMode );\
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;\
                SetConsoleMode( hConsole, dwMode );\
                LOG_INFO("Windows x64 > Debug Console initialized");\
            } while(0)
        #else
            #define INIT_CONSOLE() LOG_INFO("Console initialized!")
        #endif
    #endif

#else
    #define INIT_CONSOLE()
    #define SET_LOG_LEVEL(level)
    #define LOG_LEVEL_VALID(level)
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
    #define DEBUG_UNIMPLEMENTED(...)
    #define PANIC(...)
    #define DEBUG_ASSERT(condition)
    #define DEBUG_ASSERT_LOG(condition, ...)
#endif // if debug

#endif // header guard