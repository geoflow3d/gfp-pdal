#include <geoflow/geoflow.hpp>

namespace geoflow::nodes::gfpdal {

  class EptLoaderNode:public Node {
    std::string dirpath = "";
    int filter_step=5;
    std::string filter_limits = "Classification[0:18]";
    public:
    using Node::Node;
    void init() {
      add_output("points", typeid(PointCollection));
      add_output("classification", typeid(vec1i));
      add_output("intensity", typeid(vec1f));
      add_output("order", typeid(vec1f));
      add_output("colors", typeid(vec3f));

      add_param(ParamPath(dirpath,"dirpath", "EPT directory"));
      add_param(ParamBoundedInt(filter_step, 1, 100, "filter_step", "Keep Nth (decimate)"));
      add_param(ParamString(filter_limits, "filter_limits", "PDAL Range filter"));
    }
    void process();
  };

  class EptVecLoaderNode:public Node {
    std::string ept_folder = "";
    public:
    using Node::Node;
    void init() {
      add_vector_output("point_clouds", typeid(PointCollection));

      add_param(ParamPath(ept_folder, "las_folder", "Folder with the Entwine Point Tiles"));
    }
    void process();
  };

}