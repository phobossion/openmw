#ifndef MWO_NET_COMMON
#define MWO_NET_COMMON

// Some common stuff for out netcode

#if defined(__linux__) || defined(__APPLE__)

// The Unix compliant part
#define MWO_UNIX 1

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#elif defined(WIN32)

// The Windows part
#define MWO_UNIX 0

#include <winsock2.h>

// [TODO] #include <ws2_32.h> ???

#endif

#include <stdexcept>
#include <iostream>

#endif

#ifndef NDEBUG
#define MWO_NET_DEBUG
#endif

#ifdef MWO_NET_DEBUG
#define MWO_TRACE(msg) std::cout << msg << std::endl;
#else
#define MWO_TRACE(msg)
#endif
