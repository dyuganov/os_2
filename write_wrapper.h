#include <stddef.h>

#define WRITE_SUCCESS (0)
#define WRITE_FAIL (1)
#define ERROR_MSG_BUF_SIZE (256)
#define WRITE_ERROR_CODE (-1)

int isWriteWrapperError(int writeWrapperResult){
    if(WRITE_FAIL == writeWrapperResult){
        return 1;
    }
    return 0;
}

int writeWrapper(const int descriptor, const void* buffer, const size_t count){
    if(NULL == buffer){
        return WRITE_FAIL;
    }
    int writeResult = write(descriptor, buffer, count);
    /*
    if(WRITE_ERROR_CODE == writeResult){
        char message[ERROR_MSG_BUF_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "%s: Error: %s\n", argv[0], message);
    }
    */
    if(WRITE_ERROR_CODE == writeResult){
        perror("Write error");
        return WRITE_FAIL;
    }
    return WRITE_SUCCESS;
}
