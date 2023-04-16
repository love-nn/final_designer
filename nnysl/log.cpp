#include "log.h"

namespace nnysl{

/**
 * @brief 将LogLevel 转换为字符串
 *
 * 
*/
const char* LogLevel::toString(LogLevel::Level level ) {
    switch(level){
#define XX(name) \
    case(name): \
        return #name; \
        break ; \

    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
        break;
    }
    return "UNKNOW" ;
}


/**
 * @brief LogEvent 构造函数
 * @param file: 日志事件文件名
 * @param line: 发生的位置行数
 * @param elapse: 服务运行时间
 * @param thread_id: 线程号
 * @param fiber_id: 协程号
 * @param time: 发生时间
 * 
 */
LogEvent::LogEvent(std::shared_ptr<Logger> logger , LogLevel::Level level , const char* file , int32_t line , uint32_t elapse , 
    uint32_t thread_id , uint32_t fiber_id , uint64_t time) 
        :m_file(file)
        ,m_line(line)
        ,m_elapse(elapse)
        ,m_threadId(thread_id)
        ,m_fiberId(fiber_id)
        ,m_time(time)
        ,m_logger(logger)
        ,m_level(level)
        {

    }


void LogEvent::format(const char* fmt , ...) {
    va_list al;
    va_start(al,fmt) ;
    format(fmt, al) ;
}

void LogEvent::format(const char* fmt , va_list al) {
    char* buf = nullptr ;
    int len = vasprintf(&buf, fmt, al ) ;
    if ( len != -1 ) {
        m_ss << std::string(buf, len) ;
        free ( buf ) ;
    }
}

/**
 * @brief Logger 构造函数 
 * @param name: 构造器名称
 */
Logger::Logger(const std::string& name) 
    :m_name(name)
    ,m_level(LogLevel::DEBUG)  {
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

/**
 * @brief log 日志器提供 的日志打印函数
 * @param level 日志等级
 * @param event 日志事件
 * 
 */
void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if ( level >= m_level ) {
        if ( !m_appenders.empty() ) {
            auto self = shared_from_this() ;
            for ( auto& i : m_appenders ) {
                i->log(self , level, event) ;
            }
        }
    }
}

void Logger::debug(LogEvent::ptr event ) {
    log(LogLevel::DEBUG , event) ;
}

void Logger::info(LogEvent::ptr event ) {
    log(LogLevel::INFO , event) ;
}

void Logger::fatal(LogEvent::ptr event ) {
    log(LogLevel::FATAL , event) ;
}

void Logger::warn(LogEvent::ptr event ) {
    log(LogLevel::WARN , event) ;
}

void Logger::error(LogEvent::ptr event ) {
    log(LogLevel::ERROR , event) ;
}

/**
 * @brief 为日志器（logger） 增加 appender
 * @param appender: LogAppender 对象的shared_ptr
 */
void Logger::addAppender(LogAppender::ptr appender) {
    if ( !appender->getFormatter() ) {
        appender->setFormatter(m_formatter) ;
    }
    m_appenders.push_back(appender) ;
}

/**
 * @brief 为日志器（logger） 删除 appender
 * @param appender: LogAppender 对象的shared_ptr
 */
void Logger::delAppender(LogAppender::ptr appender) {
    for (auto it  = m_appenders.begin() ;
              it != m_appenders.end() ; it ++ ){
            if ( *it == appender ) {
                m_appenders.erase(it) ;
                break ;
            }

        }
}




/**
 * @brief 日志格式化类（LogFormatter）构造方法
 * @param pattern: 模式， 日志格式的模式 在init中详细介绍各种格式 
 */
LogFormatter::LogFormatter(const std::string& pattern) 
    :m_pattern(pattern) {
    init() ;
}


/**
 * @brief 日志格式化类（LogFormatter）中的format 方法， 将日志器（logger）， 日志等级（Level），日志事件（LogEvent）格式化为appender输出的格式。
 * @param logger: 需要格式的日志
 * @param level: 
 * @param event: 
 * 
 */
std::string LogFormatter::format(std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    std::stringstream ss;
    for ( auto& i : m_items ) {
        i->format(ss, logger , level , event ) ;
    } 
    return ss.str() ;
}

bool FileLogAppender::reopen() {
    if ( m_filestream ) {
        m_filestream.close() ;
    }
    m_filestream.open( m_filename ) ;
    return !! m_filestream ;
}




void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level , LogEvent::ptr event ) {
    if ( level >= m_level ) {
        std::cout << m_formatter->format(logger, level , event);
    }
}

FileLogAppender::FileLogAppender( const std::string& filename)
    :m_filename(filename) {
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level , LogEvent::ptr event ) {
    if ( level >= m_level ) {
        if ( reopen() ) {
            m_filestream << m_formatter->format(logger , level , event) ;
        }
    }
}


/**
 * @brief 初始化formatter 
 * default formatter parse : "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
 * 
 */
void LogFormatter::init() {

    std::vector<std::tuple<std::string,std::string,int> >  vec ;
    std::string n_str ;
    
    for ( size_t i = 0 ; i < m_pattern.size() ; ++ i ) {
        if ( m_pattern[i] != '%' ) {
            n_str.append(1,m_pattern[i]) ;
            continue ; 
        }    

        if ( ( i + 1 ) < m_pattern.size() ) {
            if ( m_pattern[ i + 1 ] == '%' ) {
                n_str.append(1,'%') ;
                continue ; 
            }
        }
        
        size_t n = i + 1 ;
        int fmt_status = 0 ;
        size_t fmt_begin = 0 ;
        std::string str ;
        std::string fmt ;

        while ( n < m_pattern.size() ) {
            if (!fmt_status &&  ! isalpha( m_pattern[n] ) && m_pattern[n] != '{' && m_pattern[n] != '}') {
                str = m_pattern.substr( i + 1 , n - i - 1 ) ;
                break ;
            }
            if ( fmt_status == 0 ) {
                if ( m_pattern[n] == '{' ) {
                    str = m_pattern.substr( i + 1 , n - i - 1 ) ;
                    fmt_status = 1 ;
                    fmt_begin = n ;
                    ++ n;
                    continue ; 
                }

            }
            else if ( fmt_status == 1 ) {
                if ( m_pattern[n] == '}' ) {
                    fmt = m_pattern.substr(fmt_begin + 1 , n - fmt_begin - 1 ) ;
                    fmt_status = 0 ;
                    ++ n ;
                    break ;
                }
            }
            ++ n ;
            if ( n == m_pattern.size() ) {
                if ( str.empty() ) {
                    str = m_pattern.substr( i+1 ) ;
                }
            }
        }
        if ( fmt_status == 0 ) {
            if ( ! n_str.empty() ) {
                vec.push_back(std::make_tuple(n_str,"", 0 )) ;
                n_str.clear() ;
            }
            vec.push_back(std::make_tuple(str,fmt,1 ));
            i = n - 1 ;
        }
        else if ( fmt_status == 1 ) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl ;
            vec.push_back(std::make_tuple("<pattern_error>" , fmt , 0 ) ) ;
        }
    }

    if ( !n_str.empty() ) {
        vec .push_back(std::make_tuple(n_str , "" , 0 ) ) ;
        n_str.clear() ;
    }

    /**
     * %m 消息体
     * %p 日志等级
     * %r 启动后的时间
     * %c 日志名称
     * %t 线程id
     * %n 回车换行
     * %d 时间
     * %f 文件名
     * %l 行号
     * %T 制表符
     */
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str) > > s_format_items = {
#define XX(str,C) \
        {#str , [] (const std::string& fmt) { return FormatItem::ptr(new C(fmt)) ; }}
        

        XX(m,MessageFormatItem),
        XX(p,LevelFormatItem),
        XX(r,ElapseFormatItem),
        XX(c,NameFormatItem),
        XX(t,ThreadIdFormatItem),
        XX(n,NewLineFormatItem),
        XX(d,DateTimeFormatItem),
        XX(f,FileNameFormatItem),
        XX(l,LineFormatItem),
        XX(T,TabFormatItem),
        XX(F,FiberIdFormatItem)

#undef XX
    };

    for( auto& i : vec ) {
        if ( std::get<2>(i) == 0 ) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i)))); 
        }
        else {
            auto it = s_format_items.find(std::get<0>(i)) ;
            if ( it == s_format_items.end() ) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<error_format %" + std::get<0>(i) + ">")));
                
            }
            else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        // std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl ; 
    }



}

LogEventWrap::LogEventWrap(LogEvent::ptr e)
    :m_event(e) {
    
}
LogEventWrap::~LogEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(), m_event) ;
}

std::stringstream& LogEventWrap::getSS() {
    return m_event->getSS() ;
}

void MessageFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << event->getContent () ;
}

void LevelFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << LogLevel::toString(level) ;
}

void ElapseFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << event->getElapse() ;
}

void NameFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << logger->getName()  ;
}

void ThreadIdFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << event->getThreadId() ;
}

void NewLineFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << std::endl ;
}

void FiberIdFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << event->getFiberId() ;  
}

void DateTimeFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    struct tm tm ;
    time_t time = event->getTime() ;
    localtime_r(&time , &tm ) ;
    char buf[64] ;
    strftime(buf, sizeof(buf) , m_format.c_str() , &tm) ;
    os << buf ;
}

void FileNameFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << event->getFile() ;
}

void LineFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << event->getLine() ;
}

void StringFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << m_string ;
}

void TabFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger , LogLevel::Level level , LogEvent::ptr event) {
    os << "\t" ;
}


LoggerManager::LoggerManager() {
    m_root.reset(new Logger) ;
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
}

Logger::ptr LoggerManager::getLogger(const std::string& name) {
    auto it = m_loggers.find(name) ;
    return it == m_loggers.end() ? m_root : it->second ;
}

}

