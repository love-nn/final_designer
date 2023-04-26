#include "../nnysl/nnysl.h"
#include <vector> 

nnysl::Logger::ptr g_logger = NNYSL_LOG_ROOT() ;
nnysl::Mutex s_mutex ;

int count = 0 ;
void fun1() {
    NNYSL_LOG_INFO(g_logger) << "name : " << nnysl::Thread::GetName() 
        << "this.name : " << nnysl::Thread::GetThis()->getName() 
        << "id : " << nnysl::GetThreadId() 
        << "this.id : " << nnysl::Thread::GetThis() ->getId() ;

    for(int i = 0 ; i < 1000000 ; ++ i ) {
        nnysl::Mutex::Lock lock(s_mutex) ;
        ++count ;
    }
}

void fun2() {
    while(true) 
        NNYSL_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
}

void fun3() {
    while(true)
        NNYSL_LOG_INFO(g_logger) << "---------------------------------------------";
}

int main(int argc, char** argv) {
    NNYSL_LOG_INFO(g_logger) << "thread test begin " ;
    YAML::Node root = YAML::LoadFile("/home/nnysl/final/final_designer/bin/conf/log2.yaml") ;
    nnysl::Config::LoadFromYaml(root) ;
    std::vector<nnysl::Thread::ptr> threads;
    for ( int i = 0 ; i < 10 ; ++ i )  {
        nnysl::Thread::ptr thread (new nnysl::Thread(&fun2, "name_" + std::to_string(i * 2))) ;
        nnysl::Thread::ptr thread2(new nnysl::Thread(&fun3, "name_" + std::to_string(i * 2))) ;
        threads.push_back(thread) ;
        threads.push_back(thread2) ;
    }

    for(size_t i = 0 ; i < threads.size() ; ++ i ) {
        threads[i]->join() ;
    }

    
    NNYSL_LOG_INFO(g_logger) << "thread test end " ;
    NNYSL_LOG_INFO(g_logger) << count  ;
    
    return 0 ; 
}