#ifndef SWPRINTF
# ifndef __MSVCRT__
#  define SWPRINTF(wcs, maxlen, ...)	swprintf(wcs, maxlen, __VA_ARGS__)
# else /* !__MSVCRT__ */
#  define SWPRINTF(wcs, maxlen, ...)	swprintf(wcs, __VA_ARGS__)
# endif /* !__MSVCTR__ */
#endif /* !SWPRINTF */
