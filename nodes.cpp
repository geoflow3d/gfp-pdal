#include <memory>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/io/EptReader.hpp>
#include <pdal/Options.hpp>


#include <iomanip>

#include "nodes.hpp"

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
  #if __has_include(<filesystem>)
    #define GHC_USE_STD_FS
    #include <filesystem>
    namespace fs = std::filesystem;
  #endif
#endif
#ifndef GHC_USE_STD_FS
  #include <ghc/filesystem.hpp>
  namespace fs = ghc::filesystem;
#endif

namespace geoflow::nodes::gfpdal {

void EptLoaderNode::process(){
  pdal::Options options;
  options.add("filename", "ept://" + dirpath);

  pdal::EptReader reader;
  reader.setOptions(options);

  const pdal::QuickInfo qi(reader.preview());

  std::cout << "EPT Bounds: " << qi.m_bounds << std::endl;
  std::cout << "EPT Point Count: " << qi.m_pointCount << std::endl;
}


void EptVecLoaderNode::process(){
}

}