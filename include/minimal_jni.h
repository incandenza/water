#ifndef MINIMAL_JNI_H
#define MINIMAL_JNI_H

typedef void* jobject;
typedef void* jclass;
typedef void* JNIEnv;
typedef int jint;
typedef int jboolean;

struct JavaVM_ {
    void* functions;
};

typedef struct JavaVM_ JavaVM;

#endif
