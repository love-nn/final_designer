#include "../nnysl/nnysl.h"
#include "assert.h"

nnysl::Logger::ptr g_logger = NNYSL_LOG_ROOT() ;

void test_assert() {
    NNYSL_LOG_INFO(g_logger) << nnysl::BacktraceToString(10, 2, "   ") ;
    NNYSL_ASSERT(0) ;
}

int main() {
    test_assert() ;
    return 0;
}