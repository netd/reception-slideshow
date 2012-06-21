#include <fstream>
#include <stdarg.h>
/*
http://www.infernodevelopment.com/c-log-file-class-forget-debuggers
*/
using namespace std;

class Log {
public:
    Log(const char* filename, bool enabled);
    ~Log();
    void Write(const char* logline, ...);
private:
    ofstream m_stream;
    bool loggingEnabled;
};
