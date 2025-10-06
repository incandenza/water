#include <jni.h>

main() {
    JNIEnv *env;
    JavaVM *jvm;
    JDK1_1InitArgs vm_args;
    jint res;
    jclass cls;
    jmethodID mid;
    jstring jstr;
    jobjectArray args;


    /* IMPORTANT: specify vm_args version # if you use JDK1.1.2 and beyond */
    vm_args.version = 0x00010001;

    JNI_GetDefaultJavaVMInitArgs(&vm_args);

#if defined(DEBUG_djm) && 0
    printf("default VM arguments (1.1) are:\n");
    printf("checkSource: %d\n", vm_args.checkSource);
    printf("nativeStackSize: %d\n", vm_args.nativeStackSize);
    printf("javaStackSize: %d\n", vm_args.javaStackSize);
    printf("minHeapSize: %d\n", vm_args.minHeapSize);
    printf("maxHeapSize: %d\n", vm_args.maxHeapSize);
    printf("verifyMode: %d\n", vm_args.verifyMode);
    printf("classpath: '%s'\n", vm_args.classpath);
    printf("vfprintf function: %p\n", vm_args.vfprintf);
    printf("exit function: %p\n", vm_args.exit);
    printf("abort function: %p\n", vm_args.abort);
    printf("enableClassGC: %d\n", vm_args.enableClassGC);
    printf("enableVerboseGC: %d\n", vm_args.enableVerboseGC);
    printf("disableAsyncGC: %d\n", vm_args.disableAsyncGC);
#endif

    res = JNI_CreateJavaVM(&jvm,&env,&vm_args);
    if (res < 0) {
        fprintf(stderr, "Can't create Java VM\n");
        exit(1);
    }

    cls = (*env)->FindClass(env, "Prog");
    if (cls == 0) {
        fprintf(stderr, "Can't find Prog class\n");
        exit(1);
    }
 
    mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
    if (mid == 0) {
        fprintf(stderr, "Can't find Prog.main\n");
        exit(1);
    }

#if 0
    jstr = (*env)->NewStringUTF(env, " from C!");
    if (jstr == 0) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    args = (*env)->NewObjectArray(env, 1, 
                        (*env)->FindClass(env, "java/lang/String"), jstr);
    if (args == 0) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
#endif
    (*env)->CallStaticVoidMethod(env, cls, mid, args);

    (*jvm)->DestroyJavaVM(jvm);
}

