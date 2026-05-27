// cube_room_cluster.cpp — same cube room as cube_room.cpp, but one node per wall
//
// Each of the four walls is driven by a dedicated PC with its own
// fullscreen 1920x1080 output.  The scene (xforms, cameras, screens)
// is identical to the single-machine cube_room.

#include "model.h"
#include "serialize.h"

using namespace ndisplay;

int main() {
    Configuration cfg;

    cfg.description = "Cube room with four nodes driving four walls";
    cfg.asset_path = "/Game/NDisplay/CubeRoomCluster.CubeRoomCluster";

    // ── Scene (identical to cube_room) ──
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

    // ── Four nodes, each with one viewport → one wall ──
    auto make_node = [](const std::string& name, const std::string& host,
                        const std::string& vp_name, const std::string& screen,
                        bool sound) {
        Viewport vp;
        vp.name = vp_name;
        vp.camera_ref = "cam_center";
        vp.region = {0, 0, 1920, 1080};
        vp.projection.type = ProjectionType::kSimple;
        vp.projection.screen_ref = screen;

        Node node;
        node.name = name;
        node.host = host;
        node.window = {0, 0, 1920, 1080};
        node.sound = sound;
        node.full_screen = true;
        node.viewports.push_back(vp);
        return node;
    };

    cfg.nodes.push_back(make_node("node_front", "192.168.0.101",
                                  "vp_front", "front_wall", true));
    cfg.nodes.push_back(make_node("node_back",  "192.168.0.102",
                                  "vp_back",  "back_wall",  false));
    cfg.nodes.push_back(make_node("node_left",  "192.168.0.103",
                                  "vp_left",  "left_wall",  false));
    cfg.nodes.push_back(make_node("node_right", "192.168.0.104",
                                  "vp_right", "right_wall", false));

    cfg.primary_node.id = "node_front";

    ToFile(cfg, "cube_room_cluster.ndisplay");
    return 0;
}
