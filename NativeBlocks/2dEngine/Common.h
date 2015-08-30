
#ifdef DEBUG_PRINTS
   #include <cstdio>
   #define DebugPrintf(FMT, ARGS...) do { \
           fprintf(stderr, "%s:%d\t" FMT, __PRETTY_FUNCTION__, __LINE__, ## ARGS); \
       } while (0)
   #define DebugTrace() do { \
           fprintf(stderr, "%s\n", __PRETTY_FUNCTION__); \
      } while (0)
#else
   #define DebugPrintf(FMT, ARGS...)
   #define DebugTrace()
#endif


#define PI 3.141592653589793238463
