#include "../nnysl/nnysl.h"
#include <vector> 

nnysl::Logger::ptr g_logger = NNYSL_LOG_ROOT() ;

void fun1() {
    NNYSL_LOG_INFO(g_logger) << "name : " << nnysl::Thread::GetName() 
        << "this.name : " << nnysl::Thread::GetThis()->getName() 
        << "id : " << nnysl::GetThreadId() 
        << "this.id : " << nnysl::Thread::GetThis() ->getId() ;
}

void fun2() {

}

int main(int argc, char** argv) {
    NNYSL_LOG_INFO(g_logger) << "thread test begin " ;
    std::vector<nnysl::Thread::ptr> threads;
    for ( int i = 0 ; i < 5 ; ++ i )  {
        nnysl::Thread::ptr thread(new nnysl::Thread(&fun1, "name_" + std::to_string(i))) ;
        threads.push_back(thread) ;
    }

    for(int i = 0 ; i < 5 ; ++ i ) {
        threads[i]->join() ;
    }

    NNYSL_LOG_INFO(g_logger) << "thread test end " ;
    return 0 ; 
}