
#include "../nnysl/config.hpp"
#include "../nnysl/log.h"
#include <yaml-cpp/yaml.h>


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
    std::string m_name = 0 ;
    int m_age = 0;
    bool m_sex = 0;
    
    std::string toString() const {
        std::stringstream ss ;
        ss << "[Person name=" << m_name 
        << " age = " << m_age 
        << " sex = " << m_sex 
        << " ]" ;
        return ss.str() ;

    }
};

nnysl::ConfigVar<Person>::ptr g_person = 
    nnysl::Config::Lookup("class.person", Person() , "system person") ;


void test_class() {
    NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "before" << g_person->getValue().toString() << " - " << g_person->toString() ;
    
    YAML::Node root = YAML::LoadFile("/home/nnysl/final/final_designer/bin/conf/log.yaml") ;
    nnysl::Config::LoadFromYaml(root) ;

    NNYSL_LOG_INFO(NNYSL_LOG_ROOT()) << "after" << g_person->getValue().toString() << " - " << g_person->toString() ;
    

}

int main() {
    // test_config() ;
    test_class() ;
    
    return 0 ;
}