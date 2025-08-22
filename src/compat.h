#ifndef COMPAT_H
#define COMPAT_H

/* Cross-platform includes */
#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <io.h>
  #define close_socket(s) closesocket(s)
  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
  typedef int socklen_t;
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #define close_socket(s) close(s)
#endif

/* Logging fix: avoid conflict with <math.h> logf() */
#define logf mudlogf
void mudlogf(const char *fmt, ...);

/* Crypt fix */
#ifndef NOCRYPT
  /* Ensure prototype matches system crypt() */
  #include <unistd.h>
  /* Some systems hide crypt() unless feature macros set */
  #ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE
  #endif
  char *crypt(const char *key, const char *salt);
#else
  /* Stub crypt for builds with -DNOCRYPT */
  static inline char *crypt(const char *key, const char *salt) {
      (void)key; (void)salt;
      return "NOCRYPT";
  }
#endif

#endif /* COMPAT_H */
