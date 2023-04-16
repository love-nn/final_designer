#ifndef __NNYSL_UTIL_H_
#define __NNYSL_UTIL_H_
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>


namespace nnysl{
    

pid_t GetThreadId();

uint32_t GetFiberId() ;

}



#endif