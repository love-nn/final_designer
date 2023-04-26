#ifndef __NNYSL_UTIL_H_
#define __NNYSL_UTIL_H_
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>



namespace nnysl{
    

pid_t GetThreadId();

uint32_t GetFiberId() ;

void Backtrace(std::vector<std::string>& bt ,int size , int skip = 1 ) ;

std::string BacktraceToString(int size, int skip = 2 ,const std::string& prefix = "") ;

}



#endif