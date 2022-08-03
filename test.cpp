

#include "soraxas_toolbox/print_utils.h"


#include "include/SimpleYamlConfigReader.h"

using namespace sxs;


int main() {

    using config = StaticConfigReader<CT_STR("/home/tin/git-repos/simpleConfigReader-cpp/test.yaml"), CT_STR(
            "/home/tin/git-repos/simpleConfigReader-cpp/second.yaml") >;

    using config2 = StaticConfigReader<CT_STR("/home/tin/git-repos/simpleConfigReader-cpp/test.yaml") >;


    println("here");

    println(config::get<CT_STR("yes2"), double>());


//    println(config::get<CT_STR("yes2"), bool>());



    println(config::get<CT_STR("yes"), std::string>());
    println(config::get<CT_STR("yes"), std::string>());
    println(config::get<CT_STR("yes3"), std::string>());
    println(config::get<CT_STR("yes3"), bool>());

//    println(config2::get<CT_STR("yes3"), bool>());


    println("ok");
}