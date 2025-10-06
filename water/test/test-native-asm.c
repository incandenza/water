#include "jni.h"
#include "native-asm.h"
#include "stack.h"

int foo(JNIEnv *env, void *class_or_object, int a, int b)
{
    printf("in foo().\n");
    printf("got a is %d, b is %d\n", a, b);

    return 99;
}

double retdouble(void) { return 5.0; }
float retfloat(void) { return 5.0; }
void *retvoidptr(void) { return (void *)0x456; }
int retint(void) { return 5; }
long retlong(void) { return 5; }
long long retlonglong(void) { return 5; }
char retchar(void) { return 5; }

int main()
{
    char fake_stack[255];
    char *stack_ptr;
    jint i;

    stack_ptr = &fake_stack[0];
    i = 10;
    opstack_push_int(stack_ptr, &i);
    i = 20;
    opstack_push_int(stack_ptr, &i);

    water_asmCallNative((void *)0x123, (void *)0x456,
			foo,
			&fake_stack[0], 8, &i, JNI_INT);
    printf("called foo() succesfully.  return value was %d.\n", i);
}
