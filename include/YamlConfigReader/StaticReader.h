//
// Created by tin on 3/08/22.
//

#ifndef SIMPLE_YMAL_CONFIG_READER_SIMPLEYAMLCONFIGREADERDYNAMIC_H
#define SIMPLE_YMAL_CONFIG_READER_SIMPLEYAMLCONFIGREADERDYNAMIC_H

#include <yaml-cpp/yaml.h>

#include <exception>
#include <iostream>

#include "helpers.h"
#include "soraxas_toolbox/compile_time_string.h"

/**
 * USAGE:
 *
 * using Configs = sxs::StaticConfigReader<CT_STR("MyTag")>;
 * // convenient MACRO
 * #define GET_CONFIG(KEY, TYPE) gplanner::Configs::get<CT_STR(#KEY), TYPE>()
 *
 * ...
 *
 * // set the config path once.
 * Configs::set_config_file(config_filename, overriding_config_filename);
 *
 * ...
 *
 * // retrieve config value
 * auto kappa = GET_CONFIG(vonMisesFisherKappa);
 * // or
 * auto kappa = Configs::get<CT_STR(double, "vonMisesFisherKappa")>();
 *
 */

namespace sxs
{
    // prints the explanatory string of an exception. If the exception is nested,
    // recurses to print the explanatory of the exception it holds
    //    void print_exception(const std::exception& e, int level =  0)
    //    {
    //        std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    //        try {
    //            std::rethrow_if_nested(e);
    //        } catch(const std::exception& nestedException) {
    //            print_exception(nestedException, level+1);
    //        } catch(...) {}
    //    }

    class ConfigReaderException : public std::exception
    {
    };

    class ConfigReaderMissingKeyException : public ConfigReaderException
    {
    private:
        std::string message;

    public:
        explicit ConfigReaderMissingKeyException(std::string key)
        {
            message = std::string() + "The key '" + key + "' does not exists in the config file.";
        }

        const char *what() const throw()
        {
            return message.c_str();
        }
    };

    template <bool...>
    struct bool_pack;
    template <bool... bs>
    using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

    template <class... Ts>
    using are_all_compile_time_strings = all_true<sxs::is_compile_time_string<Ts>::value...>;

    /**
     * Forward declare
     *
     * @tparam ConfigReaderTag A unique tag that globally identify this config reader
     */
    template <typename ConfigReaderTag>
    class StaticConfigReader;

    /**
     * A helper class that acts as a register class that register each requested combination of
     * ConfigReader Tag + Config Key + Requested Data Type.
     * Each registered combinations at compile time will be validated at static allocation time.
     *
     * @tparam ConfigReaderTag
     * @tparam Args
     */
    template <typename ConfigReaderTag, typename... Args>
    class _StaticConfigReaderRegisterClass
    {
    protected:
        static _StaticConfigReaderRegisterClass instance;

        /**
         * Constructor that call back into the config reader to register the type.
         */
        _StaticConfigReaderRegisterClass()
        {
            StaticConfigReader<ConfigReaderTag>::template __register<Args...>();
        }

    public:
        /**
         * Public function for the main class to perform register to request a new template
         * type of this class
         *
         * @return an instance of this class, which can be discarded.
         */
        static const _StaticConfigReaderRegisterClass &doRegister()
        {
            return instance;
        }
    };

    /**
     * The public facing config reader that stack allocate all requested config combinations.
     *
     * @tparam ConfigReaderTag A globally unique tag that makes this config reader type a
     * global instance (i.e. using the same tag will always use the same underlying memory)
     */
    template <typename ConfigReaderTag>
    class StaticConfigReader
    {
        static_assert(sxs::is_compile_time_string<ConfigReaderTag>::value,  //
                      "The template argument for ConfigReaderTag must be of type compile_time_string.");

    public:
        template <typename T, typename... Key>
        static const T &get()
        {
            static_assert(sxs::are_all_compile_time_strings<Key...>::value,  //
                          "The template argument for Keys must be all of type compile_time_string.");

            static const T &thing = []
            {
                _StaticConfigReaderRegisterClass<ConfigReaderTag, T, Key...>::doRegister();

                auto &&node = accessNode(_get_root(), Key::c_str()...);

                try
                {
                    return node.template as<T>();
                }
                catch (...)
                {
                    std::cerr << "Exception: Trying to cast the key " << concatString(Key::c_str()...) << " into type "
                              << typeid(T).name() << std::endl;
                    throw;
                }
            }();
            return thing;
        }

        template <class... STR>
        static std::string concatString(const std::string &str1, STR... strs)
        {
            return str1 + ":" + concatString(strs...);
        }

        static std::string concatString(const std::string &str1)
        {
            return str1;
        }

        static void print()
        {
            //            std::stringstream ss;
            std::cout << "=== File: " << getFilepaths() << " ===" << std::endl;
            std::cout << _get_root() << std::endl;
            std::cout << "==========" << std::endl;
            //            ss.str();
        }

        //        friend std::ostream &operator<<(std::ostream &_stream, const StaticConfigReader &t)
        //        {
        //            _stream << std::string(t);
        //            return _stream;
        //        }

        template <typename T, typename... Key>
        static void __register()
        {
            static_assert(sxs::are_all_compile_time_strings<Key...>::value,  //
                          "The template argument for Keys must be all of type compile_time_string.");

            // creates lambda which will be eagerly ran once the file config is set.
            get_type_validation_container().push_back([] { get_instance().template get<T, Key...>(); });
        }

        static void set_config_file(const std::string &default_configs_filepath,
                                    const std::string &overriding_configs_filepath = "")
        {
            auto &storedPaths = getFilepaths();
            if (storedPaths.first != "")
            {
                throw std::runtime_error("Config filepath had already been set to " + storedPaths.first + "!");
            }

            storedPaths = {default_configs_filepath, overriding_configs_filepath};
            for (auto &&runnable : get_type_validation_container())
            {
                runnable();
            }
        }

    private:
        StaticConfigReader()
        {
        }

        static YAML::Node accessNode(const YAML::Node &operating_node, const std::string &key)
        {
            return operating_node[key];
        }

        template <typename... Rest>
        static YAML::Node accessNode(const YAML::Node &operating_node, const std::string &key, Rest... keys)
        {
            return accessNode(accessNode(operating_node, key), keys...);
        }

        static StaticConfigReader &get_instance()
        {
            static StaticConfigReader thing{};
            return thing;
        }

        static auto &get_type_validation_container()
        {
            static std::vector<std::function<void()>> runnables;
            return runnables;
        }

        static auto &getFilepaths()
        {
            static std::pair<std::string, std::string> filepaths = {"", ""};
            return filepaths;
        };

        static auto &_get_root()
        {
            static YAML::Node thing = []()
            {
                const auto &paths = getFilepaths();
                if (paths.first == "")
                {
                    throw std::runtime_error("Config file path had not been set yet! "
                                             "Use set_config_file(...) to set the config file path "
                                             "prior to the first call to retrieve config.");
                }
                auto root = YAML::LoadFile(paths.first);

                // load and merge the second overriding config
                if (paths.second != "")
                {
                    auto overriding_node = YAML::LoadFile(paths.second);
                    root = merge_nodes(root, overriding_node);
                }

                return root;
            }();

            return thing;
        };
    };

    /**
     * Static instance for each requested config type
     *
     * @tparam ConfigReaderTag
     * @tparam Args
     */
    template <typename ConfigReaderTag, typename... Args>
    _StaticConfigReaderRegisterClass<ConfigReaderTag, Args...>
        _StaticConfigReaderRegisterClass<ConfigReaderTag, Args...>::instance{};

#define YAML_STATIC_CONFIG_READER_GET(ReaderTag) sxs::StaticConfigReader<ReaderTag>::get<TYPE, CT_STR()>()
}  // namespace sxs

#endif  // SIMPLE_YMAL_CONFIG_READER_SIMPLEYAMLCONFIGREADERDYNAMIC_H
