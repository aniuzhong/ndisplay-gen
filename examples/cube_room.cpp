// cube_room.cpp — four walls forming a 200x200x200 cube room, camera at center
//
// Layout (top-down, Y=up in UE):
//         back_wall (-100,0,0) yaw=180
//   left_wall (0,-100,0)       right_wall (0,100,0)
//   yaw=-90                     yaw=90
//         front_wall (100,0,0) yaw=0
//
// Each wall is 200x200 UE units. A single 1920x1080 window is
// quartered into 960x540 viewports, one per wall.

#include "model.h"
#include "serialize.h"

using namespace ndisplay;

int main() {
    Configuration cfg;

    cfg.description = "Four walls forming a cube room display";
    cfg.asset_path = "/Game/NDisplay/CubeRoom.CubeRoom";

    cfg.xforms.push_back({"root", "", {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}});

    cfg.cameras.push_back({"cam_center", "root",
                           {0.0F, 0.0F, 0.0F},
                           {0.0F, 0.0F, 0.0F}});

    cfg.screens.push_back({"front_wall", "root",
                           {200.0F, 200.0F}, {100.0F, 0.0F, 0.0F},
                           {0.0F, 0.0F, 0.0F}});
    cfg.screens.push_back({"back_wall", "root",
                           {200.0F, 200.0F}, {-100.0F, 0.0F, 0.0F},
                           {0.0F, 180.0F, 0.0F}});
    cfg.screens.push_back({"left_wall", "root",
                           {200.0F, 200.0F}, {0.0F, -100.0F, 0.0F},
                           {0.0F, -90.0F, 0.0F}});
    cfg.screens.push_back({"right_wall", "root",
                           {200.0F, 200.0F}, {0.0F, 100.0F, 0.0F},
                           {0.0F, 90.0F, 0.0F}});

    // Quarter the 1920x1080 window: each viewport gets 960x540
    Viewport vp_front;
    vp_front.name = "vp_front";
    vp_front.camera_ref = "cam_center";
    vp_front.region = {0, 0, 960, 540};
    vp_front.projection.type = ProjectionType::kSimple;
    vp_front.projection.screen_ref = "front_wall";

    Viewport vp_back;
    vp_back.name = "vp_back";
    vp_back.camera_ref = "cam_center";
    vp_back.region = {960, 0, 960, 540};
    vp_back.projection.type = ProjectionType::kSimple;
    vp_back.projection.screen_ref = "back_wall";

    Viewport vp_left;
    vp_left.name = "vp_left";
    vp_left.camera_ref = "cam_center";
    vp_left.region = {0, 540, 960, 540};
    vp_left.projection.type = ProjectionType::kSimple;
    vp_left.projection.screen_ref = "left_wall";

    Viewport vp_right;
    vp_right.name = "vp_right";
    vp_right.camera_ref = "cam_center";
    vp_right.region = {960, 540, 960, 540};
    vp_right.projection.type = ProjectionType::kSimple;
    vp_right.projection.screen_ref = "right_wall";

    cfg.nodes.push_back({"node_1", "127.0.0.1",
                         {0, 0, 1920, 1080},
                         true, true,
                         {vp_front, vp_back, vp_left, vp_right}});

    cfg.primary_node.id = "node_1";

    ToFile(cfg, "cube_room.ndisplay");
    return 0;
}
