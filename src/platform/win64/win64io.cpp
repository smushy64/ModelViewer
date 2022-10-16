#ifdef WINDOWS

// Windows-specific IO functions

#include "platform/io.hpp"
#include "debug.hpp"
#include <windows.h>
#include <string>
#include "utils.hpp"

Platform::File Platform::LoadFile( const char* filePath ) {
    File result     = {};
    result.filePath = filePath;
    result.size     = 0;
    result.contents = nullptr;

    std::wstring wfilePath = Utils::CStringToWString( filePath );
    if( wfilePath.empty() ) {
        return result;
    }

    HANDLE fileHandle = CreateFile(
        wfilePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0, 0
    );

    if(fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if( GetFileSizeEx( fileHandle, &fileSize ) == TRUE ) {
            result.size     = (usize)fileSize.QuadPart;
            result.contents = malloc( result.size );

            if( result.contents ) {
                DWORD bytesRead;
                bool readResult = ReadFile(
                    fileHandle,
                    result.contents,
                    result.size,
                    &bytesRead,
                    NULL
                ) && bytesRead == result.size;

                if(!readResult) {
                    LOG_ERROR("Windows x64 > Failed to read file \"%s\"!", filePath);
                    FreeFile( result );
                    result.size = 0;
                }
            }
        }
        CloseHandle( fileHandle );
    } else {
        LOG_ERROR("Windows x64 > Failed to create file handle!");
    }

    return result;
}

#endif