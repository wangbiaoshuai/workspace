#include "jni.h"
#include "HelloWorld.h"
#include <stdio.h>

JNIEXPORT void JNICALL Java_HelloWorld_displayHelloWorld(JNIEnv *, jobject)
{
    printf("this is a sample jni.\n");
    return;
}
