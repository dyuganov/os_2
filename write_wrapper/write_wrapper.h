#define WRITE_SUCCESS (0)
#define WRITE_FAIL (1)
#define ERROR_MSG_BUF_SIZE (256)
#define WRITE_ERROR_CODE (-1)

int isWriteWrapperError(int writeWrapperResult);
int writeWrapper(const int descriptor, const void* buffer, const size_t count);
