#ifndef COMPAT_H
#define COMPAT_H

/* =========================
 * Windows
 * ========================= */
#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #ifndef _WINSOCKAPI_
    #define _WINSOCKAPI_
  #endif

  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #include <io.h>

  typedef int socklen_t;

/* Socket close alias (different from your game’s close_socket() function) */
#define close_fd(fd) closesocket(fd)

/* String compares */
#define strcasecmp  _stricmp
#define strncasecmp _strnicmp

/* Windows has no crypt(); just stub it */
static inline char *crypt(const char *key, const char *salt) {
  (void)key; (void)salt;
  return "NOCRYPT";
}

#else /* =========================
* POSIX / Linux
* ========================= */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

#define close_fd(fd) close(fd)

/* System provides crypt() in unistd.h */
#include <unistd.h>

#endif /* _WIN32 */

/* =========================
 * Shared logging fix
 * ========================= */
#define logf mudlogf
void mudlogf(const char *fmt, ...);


#ifdef _WIN32
#define WOULD_BLOCK WSAEWOULDBLOCK
#else
#include <errno.h>
#define WOULD_BLOCK EWOULDBLOCK
#endif


#endif /* COMPAT_H */
