#ifndef slic3r_AvoidCrossingPerimeters_hpp_
#define slic3r_AvoidCrossingPerimeters_hpp_

#include "../libslic3r.h"
#include "../ExPolygon.hpp"
#include "../EdgeGrid.hpp"

namespace Slic3r {

// Forward declarations.
class GCode;
class Layer;
class Point;

class AvoidCrossingPerimeters
{
public:
    // Routing around the objects vs. inside a single object.
    void        use_external_mp(bool use = true) { m_use_external_mp = use; };
    void        use_external_mp_once()  { m_use_external_mp_once = true; }
    void        disable_once()          { m_disabled_once = true; }
    bool        disabled_once() const   { return m_disabled_once; }
    void        reset_once_modifiers()  { m_use_external_mp_once = false; m_disabled_once = false; }

    void        init_layer(const Layer &layer);
    bool        is_init() { return m_init; }

    Polyline    travel_to(const GCode& gcodegen, const Point& point)
    {
        bool could_be_wipe_disabled;
        return this->travel_to(gcodegen, point, &could_be_wipe_disabled);
    }

    Polyline    travel_to(const GCode& gcodegen, const Point& point, bool* could_be_wipe_disabled);

    struct Boundary {
        // Collection of boundaries used for detection of crossing perimeters for travels
        Polygons boundaries;
        // Bounding box of boundaries
        BoundingBoxf bbox;
        // Precomputed distances of all points in boundaries
        std::vector<std::vector<float>> boundaries_params;
        // Used for detection of intersection between line and any polygon from boundaries
        EdgeGrid::Grid grid;
        //used to move the point inside the boundary
        std::vector<std::pair<ExPolygon, ExPolygons>> boundary_growth;

        void clear()
        {
            boundaries.clear();
            boundaries_params.clear();
            boundary_growth.clear();
        }
    };

private:
    bool           m_use_external_mp { false };
    // just for the next travel move
    bool           m_use_external_mp_once { false };
    // this flag disables avoid_crossing_perimeters just for the next travel move
    // we enable it by default for the first travel move in print
    bool           m_disabled_once { true };

    bool m_init{ false };

    // Used for detection of line or polyline is inside of any polygon.
    EdgeGrid::Grid m_grid_lslice;
    // Store all needed data for travels inside object
    Boundary m_internal;
    // Store all needed data for travels outside object
    Boundary m_external;
};

} // namespace Slic3r

#endif // slic3r_AvoidCrossingPerimeters_hpp_
