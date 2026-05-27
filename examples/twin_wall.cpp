// twin_wall.cpp — two adjacent flat walls, single window, dual viewport (32:9)
//
// Left wall  200×112.5 at (-100, 0, 0)
// Right wall 200×112.5 at ( 100, 0, 0)
// Camera at origin.  A single 3840×1080 window is split vertically:
//   vp_left  → left_wall  (region 0..1920)
//   vp_right → right_wall (region 1920..3840)

#include "model.h"
#include "serialize.h"

using namespace ndisplay;

int main() {
    Configuration cfg;

    cfg.description = "Two adjacent flat walls forming a wide display";
    cfg.asset_path = "/Game/NDisplay/TwinWall.TwinWall";

    cfg.xforms.push_back({"root", "", {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}});

    cfg.cameras.push_back({"cam_main", "root", {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}});

    cfg.screens.push_back({"left_wall", "root",
                           {200.0F, 112.5F},
                           {-100.0F, 0.0F, 0.0F},
                           {0.0F, 0.0F, 0.0F}});
    cfg.screens.push_back({"right_wall", "root",
                           {200.0F, 112.5F},
                           {100.0F, 0.0F, 0.0F},
                           {0.0F, 0.0F, 0.0F}});

    Viewport vp_left;
    vp_left.name = "vp_left";
    vp_left.camera_ref = "cam_main";
    vp_left.region = {0, 0, 1920, 1080};
    vp_left.projection.type = ProjectionType::kSimple;
    vp_left.projection.screen_ref = "left_wall";

    Viewport vp_right;
    vp_right.name = "vp_right";
    vp_right.camera_ref = "cam_main";
    vp_right.region = {1920, 0, 1920, 1080};
    vp_right.projection.type = ProjectionType::kSimple;
    vp_right.projection.screen_ref = "right_wall";

    cfg.nodes.push_back({"node_1", "127.0.0.1",
                         {0, 0, 3840, 1080},
                         true, true,
                         {vp_left, vp_right}});

    cfg.primary_node.id = "node_1";

    ToFile(cfg, "twin_wall.ndisplay");
    return 0;
}
