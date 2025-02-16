#pragma once

#ifdef __cplusplus
#include <cstdbool>
#else
#include <stdbool.h>
#endif // __cplusplus

#include <SDL3/SDL.h>

#include "com_defs.h"

typedef struct StreamFile {
	SDL_RWops* f;
	size_t size;
} StreamFile;

A_EXTERN_C A_NO_DISCARD void* FS_ReadFile(const char* path);
A_EXTERN_C A_NO_DISCARD char* FS_ReadFileText(const char* path);

typedef enum SeekFrom {
	FS_SEEK_BEGIN,
	FS_SEEK_CUR,
	FS_SEEK_END
} SeekFrom;

typedef enum StreamMode {
	FS_STREAM_READ_EXISTING,
	FS_STREAM_WRITE_NEW,
	FS_STREAM_READ_WRITE_EXISTING,
	FS_STREAM_READ_WRITE_NEW,
	FS_STREAM_APPEND
} StreamMode;

A_EXTERN_C A_NO_DISCARD StreamFile FS_StreamFile(
	const char* path, SeekFrom from, StreamMode mode, size_t off
);

A_EXTERN_C long long FS_SeekStream (A_INOUT StreamFile* file, SeekFrom from, size_t off);
A_EXTERN_C void      FS_CloseStream(A_INOUT StreamFile* file);

A_EXTERN_C A_NO_DISCARD size_t FS_StreamPos(A_INOUT StreamFile* file);
A_EXTERN_C A_NO_DISCARD size_t FS_FileSize (A_INOUT StreamFile* file);

A_EXTERN_C A_NO_DISCARD bool FS_ReadStream (A_INOUT StreamFile* file,       void* dst, size_t count);
A_EXTERN_C              bool FS_WriteStream(A_INOUT StreamFile* file, const void* src, size_t count);

A_EXTERN_C bool FS_DeleteFile(const char* filename);
A_EXTERN_C bool FS_FileExists(const char* filename);
