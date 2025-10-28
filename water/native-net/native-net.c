/*
 * water
 * a Java virtual machine
 * 
 * Copyright (C) 1998-2025 Dan McGuirk <mcguirk@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "jni.h"
#include "objects.h"
#include "classes.h"
#include "exception.h"
#include <assert.h>
#include "nspr.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "byteops.h"

/* don't ask me why we have to allocate space for this */
static char pr_netdb_buf[PR_NETDB_BUF_SIZE];

static PRFileDesc *getFD(JNIEnv *env, jobject object);
static void storeFD(JNIEnv *env, jobject object, PRFileDesc *fd);
static int getIP(JNIEnv *env, jobject object);
static void storeNetAddr(JNIEnv *env, jobject addr_object, PRNetAddr *addr);
static jobject getAddress(JNIEnv *env, jobject socket_object);
static jint getLocalPort(JNIEnv *env, jobject socket_object);
static void storePort(JNIEnv *env, jobject socket_object, jint port);
/* ugh.  it's not my fault, dammit. */
static void storeLocalport(JNIEnv *env, jobject socket_object, jint port);
static void storeLocalPort(JNIEnv *env, jobject socket_object, jint port);

#if 0
/* not used by anything at the moment */
static jint getPort(JNIEnv *env, jobject socket_object);
#endif

jint Java_java_net_InetAddressImpl_getInetFamily(JNIEnv *env, 
						 jobject address_object)
{
    return PR_AF_INET;
}

jobject Java_java_net_InetAddressImpl_getLocalHostName(JNIEnv *env,
						       jobject address_object)
{
    PRHostEnt host_entry;
    PRNetAddr localhost_addr;

#ifdef DEBUG_djm
    fprintf(stderr, "in getLocalHostname...\n");
#endif

    if(PR_InitializeNetAddr(PR_IpAddrLoopback, 0, 
			    &localhost_addr) != PR_SUCCESS) {
#ifdef DEBUG_djm
	fprintf(stderr, "exception initializing localhost address.\n");
#endif
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_InitializedNetAddr failed");
	return 0;
    }
    if(PR_GetHostByAddr(&localhost_addr, pr_netdb_buf, 
			PR_NETDB_BUF_SIZE, &host_entry) != PR_SUCCESS) {
#ifdef DEBUG_djm
	fprintf(stderr, "exception looking up localhost address.\n");
#endif
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_GetHostByAddr for localhost failed");
	return 0;
    }

#ifdef DEBUG_djm
    fprintf(stderr, "I think the local hostname is '%s'.\n", 
	    host_entry.h_name);
#endif

    return water_newStringFromBytes(env, host_entry.h_name);
}

void Java_java_net_InetAddressImpl_makeAnyLocalAddress(JNIEnv *env,
						       jobject aimpl_object,
						       jobject addr_object)
{
    PRNetAddr any_addr;

    if(PR_InitializeNetAddr(PR_IpAddrAny, 0, &any_addr) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_InitializeNetAddr failed");
	return;
    }

    storeNetAddr(env, addr_object, &any_addr);
}

jobject 
Java_java_net_InetAddressImpl_lookupAllHostAddr(JNIEnv *env,
						jobject aimpl_object,
						jstring hostname_string)
{
    unsigned char *hostname;
    PRHostEnt host_entry;
    waterClass *array_class;
    waterObject *array, *inner_array;
    jint dimensions[2];
    int i, length;

#ifdef DEBUG_djm
    fprintf(stderr, "in lookupAllHostAddr...\n");
#endif
    /* this wants, as a result, an array of byte arrays specifying */
    /* the IP's for the host.  each byte array is of length 4,     */
    /* and contains the IP in big-endian (network) order.          */
    hostname = water_getStringBytes(env, hostname_string);
#ifdef DEBUG_djm
    fprintf(stderr, "hostname is '%s'...\n", hostname);
#endif
    if(!hostname) {
	return 0;
    }
    if(PR_GetHostByName(hostname, pr_netdb_buf, 
			PR_NETDB_BUF_SIZE, &host_entry) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), 
				 "java/net/UnknownHostException",
				 "PR_GetHostByName failed");
	return 0;
    }

    i = 0;
    while(host_entry.h_addr_list[i]) {	
	i++;
    }
    length = i;
#ifdef DEBUG_djm
    fprintf(stderr, "number of entries in the host_entry: %d\n", length);
#endif

    array_class = water_getArrayClassByName(env, "[[B", 0, CLASS_VERIFIED);
    assert(array_class);
    dimensions[0] = length;
    dimensions[1] = 4;
    array = water_newMultiArray(env, array_class, 2, dimensions);
    if(!array) {
	return 0;
    }

    for(i = 0; i < length; i++) {
	inner_array = (*env)->GetObjectArrayElement(env, array, i);
	assert(inner_array);
#ifdef DEBUG_djm
	fprintf(stderr, "I think one of the IP's is %x\n",
		*((int *)host_entry.h_addr_list[i]));
#endif
	(*env)->SetByteArrayRegion(env, inner_array, 0, 4,
				   (jbyte *)host_entry.h_addr_list[i]);
    }

    return array;
}

jobject Java_java_net_InetAddressImpl_getHostByAddr(JNIEnv *env,
						    jobject aimpl_object,
						    jint addr_int)
{
    PRNetAddr net_addr;
    PRHostEnt host_entry;

    net_addr.inet.family = PR_AF_INET;
    net_addr.inet.port = PR_htons(0);
    net_addr.inet.ip = PR_htonl(addr_int);

    if(PR_GetHostByAddr(&net_addr, pr_netdb_buf, 
			PR_NETDB_BUF_SIZE, &host_entry) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_GetHostByAddr failed");
	return 0;	
    }

    return water_newStringFromBytes(env, host_entry.h_name);
}

void Java_java_net_PlainSocketImpl_socketCreate(JNIEnv *env,
						jobject socket_object,
						jboolean thing)
{
    /* XXX I'm not sure what the boolean parameter is--it's always set */
    /* to 'true' in calls to this method.                              */
    PRFileDesc *fd;

    fd = PR_NewTCPSocket();
    if(fd == 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_NewTCPSocket failed");
	return;
    }
    
    /* XXX more problems for 64-bit machines here. */
    storeFD(env, socket_object, fd);
}

void Java_java_net_PlainSocketImpl_socketConnect(JNIEnv *env,
						 jobject socket_object,
						 jobject address_object,
						 jint port)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRNetAddr addr, local_addr;

    /* XXX violating the data abstraction */
    addr.inet.family = PR_AF_INET;
    addr.inet.port = PR_htons(port);
    addr.inet.ip = PR_htonl(getIP(env, address_object));

#ifdef DEBUG_djm
    fprintf(stderr, "trying to connect to IP %x, port %x\n", 
	    addr.inet.ip, addr.inet.port);
#endif

    if(PR_Connect(fd, &addr, PR_INTERVAL_NO_TIMEOUT) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Connect failed");
	return;
    }

    /* it's connected. */
    storePort(env, socket_object, port);

    /* store local port */
    if(PR_GetSockName(fd, &local_addr) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_GetSockName failed");
	return;
    }
	
    storeLocalport(env, socket_object, PR_ntohs(local_addr.inet.port));
}

void Java_java_net_PlainSocketImpl_socketBind(JNIEnv *env,
					      jobject socket_object,
					      jobject address_object,
					      jint port)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRNetAddr addr;

    /* XXX violating the data abstraction */
    addr.inet.family = PR_AF_INET;
    addr.inet.port = PR_htons(port);
    addr.inet.ip = PR_htonl(getIP(env, address_object));

    if(PR_Bind(fd, &addr) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Bind failed");
	return;
    }

    /* it's bound. */
    storeLocalport(env, socket_object, port);
}

void Java_java_net_PlainSocketImpl_socketListen(JNIEnv *env,
						jobject socket_object,
						jint backlog)
{
    PRFileDesc *fd = getFD(env, socket_object);

    if(PR_Listen(fd, backlog) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Listen failed");
	return;
    }

    /* we're listening. */
}

void Java_java_net_PlainSocketImpl_socketAccept(JNIEnv *env,
						jobject socket_object,
						jobject new_socket_object)
{
    PRFileDesc *server_fd = getFD(env, socket_object);
    PRFileDesc *connecting_fd;
    PRNetAddr connecting_address;
    jobject address_object;
    jint server_port;

    connecting_fd = 
	PR_Accept(server_fd, &connecting_address, PR_INTERVAL_NO_TIMEOUT);
    if(!connecting_fd) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Accept failed");
	return;	
    }

    address_object = getAddress(env, new_socket_object);
    storeFD(env, new_socket_object, connecting_fd);
    storeNetAddr(env, address_object, &connecting_address);

    /* store 'port' and 'localport' for the new socket.  'localport' is the */
    /* port the server socket was listening on, and 'port' is the port      */
    /* the connector connected from.                                        */
    server_port = getLocalPort(env, socket_object);
    storeLocalport(env, new_socket_object, server_port);
    storePort(env, new_socket_object, connecting_address.inet.port);
}

jint Java_java_net_PlainSocketImpl_socketAvailable(JNIEnv *env,
						   jobject socket_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
    jint result;

    result = PR_Available(fd);
    if(result < 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Available failed");
	return 0;
    }

    return result;
}

void Java_java_net_PlainSocketImpl_socketClose(JNIEnv *env, 
					       jobject socket_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
    
    if(PR_Close(fd) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Close failed");
	return;
    }
}

void Java_java_net_PlainSocketImpl_initProto(JNIEnv *env, jclass socket_class)
{
    /* yeah, I'll init your proto, buddy, right here. */
}

void Java_java_net_PlainSocketImpl_socketSetOption(JNIEnv *env,
						   jobject socket_object,
						   jint option,
						   jboolean toggle,
						   jobject value)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRSocketOptionData option_data;
    jmethodID intvalue_method;

    /* I got these option numbers from reading the disassembly of */
    /* Sun's bytecode.                                            */
    if(option == 1) {
	/* NODELAY option. */
	/* whether it should be on is set in the 'toggle' parameter. */
	option_data.option = PR_SockOpt_NoDelay;
	option_data.value.no_delay = (toggle == JNI_TRUE) ? PR_TRUE : PR_FALSE;
    } else if(option == 128) {
	/* SO_LINGER option */
	/* 'toggle' tells us whether it should be on or off.           */
	/* if it's going on, the 'value' object is a java.lang.Integer */
	/* giving the value to set it to.                              */
	option_data.option = PR_SockOpt_Linger;
	if(toggle == JNI_TRUE) {
	    /* ugh, 'polarity', what a terrible name. */
	    /* I'll assume a true 'polarity' means the option is enabled. */
	    option_data.value.linger.polarity = PR_TRUE;

	    intvalue_method = 
		(*env)->GetMethodID(env, 
				    (*env)->GetObjectClass(env, value),
				    "intValue", "()I");
	    assert(intvalue_method);
	    option_data.value.linger.linger =
		(*env)->CallIntMethod(env, value, intvalue_method);
	} else {
	    option_data.value.linger.polarity = PR_FALSE;
	}
    } else {
	assert(0);
	return;
    }

    if(PR_SetSocketOption(fd, &option_data) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), 
				 "java/net/SocketException",
				 "PR_SetSocketOption failed");
	return;
    }
}

jint Java_java_net_PlainSocketImpl_socketGetOption(JNIEnv *env,
						   jobject socket_object,
						   jint option)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRSocketOptionData option_data;

    if(option == 1) {
	option_data.option = PR_SockOpt_NoDelay;
    } else if(option == 128) {
	option_data.option = PR_SockOpt_Linger;
    }

    if(PR_GetSocketOption(fd, &option_data) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), 
				 "java/net/SocketException",
				 "PR_GetSocketOption failed");
	return 0;
    }

    /* these need to return a -1 to represent 'false' */
    if(option == 1) {
	return (option_data.value.no_delay == PR_TRUE) ? 1 : -1;
    } else if(option == 128) {
	if(option_data.value.linger.polarity == PR_FALSE) {
	    return -1;
	} else {
	    return option_data.value.linger.linger;
	}
    }

    /* shouldn't get here */
    assert(0);
    return -1;
}

void Java_java_net_PlainDatagramSocketImpl_bind(JNIEnv *env,
						jobject socket_object,
						jint port,
						jobject address_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRNetAddr addr;

    /* XXX violating the data abstraction */
    addr.inet.family = PR_AF_INET;
    addr.inet.port = PR_htons(port);
    addr.inet.ip = PR_htonl(getIP(env, address_object));

    if(PR_Bind(fd, &addr) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Bind failed");
	return;
    }

    /* it's bound. */
    storeLocalPort(env, socket_object, port);
}

void Java_java_net_PlainDatagramSocketImpl_datagramSocketCreate(JNIEnv *env,
							jobject socket_object)
{
    PRFileDesc *fd;

    fd = PR_NewUDPSocket();
    if(fd == 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_NewUDPSocket failed");
	return;
    }
    
    /* XXX more problems for 64-bit machines here. */
    storeFD(env, socket_object, fd);    
}

void Java_java_net_PlainDatagramSocketImpl_datagramSocketClose(JNIEnv *env,
						       jobject socket_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
    
    if(PR_Close(fd) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/net/SocketException",
				 "PR_Close failed");
	return;
    }
}

void Java_java_net_PlainDatagramSocketImpl_send(JNIEnv *env,
						jobject socket_object,
						jobject packet_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
    jmethodID getdata_method, getaddress_method, getport_method;
    jobject byte_array, address_object;
    int num_bytes, bytes_sent;
    jint port;
    PRNetAddr remote_addr;

    getdata_method = 
	(*env)->GetMethodID(env,
			    (*env)->GetObjectClass(env, packet_object),
			    "getData", "()[B");
    assert(getdata_method);
    getaddress_method = 
	(*env)->GetMethodID(env,
			    (*env)->GetObjectClass(env, packet_object),
			    "getAddress", "()Ljava/net/InetAddress;");
    assert(getaddress_method);
    getport_method = 
	(*env)->GetMethodID(env,
			    (*env)->GetObjectClass(env, packet_object),
			    "getPort", "()I");
    assert(getport_method);

    byte_array = (*env)->CallObjectMethod(env, packet_object, getdata_method);
    assert(byte_array);

    GET_INT_MACHINE(num_bytes, ARRAY_LENGTH_PTR(byte_array), 0);

    address_object = 
	(*env)->CallObjectMethod(env, packet_object, getaddress_method);
    assert(address_object);

    port = (*env)->CallIntMethod(env, packet_object, getport_method);

    remote_addr.inet.family = PR_AF_INET;
    remote_addr.inet.port = PR_htons(port);
    remote_addr.inet.ip = PR_htonl(getIP(env, address_object));
    
    bytes_sent =
	PR_SendTo(fd, ARRAY_ELEMENT_PTR(byte_array, 0), num_bytes, 0,
		  &remote_addr, PR_INTERVAL_NO_TIMEOUT);
    if(bytes_sent < 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/io/IOException",
				 "PR_SendTo failed");
	return;
    }
    /* XXX are we supposed to send again if the whole packet wasn't sent? */
    /* seems unlikely.                                                    */
}

void Java_java_net_PlainDatagramSocketImpl_receive(JNIEnv *env,
						   jobject socket_object,
						   jobject packet_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
#define RECEIVE_BUFFER_SIZE 2048
    unsigned char receive_buffer[RECEIVE_BUFFER_SIZE];
    PRNetAddr remote_addr;
    PRInt32 bytes_read;
    jmethodID setaddress_method, setport_method, 
	setdata_method, setlength_method, inetaddress_constructor;
    waterObject *byte_array;
    waterClass *inetaddress_class;
    waterObject *remote_addr_object;

    bytes_read = 
	PR_RecvFrom(fd, receive_buffer, RECEIVE_BUFFER_SIZE, 0,
		    &remote_addr, PR_INTERVAL_NO_TIMEOUT);
    if(bytes_read < 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/io/IOException",
				 "PR_RecvFrom failed");
	return;
    }

    /* we need to fill in all the stuff in the DatagramPacket object. */
    setaddress_method = 
	(*env)->GetMethodID(env, 
			    (*env)->GetObjectClass(env, packet_object),
			    "setAddress", "(Ljava/net/InetAddress;)V");
    assert(setaddress_method);
    setport_method = 
	(*env)->GetMethodID(env, 
			    (*env)->GetObjectClass(env, packet_object),
			    "setPort", "(I)V");
    assert(setport_method);
    setdata_method = 
	(*env)->GetMethodID(env, 
			    (*env)->GetObjectClass(env, packet_object),
			    "setData", "([B)V");
    assert(setdata_method);
    setlength_method = 
	(*env)->GetMethodID(env, 
			    (*env)->GetObjectClass(env, packet_object),
			    "setLength", "(I)V");
    assert(setlength_method);

    inetaddress_class = (*env)->FindClass(env, "java/net/InetAddress");
    if(!inetaddress_class) {
	return;
    }
    inetaddress_constructor = 
	(*env)->GetMethodID(env, inetaddress_class, "<init>", "()V");
    assert(inetaddress_constructor);

    remote_addr_object = (*env)->NewObject(env, inetaddress_class,
					   inetaddress_constructor);
    if(!remote_addr_object) {
	return;
    }

    storeNetAddr(env, remote_addr_object, &remote_addr);

    byte_array = water_newPrimitiveArray(env, bytes_read, JNI_BYTE);
    if(!byte_array) {
	return;
    }
    memcpy(ARRAY_ELEMENT_PTR(byte_array, 0), receive_buffer, bytes_read);

    (*env)->CallVoidMethod(env, packet_object, setaddress_method,
			   remote_addr_object);
    (*env)->CallVoidMethod(env, packet_object, setport_method,
			   PR_ntohs(remote_addr.inet.port));
    (*env)->CallVoidMethod(env, packet_object, setlength_method, bytes_read);
    (*env)->CallVoidMethod(env, packet_object, setdata_method, byte_array);
}

void Java_java_net_PlainDatagramSocketImpl_setTTL(JNIEnv *env,
						  jobject socket_object,
						  jbyte ttl)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRSocketOptionData option_data;

    option_data.option = PR_SockOpt_IpTimeToLive;
    option_data.value.ip_ttl = ttl;

    if(PR_SetSocketOption(fd, &option_data) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), 
				 "java/net/SocketException",
				 "PR_SetSocketOption failed");
	return;
    }
}

jbyte Java_java_net_PlainDatagramSocketImpl_getTTL(JNIEnv *env,
						   jobject socket_object)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRSocketOptionData option_data;

    option_data.option = PR_SockOpt_IpTimeToLive;

    if(PR_GetSocketOption(fd, &option_data) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), 
				 "java/net/SocketException",
				 "PR_GetSocketOption failed");
	return 0;
    }

    return (jbyte)option_data.value.ip_ttl;
}

void Java_java_net_PlainDatagramSocketImpl_socketSetOption(JNIEnv *env,
						   jobject socket_object,
							   jint option,
							   jobject value)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRSocketOptionData option_data;
    jmethodID intvalue_method;

    /* I got these option numbers from reading the disassembly of */
    /* Sun's bytecode.                                            */
    if(option == 4) {
	/* SO_REUSEADDR */
	option_data.option = PR_SockOpt_Reuseaddr;

	intvalue_method = 
	    (*env)->GetMethodID(env, 
				(*env)->GetObjectClass(env, value),
				    "intValue", "()I");
	assert(intvalue_method);
	option_data.value.reuse_addr =
	    (*env)->CallIntMethod(env, value, intvalue_method);
    } else {
	/* nothing else recognized (except multicast stuff) */
	assert(0);
	return;
    }

    if(PR_SetSocketOption(fd, &option_data) != PR_SUCCESS) {
	water_throwExceptionNSPR(WATER_ENV(env), 
				 "java/net/SocketException",
				 "PR_SetSocketOption failed");
	return;
    }
}

jint Java_java_net_PlainDatagramSocketImpl_socketGetOption(JNIEnv *env,
						   jobject socket_object,
							   jint option)
{
    PRFileDesc *fd = getFD(env, socket_object);
    PRNetAddr local_addr;

    if(option == 15) {
	/* this is the SO_BINDADDR option, which seems to return the IP */
	/* the socket is bound to?                                      */
	if(PR_GetSockName(fd, &local_addr) != PR_SUCCESS) {
	    water_throwExceptionNSPR(WATER_ENV(env), 
				     "java/net/SocketException",
				     "PR_GetSockName failed");
	    return 0;
	}
	
	return (jint)PR_ntohl(local_addr.inet.ip);
    }


    /* shouldn't get here */
    assert(0);
    return -1;    
}

jint Java_java_net_SocketInputStream_socketRead(JNIEnv *env,
						jobject sis_object,
						jbyteArray byte_array,
						jint start_byte, 
						jint num_bytes)
{
    PRFileDesc *fd = getFD(env, sis_object);
    PRInt32 bytes_read;

#if defined(DEBUG_djm)
    int array_length;
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(byte_array), 0);
    assert(start_byte + num_bytes <= array_length);
#endif

#ifdef DEBUG_djm
    fprintf(stderr, "in socketRead(), fd is %p, reading %d bytes...\n", fd, num_bytes);
#endif

    bytes_read = PR_Read(fd, 
			 ARRAY_ELEMENT_PTR(byte_array, start_byte), num_bytes);
#ifdef DEBUG_djm
    fprintf(stderr, "socketRead() done, read %d bytes.\n", bytes_read);
#endif
    if(bytes_read < 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/io/IOException",
				 "PR_Read failed");
	return -1;
    } else if(bytes_read == 0) {
	return -1;
    } 
    
    return bytes_read;    
}

void Java_java_net_SocketOutputStream_socketWrite(JNIEnv *env, 
						  jobject sos_object,
						  jobject byte_array,
						  jint start_byte,
						  jint num_bytes)
{
    PRFileDesc *fd = getFD(env, sos_object);

#if defined(DEBUG_djm)
    int array_length;
    GET_INT_MACHINE(array_length, ARRAY_LENGTH_PTR(byte_array), 0);
    assert(start_byte + num_bytes <= array_length);
#endif

    if(PR_Write(fd, 
		ARRAY_ELEMENT_PTR(byte_array, start_byte), num_bytes) < 0) {
	water_throwExceptionNSPR(WATER_ENV(env), "java/io/IOException",
				 "PR_Write failed");
	return;	
    }
}

static PRFileDesc *getFD(JNIEnv *env, jobject object)
{
    jfieldID fd_object_field, fd_field;
    jobject fd_object;
   
    fd_object_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, object),
			   "fd", "Ljava/io/FileDescriptor;");
    assert(fd_object_field);
    fd_field =
	(*env)->GetFieldID(env, 
			   (*env)->FindClass(env, "java/io/FileDescriptor"),
			   "fd", "I");
    assert(fd_field);
    fd_object = (*env)->GetObjectField(env, object, fd_object_field);
    assert(fd_object);

    return (PRFileDesc *)(*env)->GetIntField(env, fd_object, fd_field);
}

static void storeFD(JNIEnv *env, jobject object, PRFileDesc *fd)
{
    jfieldID fd_object_field, fd_field;
    jobject fd_object;

    fd_object_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, object),
			   "fd", "Ljava/io/FileDescriptor;");
    assert(fd_object_field);
    fd_field =
	(*env)->GetFieldID(env, 
			   (*env)->FindClass(env, "java/io/FileDescriptor"),
			   "fd", "I");
    assert(fd_field);
    fd_object = (*env)->GetObjectField(env, object, fd_object_field);
    assert(fd_object);

    (*env)->SetIntField(env, fd_object, fd_field, (jint)fd);
}

static int getIP(JNIEnv *env, jobject object)
{
    jfieldID address_field;
   
    address_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, object),
			   "address", "I");
    assert(address_field);

    return (*env)->GetIntField(env, object, address_field);
}

static void storeNetAddr(JNIEnv *env, jobject addr_object, PRNetAddr *addr)
{
    jfieldID address_field, family_field;
    jclass addr_class;

    addr_class = (*env)->GetObjectClass(env, addr_object);
    if(!addr_class) {
	assert(0);
	return;
    }
    address_field = (*env)->GetFieldID(env, addr_class, "address", "I");
    family_field = (*env)->GetFieldID(env, addr_class, "family", "I");
    if(!address_field || !family_field) {
	assert(0);
	return;
    }

    (*env)->SetIntField(env, addr_object, address_field, addr->inet.ip);
    (*env)->SetIntField(env, addr_object, family_field, addr->inet.family);
}

static jobject getAddress(JNIEnv *env, jobject socket_object)
{
    jfieldID address_field;
    jobject address_object;

    address_field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, socket_object),
			   "address", "Ljava/net/InetAddress;");
    if(!address_field) {
	assert(0);
	return 0;
    }
    address_object = 
	(*env)->GetObjectField(env, socket_object, address_field);
    assert(address_object);

    return address_object;
}

#if 0
/* not used right now */
static jint getPort(JNIEnv *env, jobject socket_object)
{
    jfieldID field;

    field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, socket_object),
			   "port", "I");
    if(!field) {
	assert(0);
	return 0;
    }

    return (*env)->GetIntField(env, socket_object, field);
}
#endif

static jint getLocalPort(JNIEnv *env, jobject socket_object)
{
    jfieldID field;

    field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, socket_object),
			   "localport", "I");
    if(!field) {
	assert(0);
	return 0;
    }

    return (*env)->GetIntField(env, socket_object, field);
}

static void storePort(JNIEnv *env, jobject socket_object, jint port)
{
    jfieldID field;

    field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, socket_object),
			   "port", "I");
    if(!field) {
	assert(0);
	return;
    }

    (*env)->SetIntField(env, socket_object, field, port);
}

static void storeLocalPort(JNIEnv *env, jobject socket_object, jint port)
{
    jfieldID field;

    field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, socket_object),
			   "localPort", "I");
    if(!field) {
	assert(0);
	return;
    }

    (*env)->SetIntField(env, socket_object, field, port);
}

static void storeLocalport(JNIEnv *env, jobject socket_object, jint port)
{
    jfieldID field;

    field = 
	(*env)->GetFieldID(env, (*env)->GetObjectClass(env, socket_object),
			   "localport", "I");
    if(!field) {
	assert(0);
	return;
    }

    (*env)->SetIntField(env, socket_object, field, port);
}

