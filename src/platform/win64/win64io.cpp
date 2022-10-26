#ifdef WINDOWS

// Windows-specific IO functions

#include "platform/io.hpp"
#include "debug.hpp"
#include "win64global.hpp"
#include <string>
#include "utils.hpp"

Platform::File Platform::LoadFile( const char* filePath ) {
    File result     = {};
    result.filePath = filePath;
    result.size     = 0;
    result.contents = nullptr;

    std::wstring wfilePath = Utils::CStringToWString( filePath );
    if( wfilePath.empty() ) {
        LOG_WARN("Windows x64 > File path is empty, loaded nothing");
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
                }
            }
        }
        CloseHandle( fileHandle );
    } else {
        LOG_ERROR("Windows x64 > Failed to create file handle!");
    }

    return result;
}

Platform::File Platform::LoadFilePopup() {
    LOG_INFO("Windows x64 > Loading file from pop up . . .");
    File result = {};

    const usize MAX_FILE_NAME = 256;

    OPENFILENAME openFileName = {};
    // buffer for file name
    wchar_t szFile[MAX_FILE_NAME];

    HANDLE fileHandle;

    openFileName.lStructSize     = sizeof(openFileName);
    openFileName.hwndOwner       = WINDOW_HANDLE;
    openFileName.lpstrFile       = szFile;
    openFileName.lpstrFile[0]    = '\0';
    openFileName.nMaxFile        = sizeof(szFile);
    openFileName.lpstrFilter     = L"All\0*.*\0Text\0*.TXT\0";
    openFileName.nFilterIndex    = 1;
    openFileName.lpstrFileTitle  = NULL;
    openFileName.nMaxFileTitle   = 0;
    openFileName.lpstrInitialDir = NULL;
    openFileName.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if( GetOpenFileName( &openFileName ) == FALSE ) {
        LOG_ERROR("Windows x64 > Failed to Open File Name!");
        return result;
    }

    fileHandle = CreateFile(
        openFileName.lpstrFile,
        GENERIC_READ,
        FILE_SHARE_READ, // dwShareMode
        NULL, // lpSecurityAttributes
        OPEN_EXISTING, // dwCreationDisposition
        0, 0
    );

    if(fileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        if( GetFileSizeEx( fileHandle, &fileSize ) == TRUE ) {
            result.size     = (usize)fileSize.QuadPart;
            result.contents = malloc( result.size );
            result.filePath = Utils::WStringToString( openFileName.lpstrFile );

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
                    LOG_ERROR("Windows x64 > Failed to read file \"%s\"!", result.filePath.c_str());
                    FreeFile( result );
                }
            }
        }
        CloseHandle( fileHandle );
    } else {
        LOG_ERROR("Windows x64 > Failed to create file handle!");
        return result;
    }

    LOG_INFO("Windows x64 > File loaded successfully!");
    return result;
}

#endif