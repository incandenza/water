#include "jni.h"

void Java_JNIClass_foo1(JNIEnv *env, jclass class)
{
    jmethodID foo2_id;

    fprintf(stderr, "hello from foo1().  calling foo2()...\n");
    foo2_id = (*env)->GetStaticMethodID(env, class, "foo2", "()V");
    if(!foo2_id) {
	fprintf(stderr, "couldn't find foo2().\n");
	return;
    }

    (*env)->CallStaticVoidMethod(env, class, foo2_id);
}

#if 0
/* japhar is looking for this one for some reason */
void Java_JNIClass_foo1_stub(JNIEnv *env, jclass class)
{
    jmethodID foo2_id;

    fprintf(stderr, "hello from foo1_stub().  calling foo2()...\n");
    foo2_id = (*env)->GetStaticMethodID(env, class, "foo2", "()V");
    if(!foo2_id) {
	fprintf(stderr, "couldn't find foo2().\n");
	return;
    }

    (*env)->CallStaticVoidMethod(env, class, foo2_id);
}
#endif
