#include "../nnysl/log.h"
#include "../nnysl/util.h"

#include <iostream>


int main(){

    nnysl::Logger::ptr logger(new nnysl::Logger) ;
    logger->addAppender(nnysl::LogAppender::ptr(new nnysl::StdoutLogAppender));
    
    nnysl::FileLogAppender::ptr file_appender(new nnysl::FileLogAppender("./test/log.txt"));
    

    // nnysl::LogEvent::ptr event(new nnysl::LogEvent(logger ,nnysl::LogLevel::DEBUG ,__FILE__ , __LINE__, 0, nnysl::GetThreadId(),  2, time(0)));

    // logger->log(nnysl::LogLevel::DEBUG,event) ;

    // nnysl::LogFormatter::ptr fmt(new nnysl::LogFormatter("%d%t%m%n"));
    // file_appender->setFormatter(fmt) ;
    logger->addAppender(file_appender) ;

    NNYSL_LOG_INFO(logger) << "这是一个日志" ; 
    NNYSL_LOG_INFO(logger) << "test test test " ;
    
    auto i = nnysl::LoggerMgr::GetInstance()->getLogger("xx") ;
    NNYSL_LOG_INFO(i) << "mgr test " ;

    return 0;
	
}

