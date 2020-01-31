#include "nodes.hpp"

using namespace geoflow::nodes::pdal;

void register_nodes(geoflow::NodeRegister& node_register) {
    node_register.register_node<EptLoaderNode>("EptLoader");
}