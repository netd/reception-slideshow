#include "log.h"

Log::Log(const char* filename, bool enabled) {
    if(enabled)
    {
        loggingEnabled = true;
        m_stream.open(filename);
    }
    else
    {
        loggingEnabled = false;
    }

}

void Log::Write(const char* logline, ...){
    if(loggingEnabled)
    {
       va_list argList;
       char cbuffer[1024];
       va_start(argList, logline);
       vsnprintf(cbuffer, 1024, logline, argList);
       va_end(argList);
       m_stream << cbuffer << endl;
    }
}

Log::~Log(){
    if(loggingEnabled)
        m_stream.close();
}
