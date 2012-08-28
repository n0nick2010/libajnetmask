
#include "jni.h"
#include "de_applejuicenet_nativeclasses_JNISubNetMask.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static jobjectArray make_row(JNIEnv *env, jsize count, const char* elements[])
{
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray row = (*env)->NewObjectArray(env, count, stringClass, 0);
    jsize i;

    for (i = 0; i < count; ++i) {
        (*env)->SetObjectArrayElement(env, row, i, (*env)->NewStringUTF(env, elements[i]));
    }
    return row;
}


JNIEXPORT jobjectArray JNICALL Java_de_applejuicenet_nativeclasses_JNISubNetMask_getSubNetMask(JNIEnv *env, jobject jobj){
    struct ifaddrs *ifaddr, *ifa;
    int family, s ,s2;
    int i = 0;
    int count = 0;
    char host[NI_MAXHOST];
    char subnet[NI_MAXHOST];
    char *tmp = NULL;
    
    const char* net[1000];
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    /* Walk through linked list, maintaining head pointer so we
    can free list later */
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
           continue;
        
        family = ifa->ifa_addr->sa_family;
        
        if (family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            s2 = getnameinfo(ifa->ifa_netmask,sizeof(struct sockaddr_in), subnet, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0 || s2 != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            
            tmp = (char *)malloc(100*sizeof(char)); 
            strcpy (tmp,ifa->ifa_name);
            net[i++] = tmp;
            tmp = (char *)malloc(100*sizeof(char)); 
            strcpy (tmp,host);
            net[i++] = tmp;
            tmp = (char *)malloc(100*sizeof(char)); 
            strcpy (tmp,subnet);
            net[i++] = tmp;
            
        }
    }

    freeifaddrs(ifaddr);
    count = i;
    jobjectArray jnet = make_row(env, count, net);
    return jnet;
}

