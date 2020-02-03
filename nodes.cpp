#include <memory>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/Options.hpp>
#include <pdal/filters/DecimationFilter.hpp>
#include <pdal/filters/RangeFilter.hpp>
#include <pdal/io/EptReader.hpp>


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
  PointCollection points;
  vec1i classification;
  vec1f intensity;
  vec1f order;
  vec3f colors;

  // Init a Pipeline and build it up based on the user parameters. Then we
  // need to execute the Pipeline itself, and not the individual readers or
  // filters.
  pdal::PipelineManager mgr;

  // limit to a small extent for testing
  pdal::BOX2D bounds(93876.35291718, 434066.13498522, 94084.78298842, 434329.92274137);

  pdal::EptReader reader;
  {
    pdal::Options options;
    options.add("filename", "ept://" + dirpath);
    options.add("bounds", bounds);
    reader.setOptions(options);
  }

  // Give quick overview of the EPT data in the console
  const pdal::QuickInfo qi(reader.preview());
  std::cout << std::endl << "EPT Bounds:\t" << qi.m_bounds << std::endl;
  std::cout << "EPT Point Count:\t" << qi.m_pointCount << std::endl;
  std::cout << "EPT WKT:\t" << qi.m_srs.getWKT() << std::endl;

  // Keep every Nth (step) point. If filter_step == 1 then keep all points.
  pdal::DecimationFilter filterDecimation;
  pdal::Options decimationOps;
  decimationOps.add("step", filter_step);
  filterDecimation.setOptions(decimationOps);
  filterDecimation.setInput(reader);

  // Range filter. See https://pdal.io/stages/filters.range.html
  pdal::RangeFilter filter;
  bool has_range = filter_limits.length() > 0;
  if (has_range) {
    // Range filter
    pdal::Options rangeOps;
    rangeOps.add("limits", filter_limits);
    filter.setOptions(rangeOps);
    filter.setInput(filterDecimation);

    // Reading the points now.
    pdal::PointTable eptTable;
    filter.prepare(eptTable);
    const auto set(filter.execute(eptTable));

    // The PointViews contain the actual data, there is a 1-to-1 relationship
    // to the LAZ files of the octree
    double x, y, z;
    float r, g, b;
    uint64_t o;
    uint64_t np(0);
    bool found_offset = manager.data_offset.has_value();
    for (const pdal::PointViewPtr& view : set)
    {
      for (pdal::point_count_t i(0); i < view->size(); ++i)
      {
        ++np;

        r = view->getFieldAs<float>(pdal::Dimension::Id::Red, i);
        g = view->getFieldAs<float>(pdal::Dimension::Id::Green, i);
        b = view->getFieldAs<float>(pdal::Dimension::Id::Blue, i);

        x = view->getFieldAs<double>(pdal::Dimension::Id::X, i);
        y = view->getFieldAs<double>(pdal::Dimension::Id::Y, i);
        z = view->getFieldAs<double>(pdal::Dimension::Id::Z, i);

        // TODO: Is this the equivalent of 'order' in the LAS reader plugin?
        o = view->getFieldAs<uint64_t>(pdal::Dimension::Id::OriginId, i);

        order.push_back(float(o)/1000);

        points.push_back({
                           float(x - (*manager.data_offset)[0]),
                           float(y - (*manager.data_offset)[1]),
                           float(z - (*manager.data_offset)[2])}
        );

        colors.push_back({
                           r/65535,
                           g/65535,
                           b/65535
                         });

        classification.push_back(
          view->getFieldAs<int>(pdal::Dimension::Id::Classification, i)
        );
        intensity.push_back(
          view->getFieldAs<float>(pdal::Dimension::Id::Intensity, i)
        );
      }
    }

    std::cout << "Read " << np << " points" << std::endl;

  } else {
    // TODO: ugly duplicate code here, but I don't know how to construct and
    //  then exectue a pipleline.

    std::cout << "Not using the range filter" << std::endl;

    // Reading the points now.
    pdal::PointTable eptTable;
    filterDecimation.prepare(eptTable);
    const auto set(filterDecimation.execute(eptTable));

    // The PointViews contain the actual data, there is a 1-to-1 relationship
    // to the LAZ files of the octree
    double x, y, z;
    float r, g, b;
    uint64_t o;
    uint64_t np(0);
    bool found_offset = manager.data_offset.has_value();
    for (const pdal::PointViewPtr& view : set)
    {
      for (pdal::point_count_t i(0); i < view->size(); ++i)
      {
        ++np;

        r = view->getFieldAs<float>(pdal::Dimension::Id::Red, i);
        g = view->getFieldAs<float>(pdal::Dimension::Id::Green, i);
        b = view->getFieldAs<float>(pdal::Dimension::Id::Blue, i);

        x = view->getFieldAs<double>(pdal::Dimension::Id::X, i);
        y = view->getFieldAs<double>(pdal::Dimension::Id::Y, i);
        z = view->getFieldAs<double>(pdal::Dimension::Id::Z, i);

        // TODO: Is this the equivalent of 'order' in the LAS reader plugin?
        o = view->getFieldAs<uint64_t>(pdal::Dimension::Id::OriginId, i);

        order.push_back(float(o)/1000);

        points.push_back({
                           float(x - (*manager.data_offset)[0]),
                           float(y - (*manager.data_offset)[1]),
                           float(z - (*manager.data_offset)[2])}
        );

        colors.push_back({
                           r/65535,
                           g/65535,
                           b/65535
                         });

        classification.push_back(
          view->getFieldAs<int>(pdal::Dimension::Id::Classification, i)
        );
        intensity.push_back(
          view->getFieldAs<float>(pdal::Dimension::Id::Intensity, i)
        );
      }
    }

    std::cout << "Read " << np << " points" << std::endl;
  }

  output("points").set(points);
  output("colors").set(colors);
  output("classification").set(classification);
  output("intensity").set(intensity);
  output("order").set(order);

}


void EptVecLoaderNode::process(){
}

}