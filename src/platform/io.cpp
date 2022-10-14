#include "io.hpp"
#include <cstdlib>

// Platform-independent IO functions

void Platform::FreeFile( File file ) {
    if(file.contents) {
        free( file.contents );
    }
}
