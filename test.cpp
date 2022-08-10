

#include "YamlConfigReader/StaticReader.h"
#include "YamlConfigReader/StaticReaderStaticFilename.h"
#include "soraxas_toolbox/print_utils.h"

using namespace sxs;

int main()
{
    using config = StaticConfigReader<CT_STR("/home/tin/git-repos/StaticYamlConfigReader-cpp/test.yaml"),  //
                                      CT_STR("/home/tin/git-repos/StaticYamlConfigReader-cpp/second.yaml")>;

    using config2 = StaticConfigReader<CT_STR("/home/tin/git-repos/StaticYamlConfigReader-cpp/test.yaml")>;

    println("before");

    using dynconfig = DynamicConfigReader<CT_STR("my-tag")>;

    dynconfig::set_config_file("/home/tin/git-repos/StaticYamlConfigReader-cpp/test.yaml");
    println("set config");

    auto haha = dynconfig::get_instance();

    println("after init");
    haha.get<CT_STR("yes"), bool>();
    haha.get<CT_STR("yes"), double>();

    println("here");

    println(config::get<CT_STR("yes2"), double>());

    //    println(config::get<CT_STR("yes2"), bool>());

    //    println(config::get<CT_STR("yes"), std::string>());
    //    println(config::get<CT_STR("yes"), std::string>());
    //    println(config::get<CT_STR("yes3"), std::string>());
    //    println(config::get<CT_STR("yes3"), bool>());

    //    println(config2::get<CT_STR("yes3"), bool>());

    println("ok");
}