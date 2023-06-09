
#include "../nnysl/config.h"
#include "../nnysl/log.h"
#include <yaml-cpp/yaml.h>
/**

nnysl::ConfigVar<int>::ptr g_int_value_config = 
nnysl::Config::Lookup("system.port",(int)8080,"system port") ;

nnysl::ConfigVar<float>::ptr g_int_1_value_config = 
nnysl::Config::Lookup("system.port",(float)8080,"system port") ;


nnysl::ConfigVar<float>::ptr g_float_value_config = 
nnysl::Config::Lookup("system.value",(float)1.9999,"system value") ;

nnysl::ConfigVar<std::vector<int>>::ptr g_vec_value_config = 
nnysl::Config::Lookup("system.int_vec",std::vector<int>(1,2),"system int vec") ;

nnysl::ConfigVar<std::list<int>>::ptr g_list_value_config = 
nnysl::Config::Lookup("system.int_list",std::list<int>(1,2),"system int list") ;

nnysl::ConfigVar<std::set<int>>::ptr g_set_value_config = 
nnysl::Config::Lookup("system.int_set",std::set<int>{1,2},"system int set") ;

nnysl::ConfigVar<std::unordered_set<int>>::ptr g_unordered_set_value_config = 
nnysl::Config::Lookup("system.int_unordered_set",std::unordered_set<int>{1,2},"system int unordered_set") ;

nnysl::ConfigVar<std::map<std::string , int>>::ptr g_map_value_config = 
nnysl::Config::Lookup("system.map",std::map<std::string , int>{{"k",2}},"system int map") ;

nnysl::ConfigVar<std::unordered_map<std::string , int>>::ptr g_unordered_map_value_config = 
nnysl::Config::Lookup("system.map",std::unordered_map<std::string , int>{{"k",2}},"system int map") ;


void test_config() {

    NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "before " << g_int_value_config->getValue() ;
    NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "before " <<  g_float_value_config->toString() ;
    
    auto& v = g_vec_value_config->getValue() ;
    for ( auto& i : v ) {
        NNYSL_LOG_DEBUG(NNYSL_LOG_ROOT()) << "test" ;
        NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "int_vec :" << i ;
    }

#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v ) { \
            NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << #prefix " "  #name " : " << i ; \
        } \
        NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString() ; \
    } 
#define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v ) { \
            NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << #prefix " "  #name " : " << i.first << " - " << i.second ; \
        } \
        NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString() ; \
    } 
    XX(g_vec_value_config, int_vec, before ) ;
    XX(g_list_value_config, int_list , before) ;
    XX(g_set_value_config, int_set, before)  ;
    XX(g_unordered_set_value_config, int_unordered_set, before)  ;
    XX_M(g_map_value_config, int_map, before)  ;



    YAML::Node root = YAML::LoadFile("/home/nnysl/final/final_designer/bin/conf/log.yaml") ;
    nnysl::Config::LoadFromYaml(root) ;
    

    NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "after " << g_int_value_config->getValue() ;
    NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "after " << g_float_value_config->toString() ;
    XX(g_vec_value_config, int_vec, after ) ;
    XX(g_list_value_config, int_list , after) ; 
    XX(g_set_value_config, int_set, after)  ;
    XX(g_unordered_set_value_config, int_unordered_set, after)  ;
    XX_M(g_map_value_config, int_map, after)  ;
#undef XX
}


class Person{
public:
    std::string m_name = "赵凯朦" ;
    int m_age = 0;
    bool m_sex = 0;
    
    bool operator==(const Person& oth) const {
        return m_name == oth.m_name && m_age == oth.m_age && m_sex == oth.m_sex ; 
    } 

    std::string toString() const {
        std::stringstream ss ;
        ss << "[Person name=" << m_name 
        << " age = " << m_age 
        << " sex = " << m_sex 
        << " ]" ;
        return ss.str() ;

    }
};

namespace nnysl {

template<>
class LexicalCast<std::string, Person>
{
public:
    Person operator() (const std::string& v ) {
        YAML::Node node = YAML::Load(v);
        Person p ;
        p.m_name = node["name"].as<std::string>() ;
        p.m_age  = node["age"].as<int>() ;
        p.m_sex  = node["sex"].as<bool>() ;

        return p ;
    }
};

template<>
class LexicalCast<Person , std::string>
{
public:
    std::string operator() (const Person& v ) {
        YAML::Node node ;
        node["name"] = v.m_name ;
        node["age"] = v.m_age ;
        node["sex"] = v.m_sex ;
        std::stringstream ss ;
        
        ss << node ;
        return ss.str() ;

    }
};

}

// nnysl::ConfigVar<Person>::ptr g_person = 
//     nnysl::Config::Lookup("class.person", Person() , "system person") ;

// nnysl::ConfigVar<std::map<std::string , Person> >::ptr g_person_map = 
//     nnysl::Config::Lookup("class.map", std::map<std::string , Person>(), "system person") ;


// void test_class() {
    
//     g_person->addListener(10, [](const Person& old_value, const Person& new_value) {
//         NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << " old value = " << old_value.toString() << " new value = " << new_value.toString() ;

//     });
    
//     NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "before" << g_person->getValue().toString() << " - " << g_person->toString() ;
    
//     YAML::Node root = YAML::LoadFile("/home/nnysl/final/final_designer/bin/conf/log.yaml") ;
//     nnysl::Config::LoadFromYaml(root) ;

//     NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "after" << g_person->getValue().toString() << " - " << g_person->toString() ;
    

// }
**/

void test_log() {
    static nnysl::Logger::ptr system_log = NNYSL_LOG_NAME("system") ;
    NNYSL_LOG_INFO(system_log) << "hello system" << std::endl; 

    std::cout << nnysl::LoggerMgr::GetInstance()->toYamlString() << std::endl; 
    YAML::Node root = YAML::LoadFile("/home/nnysl/final/final_designer/bin/conf/log.yaml") ;
    nnysl::Config::LoadFromYaml(root) ;
    std::cout << nnysl::LoggerMgr::GetInstance()->toYamlString() << std::endl; 
    
    NNYSL_LOG_INFO(system_log) << "hello system" << std::endl; 
    std::cout << "-----------------------" << std::endl; 

    nnysl::Config::Visit([](nnysl::ConfigVarBase::ptr var) {
        NNYSL_LOG_DEBUG(NNYSL_LOG_ROOT()) << " name = " << var->getName() 
        << "description= " << var->getDescription() << " typename=" <<var->getTypeName()
        << " value=" << var->toString() ;
    });
}

int main() {
    // NNYSL_LOG_ROOT() ;
    // NNYSL_LOG_INFO(NNYSL_LOG_ROOT() ) << "test test " ;
    // test_config() ;
    // test_class() ;
    test_log() ;   
    return 0 ;
}