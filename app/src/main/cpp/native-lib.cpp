#include <jni.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "util.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_xl_mmapclient_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//客户端
extern "C" {
char *ptr = nullptr;
bool isOpen = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xl_mmapclient_MainActivity_mmapOpen(JNIEnv *env, jobject thiz, jstring path) {
    const char *file_path = env->GetStringUTFChars(path, 0);
    //打开本地磁盘中的文件(如果没有就创建一个), 获取fd,0644是可读写的意思
    int fd = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        isOpen = false;
        LOGE("open_fail");
        perror("open_erroe");
    }
//   改变文件大小，否则大小对应不起来就会报错
    ftruncate(fd, 100);
    ptr = (char *) mmap(nullptr, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        LOGE("mmap_error");
        isOpen = false;
        perror("mmap_error");
    }
    close(fd);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_xl_mmapclient_MainActivity_mmapClose(JNIEnv *env, jobject thiz) {
    if (ptr != nullptr && isOpen) {
//       释放内存映射区域
        int ret = munmap(ptr, 100);
        if (ret == -1) {
            LOGE("mmap_error");
            perror("munmap error");
        }
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_xl_mmapclient_MainActivity_mmapWrite(JNIEnv *env, jobject thiz, jstring content) {
    if (ptr != nullptr && isOpen) {
        const char *c_content = env->GetStringUTFChars(content, 0);
//     数据拷贝到映射区域
        strcpy(ptr, c_content);
    }
}




//服务端
extern "C"
JNIEXPORT void JNICALL
Java_com_xl_mmapclient_MainActivity_mmapSetEmpty(JNIEnv *env, jobject thiz) {
    if (ptr != NULL) {
        // 将共享映射区置空
        memset(ptr, 0, 100);
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_xl_mmapclient_MainActivity_observe(JNIEnv *env, jobject thiz, jstring default_val) {
    if(ptr != NULL){
        return env->NewStringUTF(ptr);
    }
    return default_val;
}