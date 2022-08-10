//
// Created by tin on 3/08/22.
//

#ifndef YAML_CONFIG_HELPERS_H
#define YAML_CONFIG_HELPERS_H

#include <yaml-cpp/yaml.h>

namespace sxs
{

    inline const YAML::Node &cnode(const YAML::Node &n)
    {
        return n;
    }

    YAML::Node merge_nodes(YAML::Node a, YAML::Node b, bool strict = true);

    /////////////
}  // namespace sxs

#endif  // YAML_CONFIG_HELPERS_H
