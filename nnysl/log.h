#ifndef __NNYSL__LOG_H_
#define __NNYSL__LOG_H_

#include <string>
#include <stdint.h>
#include <memory> 
#include <list>
#include <sstream>
#include <fstream> 
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <time.h>
#include <stdarg.h>
#include "./singleton.h"
#include "./util.h"


#define NNYSL_LOG_LEVEL(logger,level) \
    if(logger->getLevel() <= level) \
        nnysl::LogEventWrap(nnysl::LogEvent::ptr(new nnysl::LogEvent(logger, level, \
        __FILE__,__LINE__,0,nnysl::GetThreadId(),\
            nnysl::GetFiberId(),time(0)))).getSS()

#define NNYSL_LOG_DEBUG(logger) NNYSL_LOG_LEVEL(logger,nnysl::LogLevel::DEBUG)
#define NNYSL_LOG_FATAL(logger) NNYSL_LOG_LEVEL(logger,nnysl::LogLevel::FATAL)
#define NNYSL_LOG_ERROR(logger) NNYSL_LOG_LEVEL(logger,nnysl::LogLevel::ERROR)
#define NNYSL_LOG_WARN(logger) NNYSL_LOG_LEVEL(logger,nnysl::LogLevel::WARN)
#define NNYSL_LOG_INFO(logger) NNYSL_LOG_LEVEL(logger,nnysl::LogLevel::INFO)


#define NNYSL_LOG_FMT_LEVEL(logger,level,fmt,...) \
    if(logger->getLevel() <= level ) \
        nnysl::LogEventWrap(nnysl::LogEvent::ptr(new nnysl::LogEvent(logger, level, \
        __FILE__,__LINE__,0,nnysl::GetThreadId(),\
            nnysl::GetFiberId(),time(0)))).getEvent()->format(fmt,__VA_ARGS__)

#define NNYSL_LOG_FMT_DEBUG(logger,fmt,...) NNYSL_LOG_FMT_LEVEL(logger,nnysl::LogLevel::DEBUG,fmt,__VA_ARGS__ )
#define NNYSL_LOG_FMT_FATAL(logger,fmt,...) NNYSL_LOG_FMT_LEVEL(logger,nnysl::LogLevel::FATAL,fmt,__VA_ARGS__)
#define NNYSL_LOG_FMT_ERROR(logger,fmt,...) NNYSL_LOG_FMT_LEVEL(logger,nnysl::LogLevel::ERROR,fmt,__VA_ARGS__)
#define NNYSL_LOG_FMT_WARN(logger,fmt,...) NNYSL_LOG_FMT_LEVEL(logger,nnysl::LogLevel::WARN,fmt,__VA_ARGS__)
#define NNYSL_LOG_FMT_INFO(logger,fmt,...) NNYSL_LOG_FMT_LEVEL(logger,nnysl::LogLevel::INFO,fmt,__VA_ARGS__)

#define NNYSL_LOG_ROOT() nnysl::LoggerMgr::GetInstance()->getRoot()

#define NNYSL_LOG_NAME(name) nnysl::LoggerMgr::GetInstance()->getLogger(name)


namespace nnysl{

class Logger;
class LoggerManager ; 
// 日志级别
class LogLevel {
public :
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
        
    };
    static const char* toString ( LogLevel::Level level );
    static LogLevel::Level fromString ( std::string& level );

};
// 日志格式信息
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger,LogLevel::Level level ,  const char* file , int32_t line , uint32_t elapse , 
    uint32_t thread_id , uint32_t fiber_id , uint64_t time) ;
    LogEvent() ;
    const char* getFile() const { return m_file ; }
    int32_t getLine() const { return m_line ; } 
    uint32_t getElapse() const { return m_elapse ; }
    uint32_t getThreadId() const { return m_threadId ; }
    uint32_t getFiberId() const { return m_fiberId ; }
    uint32_t getTime() const { return m_time ; }
    std::string getContent() const { return m_ss.str() ; }
    std::stringstream& getSS() { return m_ss ; }
    std::shared_ptr<Logger> getLogger() { return m_logger ; }
    LogLevel::Level getLevel() { return m_level ; } 

    void format(const char* fmt, ...) ;
    void format(const char* fmt, va_list al ) ;
private :
    const char * m_file = nullptr; // 文件指针
    int32_t m_line = 0 ;           // 行号
    uint32_t m_elapse = 0 ;        // 程序运行时间(ms)
    uint32_t m_threadId = 0 ;      // 线程号
    uint32_t m_fiberId = 0 ;       // 协程号
    uint64_t m_time = 0 ;          // 时间戳
    std::stringstream m_ss ;        // 内容
    
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level ;
};

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    LogEvent::ptr getEvent() { return m_event; } 
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};



// 日志格式化
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern) ;
    std::string format(std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) ;
public:
    class FormatItem {
        public :
            typedef std::shared_ptr<FormatItem> ptr ;
            // FormatItem(const std::string& fmt = "") {};
            virtual ~FormatItem() {} 
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level ,  LogEvent::ptr event) = 0 ;
        
    };

    std::string getPattern() const {   return m_pattern ; } 
    void init() ;
    bool isError() const { return m_error ; } 
    void setError() { m_error = true ; } 
private:
    std::string m_pattern ; 
    std::vector<FormatItem::ptr> m_items ;
    bool m_error = false ; 
};

class MessageFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<MessageFormatItem> ptr ;
    MessageFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;
};

class LevelFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<LevelFormatItem> ptr ;
    LevelFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<ElapseFormatItem> ptr ;
    ElapseFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};

class NameFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<NameFormatItem> ptr ;
    NameFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};

class ThreadIdFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<ThreadIdFormatItem> ptr ;
    ThreadIdFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<NewLineFormatItem> ptr ;
    NewLineFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};

class FiberIdFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<FiberIdFormatItem> ptr ;
    FiberIdFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<DateTimeFormatItem> ptr ;
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        : m_format(format) {
            if ( m_format.empty() ) {
                m_format = "%Y-%m-%d %H:%M:%S" ; 
            }
        }
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
private:
    std::string m_format ;
};

class FileNameFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<FileNameFormatItem> ptr ;
    FileNameFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};

class LineFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<LineFormatItem> ptr ;
    LineFormatItem(const std::string& fmt ="" ) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    
};


class StringFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<StringFormatItem> ptr ;
    StringFormatItem(const std::string& str) 
        :m_string(str){} 
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    

private:
    std::string m_string ;
};

class TabFormatItem : public LogFormatter::FormatItem {
public :
    typedef std::shared_ptr<TabFormatItem> ptr ;
    TabFormatItem(const std::string& str) 
        :m_string(str){} 
    void format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override;    

private:
    std::string m_string ;
};

// 日志输出 
class LogAppender {
friend class Logger ; 
public :
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {} ;

    virtual std::string toYamlString() = 0 ;

    virtual void log(std::shared_ptr<Logger> logger , LogLevel::Level level, LogEvent::ptr event) = 0 ;
    void setFormatter(LogFormatter::ptr val );
    void setLevel(LogLevel::Level val )  { m_level = val ; } 
    LogFormatter::ptr getFormatter() { return m_formatter; } 

protected :
    LogLevel::Level m_level = LogLevel::DEBUG; 
    LogFormatter::ptr m_formatter;

    bool m_have_formatter = false ; 
};



// 日志器  
class Logger : public std::enable_shared_from_this<Logger> {
friend class LoggerManager ;
public :
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root" ) ;

    void log( LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event ) ;
    void info (LogEvent::ptr event ) ;
    void warn (LogEvent::ptr event ) ;
    void fatal(LogEvent::ptr event ) ;
    void error(LogEvent::ptr event ) ;

    void addAppender(LogAppender::ptr appender) ;
    void delAppender(LogAppender::ptr appender) ;
    void clearAppenders() ;

    LogLevel::Level getLevel() const { return m_level; } 
    const std::string& getName() const { return m_name; }

    void setLevel(LogLevel::Level level ) { m_level = level ; }
    void setFormatter(LogFormatter::ptr val ) ;
    void setFormatter(const std::string& val ) ;

    LogFormatter::ptr getFormatter() const ;
    
    std::string toYamlString() ;

private:
    std::string m_name ;            // 日志名称
    LogLevel::Level m_level;        // 日志器级别
    std::list<LogAppender::ptr> m_appenders ;  // 日志器 输出位置集合
    LogFormatter::ptr m_formatter ;
    Logger::ptr m_root ;
};

// 输出到控制台Appender
class StdoutLogAppender : public LogAppender {
public :
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override ;
    std::string toYamlString() override ;
private :

};

// 输出到 文件的 Appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename ) ;
    virtual void log(std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) override ;
    std::string toYamlString() override ;

    bool reopen() ;  // 重新打开文件 成功-1 失败- 0 
private:
    std::string m_filename ;
    std::ofstream m_filestream ; 
};



class LoggerManager {
public:
    LoggerManager() ;
    Logger::ptr getLogger(const std::string& name) ;
    Logger::ptr getRoot() { return m_root ; }
    std::string toYamlString() ;

    void init() ;
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
    
};

typedef nnysl::Singleton<LoggerManager> LoggerMgr ; 

}



#endif