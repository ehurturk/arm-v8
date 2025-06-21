#include "frontend/uart.h"
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <unistd.h>

/* 
 * Increase program data state,
 * malloc and its friends depend on this,
 * so the _sbrk stub should be implemented as
 * there is no OS available.
 */
caddr_t _sbrk(int incr) {
  extern char _end; /* defined in linker */
  static char *heap_end = 0;
  char *prev_heap_end;

  if (heap_end == 0) {
    heap_end = &_end;
  }

  prev_heap_end = heap_end;

  heap_end += incr;
  return (caddr_t)prev_heap_end;
}

/*
 * _write - Write data to file descriptor
 *
 * For stdout/stderr, we output to UART.
 * This enables printf() to work.
 */
// int _write(int file, char *ptr, int len) {
//   if (file == STDOUT_FILENO || file == STDERR_FILENO) {
//     // Output to UART
//     for (int i = 0; i < len; i++) {
//       uart_putc(ptr[i]);
//     }
//     return len;
//   }

//   // // Invalid file descriptor
//   // errno = EBADF;
//   return -1;
// }

// void *memcpy(void *dest, const void *src, unsigned long n) {
//   unsigned char *d = dest;
//   const unsigned char *s = src;
//   while (n--) {
//     *d++ = *s++;
//   }
//   return dest;
// }

/*
 * _read - Read data from file descriptor
 *
 * For stdin, we read from UART.
 * This enables scanf(), getchar(), etc. to work.
 */
// int _read(int file, char *ptr, int len) {
//   if (file == STDIN_FILENO) {
//     // Read from UART
//     for (int i = 0; i < len; i++) {
//       ptr[i] = uart_getc();

//       // Handle line endings
//       if (ptr[i] == '\r') {
//         ptr[i] = '\n';
//         uart_putc('\r');
//         uart_putc('\n');
//         return i + 1;
//       }

//       // Echo character back
//       if (ptr[i] >= 32 && ptr[i] <= 126) {
//         uart_putc(ptr[i]);
//       }

//       // Handle newline
//       if (ptr[i] == '\n') {
//         return i + 1;
//       }
//     }
//     return len;
//   }

//   // // Invalid file descriptor
//   // errno = EBADF;
//   return -1;
// }

/*
 * _close - Close file descriptor
 *
 * We don't have a real filesystem, so just return error.
 */
int _close(int file) {
  (void)file;
  // errno = EBADF;
  return -1;
}