#ifndef __NNYSL_MACRO_H__
#define __NNYSL_MACRO_H__

#include <assert.h>
#include "util.h"
#include <string.h>
#include "log.h"

#define NNYSL_ASSERT(x) \
    if(!(x)) { \
        NNYSL_LOG_ERROR(NNYSL_LOG_ROOT()) << "ASSERTION: " #x \
        << "\nbacktrace:\n" \
        << nnysl::BacktraceToString(100,2,"    ") ; \
        assert(x) ; \
    } 

#define NNYSL_ASSERT2(x,w) \
    if(!(x)) { \
        NNYSL_LOG_ERROR(NNYSL_LOG_ROOT()) << "ASSERTION: " #x \
        << "\n" << w \
        << "\nbacktrace:\n" \
        << nnysl::BacktraceToString(100,2,"    ") ; \
        assert(x) ; \
    }

#endif