#include "jni.h"
#include "de_applejuicenet_nativeclasses_JNISubNetMask.h"
#include <cstdlib>
#include <iostream>

using namespace std;

#include <winsock2.h>
#include <ws2tcpip.h>

int count = 0;
const char* net[1000];

void getips(){
     int num = 0;
     char *tmp = NULL;
     
       WSADATA WinsockData;
    if (WSAStartup(MAKEWORD(2, 2), &WinsockData) != 0) {
        cerr << "Failed to find Winsock 2.2!" << endl;
    }
    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sd == SOCKET_ERROR) {
        cerr << "Failed to get a socket. Error " << WSAGetLastError() <<
            endl;
    }

    INTERFACE_INFO InterfaceList[20];
    unsigned long nBytesReturned;
    if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
			sizeof(InterfaceList), &nBytesReturned, 0, 0) == SOCKET_ERROR) {
        cerr << "Failed calling WSAIoctl: error " << WSAGetLastError() <<
				endl;

    }
    int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
    for (int i = 0; i < nNumInterfaces; ++i) {
        u_long nFlags = InterfaceList[i].iiFlags;
        if (nFlags & IFF_UP){
            sockaddr_in *pAddress;
            pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);
            
            tmp = (char *)malloc(100*sizeof(char));
            char buffer [10];
            sprintf (buffer, "net%d", i);
            strcpy (tmp, buffer);
            net[num++] = tmp;
            
            
            tmp = (char *)malloc(100*sizeof(char));
            strcpy (tmp,inet_ntoa(pAddress->sin_addr));
            net[num++] = tmp;
            
            pAddress = (sockaddr_in *) & (InterfaceList[i].iiNetmask);
            tmp = (char *)malloc(100*sizeof(char));
            strcpy (tmp,inet_ntoa(pAddress->sin_addr));
            net[num++] = tmp;


           }

    }
    count = num;

    WSACleanup();
     }

extern "C" JNIEXPORT jobjectArray JNICALL Java_de_applejuicenet_nativeclasses_JNISubNetMask_getSubNetMask(JNIEnv *env, jobject jobj){
       getips();
   jclass stringClass = env->FindClass( "java/lang/String");
    jobjectArray row = env->NewObjectArray( count, stringClass, 0);
    jsize i;

    for (i = 0; i < count; ++i) {
        env->SetObjectArrayElement(row, i, env->NewStringUTF( net[i]));
    }
    
    return row;
}

