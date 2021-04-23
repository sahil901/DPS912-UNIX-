/*
Sahil Patel (159-065-176)
Abdulbasid Guled (156-024-184)
Andre Jenah (134-901-180)
*/

// to avoid the use of using std:: each time

using namespace std;

// defined the enumeration called LOG_LEVEL
// specified the log levels: DEBUG, WARNING, ERROR, CRITICAL
enum LOG_LEVEL
{
    DEBUG = -1,
    WARNING = 0,
    ERROR = 1,
    CRITICAL = 2
};

// exposing the following functions to each process
void InitializeLog();
void SetLogLevel(LOG_LEVEL level);
void Log(LOG_LEVEL level, const char* prog, const char* func, int line, const char* message);
void ExitLog();