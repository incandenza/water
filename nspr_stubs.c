/*
 * Minimal NSPR stub implementations for water JVM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "dist/Darwin25.1.0_x86_64_DBG.OBJ/include/nspr.h"

/* Monitor stubs */
PRStatus PR_EnterMonitor(PRMonitor *mon) {
    return PR_SUCCESS;
}

PRStatus PR_ExitMonitor(PRMonitor *mon) {
    return PR_SUCCESS;
}

PRMonitor *PR_NewMonitor(void) {
    return malloc(sizeof(PRMonitor));
}

/* File I/O stubs */
PRStatus PR_GetFileInfo(const char *name, PRFileInfo *info) {
    struct stat st;
    if (stat(name, &st) != 0) {
        return PR_FAILURE;
    }
    
    if (S_ISREG(st.st_mode)) {
        info->type = PR_FILE_FILE;
    } else if (S_ISDIR(st.st_mode)) {
        info->type = PR_FILE_DIRECTORY;
    } else {
        info->type = PR_FILE_OTHER;
    }
    
    info->size = st.st_size;
    info->creationTime = st.st_ctime * 1000000LL;
    info->modifyTime = st.st_mtime * 1000000LL;
    
    return PR_SUCCESS;
}

PRFileDesc *PR_Open(const char *name, PRInt32 flags, PRInt32 mode) {
    int fd = open(name, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    PRFileDesc *desc = malloc(sizeof(PRFileDesc));
    desc->fd = fd;
    return desc;
}

PRInt32 PR_Read(PRFileDesc *fd, void *buf, PRInt32 amount) {
    if (!fd) return -1;
    return read(fd->fd, buf, amount);
}

PRStatus PR_Close(PRFileDesc *fd) {
    if (!fd) return PR_FAILURE;
    int result = close(fd->fd);
    free(fd);
    return (result == 0) ? PR_SUCCESS : PR_FAILURE;
}

PRThread *PR_CurrentThread(void) {
    PRThread *thread = malloc(sizeof(PRThread));
    return thread;
}

PRStatus PR_Wait(PRMonitor *mon, PRIntervalTime timeout) {
    return PR_SUCCESS;
}

PRStatus PR_Notify(PRMonitor *mon) {
    return PR_SUCCESS;
}

PRInt32 PR_GetErrorTextLength(void) {
    return 0;
}

PRInt32 PR_GetErrorText(char *text) {
    return 0;
}