// hello_wall.cpp — single flat wall driven by a 90-degree horizontal FOV
//
// The camera is at the origin facing +X.  With a 90° H-FOV the screen sits
// 100 units in front: width = 2*100*tan(45°) = 200, 16:9 → height = 112.5.
// The projection is a simple mapping from a 1920x1080 viewport to the screen.

#include "model.h"
#include "serialize.h"

using namespace ndisplay;

int main() {
    Configuration cfg;

    cfg.description = "Single flat wall, 90-degree horizontal FOV";
    cfg.asset_path = "/Game/NDisplay/HelloWall.HelloWall";

    // Root xform
    cfg.xforms.push_back({"root", "", {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}});

    // Camera at origin
    cfg.cameras.push_back({"cam_1", "", {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}});

    // Screen: 90° H-FOV → distance 100, width = 2*100*tan(45°) = 200, 16:9
    cfg.screens.push_back({"wall_screen", "root",
                           {200.0F, 112.5F},
                           {100.0F, 0.0F, 0.0F},
                           {0.0F, 0.0F, 0.0F}});

    // Viewport + projection
    Viewport vp;
    vp.name = "vp_1";
    vp.camera_ref = "cam_1";
    vp.region = {0, 0, 1920, 1080};
    vp.projection.type = ProjectionType::kSimple;
    vp.projection.screen_ref = "wall_screen";
    cfg.nodes.push_back({"node_1", "127.0.0.1",
                         {0, 0, 1920, 1080},
                         true, true, {vp}});

    cfg.primary_node.id = "node_1";

    ToFile(cfg, "hello_wall.ndisplay");
    return 0;
}
