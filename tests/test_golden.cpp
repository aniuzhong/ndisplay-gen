#include <catch2/catch_all.hpp>
#include "test_json_compare.h"
#include "serialize.h"
#include "model.h"
#include <fstream>
#include <string>

using namespace ndisplay;

namespace {

std::string TemplatePath(const std::string& filename) {
    return std::string(NDISPLAY_PROJECT_DIR) + "/tests/templates/" + filename;
}

nlohmann::json ReadFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("failed to open file: " + path);
    }
    nlohmann::json j;
    f >> j;
    return j;
}

void CheckGolden(const Configuration& cfg, const std::string& template_name) {
    auto generated = ToJson(cfg);
    auto expected = ReadFile(TemplatePath(template_name));
    auto diffs = test::Compare(generated, expected);
    if (!diffs.empty()) {
        for (const auto& d : diffs) {
            WARN(d.path << ": " << d.detail);
        }
    }
    REQUIRE(diffs.empty());
}

}  // namespace

TEST_CASE("Golden: NDC_Basic") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_Basic.NDC_Basic";

    cfg.xforms.push_back({"nDisplayXform", "", {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"nDisplayScreen", "nDisplayXform", {200, 112.5F}, {100, 0, 50}, {0, 0, 0}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {-20, 0, 50}, {0, 0, 0}});

    Node node;
    node.name = "Node_0";
    node.host = "127.0.0.1";
    node.window = {0, 0, 1920, 1080};
    node.sound = true;
    node.full_screen = true;
    node.viewports.push_back({"VP_0", "DefaultViewPoint", {0, 0, 1920, 1080},
                              {ProjectionType::kSimple, "nDisplayScreen"}});
    cfg.nodes.push_back(node);
    cfg.primary_node = {"Node_0", 41001, 41003, 41004};

    CheckGolden(cfg, "NDC_Basic.ndisplay");
}

TEST_CASE("Golden: NDC_DualMonitor") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_DualMonitor.NDC_DualMonitor";

    cfg.xforms.push_back({"nDisplayXform", "", {100, 0, 50}, {0, 0, 0}});
    cfg.screens.push_back({"nDisplayScreenRight", "nDisplayXform", {100, 56.25F}, {0, 50, 0}, {0, 0, 8.583069e-06F}});
    cfg.screens.push_back({"nDisplayScreenLeft", "nDisplayXform", {100, 56.25F}, {0, -50, 0}, {0, 0, 0}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    Node node;
    node.name = "Node_0";
    node.host = "127.0.0.1";
    node.window = {0, 0, 5120, 1440};
    node.sound = true;
    node.full_screen = false;
    node.viewports.push_back({"VP_Left", "DefaultViewPoint", {0, 0, 2560, 1440},
                              {ProjectionType::kSimple, "nDisplayScreenLeft"}});
    node.viewports.push_back({"VP_Right", "DefaultViewPoint", {2560, 0, 2560, 1440},
                              {ProjectionType::kSimple, "nDisplayScreenRight"}});
    cfg.nodes.push_back(node);
    cfg.primary_node = {"Node_0", 41001, 41003, 41004};

    CheckGolden(cfg, "NDC_DualMonitor.ndisplay");
}

TEST_CASE("Golden: NDC_DualAppWindows") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_DualAppWindows.NDC_DualAppWindows";
    cfg.override_viewports_from_external_config = false;

    cfg.xforms.push_back({"nDisplayXform", "", {100, 0, 50}, {0, 0, 0}});
    cfg.screens.push_back({"nDisplayScreenRight", "nDisplayXform", {50, 56.25F}, {0, 25, 0}, {0, 0, 8.583069e-06F}});
    cfg.screens.push_back({"nDisplayScreenLeft", "nDisplayXform", {50, 56.25F}, {0, -25, 0}, {0, 0, 0}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    Node node0;
    node0.name = "Node_0";
    node0.host = "127.0.0.1";
    node0.window = {0, 0, 960, 1080};
    node0.sound = true;
    node0.full_screen = false;
    node0.viewports.push_back({"VP_Left", "DefaultViewPoint", {0, 0, 960, 1080},
                               {ProjectionType::kSimple, "nDisplayScreenLeft"}});
    cfg.nodes.push_back(node0);

    Node node1;
    node1.name = "Node_1";
    node1.host = "127.0.0.1";
    node1.window = {960, 0, 960, 1080};
    node1.sound = false;
    node1.full_screen = false;
    node1.viewports.push_back({"VP_Right", "DefaultViewPoint", {0, 0, 960, 1080},
                               {ProjectionType::kSimple, "nDisplayScreenRight"}});
    cfg.nodes.push_back(node1);

    cfg.primary_node = {"Node_0", 41001, 41003, 41004};
    cfg.failover = Failover{"Disabled"};

    CheckGolden(cfg, "NDC_DualAppWindows.ndisplay");
}

TEST_CASE("Golden: NDC_WallCurved3x2") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_WallCurved3x2.NDC_WallCurved3x2";

    cfg.xforms.push_back({"displays_origin", "", {0, 0, 50}, {0, 0, 0}});
    cfg.xforms.push_back({"eye_level", "displays_origin", {0, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"column_2", "eye_level", {100, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"display_2_up", "column_2", {0, 0, 15.5F}, {0, 0, 0}});
    cfg.xforms.push_back({"display_2_dn", "column_2", {0, 0, -15.5F}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_1", "eye_level", {100, -25.5F, 0}, {0, -30.000006F, 0}});
    cfg.xforms.push_back({"column_1", "angle_1", {-5.960464e-07F, -25.5F, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"display_1_up", "column_1", {0, 0, 15.5F}, {0, 0, 0}});
    cfg.xforms.push_back({"display_1_dn", "column_1", {0, 0, -15.5F}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_3", "eye_level", {100, 25.5F, 0}, {0, 30.000006F, 0}});
    cfg.xforms.push_back({"column_3", "angle_3", {1.788139e-06F, 25.5F, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"display_3_up", "column_3", {0, 0, 15.5F}, {0, 0, 0}});
    cfg.xforms.push_back({"display_3_dn", "column_3", {0, 0, -15.5F}, {0, 0, 0}});

    cfg.screens.push_back({"scr_1_up", "display_1_up", {50, 30.000002F}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_1_dn", "display_1_dn", {50, 30.000002F}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_2_up", "display_2_up", {50, 30.000002F}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_2_dn", "display_2_dn", {50, 30.000002F}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_3_up", "display_3_up", {50, 30.000002F}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_3_dn", "display_3_dn", {50, 30.000002F}, {0, 0, 0}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    auto makeNode = [&](const char* name, const char* host,
                        int wx, int wy, int ww, int wh,
                        bool sound, const char* vpName, const char* screenRef) {
        Node n;
        n.name = name;
        n.host = host;
        n.window = {wx, wy, ww, wh};
        n.sound = sound;
        n.full_screen = false;
        n.viewports.push_back({vpName, "DefaultViewPoint", {0, 0, 1920, 1080},
                               {ProjectionType::kSimple, screenRef}});
        cfg.nodes.push_back(n);
    };

    makeNode("node_1_up", "192.168.1.1", 0, 0, 1920, 1080, true, "vp_1_up", "scr_1_up");
    makeNode("node_1_dn", "192.168.1.1", 0, 1080, 1920, 1080, false, "vp_1_dn", "scr_1_dn");
    makeNode("node_2_up", "192.168.1.2", 0, 0, 1920, 1080, false, "vp_2_up", "scr_2_up");
    makeNode("node_2_dn", "192.168.1.2", 0, 1080, 1920, 1080, false, "vp_2_dn", "scr_2_dn");
    makeNode("node_3_up", "192.168.1.3", 0, 0, 1920, 1080, false, "vp_3_up", "scr_3_up");
    makeNode("node_3_dn", "192.168.1.3", 0, 1080, 1920, 1080, false, "vp_3_dn", "scr_3_dn");

    cfg.primary_node = {"node_1_up", 41001, 41003, 41004};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_WallCurved3x2.ndisplay");
}

TEST_CASE("Golden: NDC_Cave") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_Cave.NDC_Cave";

    cfg.xforms.push_back({"cave_origin", "", {0, 0, 10}, {0, 0, 0}});
    cfg.xforms.push_back({"cave_center", "cave_origin", {0, 0, 100}, {0, 0, 0}});
    cfg.xforms.push_back({"display_front", "cave_center", {150, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"display_floor", "cave_center", {0, 0, -100}, {-90, 0, 0}});
    cfg.xforms.push_back({"angle_left", "cave_center", {150, -150, 0}, {0, -89.999992F, 0}});
    cfg.xforms.push_back({"display_left", "angle_left", {-3.552714e-15F, -150, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_right", "cave_center", {150, 150, 0}, {0, 90, 0}});
    cfg.xforms.push_back({"display_right", "angle_right", {0, 150, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_ceiling", "cave_center", {150, 0, 100}, {90, 0, 0}});
    cfg.xforms.push_back({"display_ceiling", "angle_ceiling", {-7.629395e-06F, 0, 74.999985F}, {0, 0, 0}});

    cfg.screens.push_back({"scr_left", "display_left", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_front", "display_front", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_right", "display_right", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_ceiling", "display_ceiling", {300, 150}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_floor", "display_floor", {300, 300}, {0, 0, 0}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    auto makeNode = [&](const char* name, const char* host, int ww, int wh,
                        bool sound, const char* vpName, const char* screenRef,
                        int rw, int rh) {
        Node n;
        n.name = name;
        n.host = host;
        n.window = {0, 0, ww, wh};
        n.sound = sound;
        n.full_screen = true;
        n.viewports.push_back({vpName, "DefaultViewPoint", {0, 0, rw, rh},
                               {ProjectionType::kSimple, screenRef}});
        cfg.nodes.push_back(n);
    };

    makeNode("node_left", "192.168.1.1", 2560, 1600, true, "vp_left", "scr_left", 2400, 1600);
    makeNode("node_front", "192.168.1.2", 2560, 1600, false, "vp_front", "scr_front", 2400, 1600);
    makeNode("node_right", "192.168.1.3", 2560, 1600, false, "vp_right", "scr_right", 2400, 1600);
    makeNode("node_ceiling", "192.168.1.4", 2560, 1600, false, "vp_ceiling", "scr_ceiling", 2560, 1280);
    makeNode("node_floor", "192.168.1.5", 2560, 1600, false, "vp_floor", "scr_floor", 1600, 1600);

    cfg.primary_node = {"node_left", 41001, 41003, 41004};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_Cave.ndisplay");
}

TEST_CASE("Golden: NDC_CaveUnwrap") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_CaveUnwrap.NDC_CaveUnwrap";

    cfg.xforms.push_back({"cave_origin", "", {0, 0, 10}, {0, 0, 0}});
    cfg.xforms.push_back({"cave_center", "cave_origin", {0, 0, 100}, {0, 0, 0}});
    cfg.xforms.push_back({"display_front", "cave_center", {150, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"display_floor", "cave_center", {0, 0, -100}, {-90, 0, 0}});
    cfg.xforms.push_back({"angle_left", "cave_center", {150, -150, 0}, {0, -89.999992F, 0}});
    cfg.xforms.push_back({"display_left", "angle_left", {-1.525879e-05F, -149.999969F, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_right", "cave_center", {150, 150, 0}, {0, 89.999992F, 0}});
    cfg.xforms.push_back({"display_right", "angle_right", {0, 150, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_ceiling", "cave_center", {150, 0, 100}, {90, 0, 0}});
    cfg.xforms.push_back({"display_ceiling", "angle_ceiling", {-7.629395e-06F, 0, 74.999985F}, {0, 0, 0}});

    cfg.screens.push_back({"scr_left", "display_left", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_front", "display_front", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_right", "display_right", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_ceiling", "display_ceiling", {300, 150}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_floor", "display_floor", {300, 300}, {0, 0, 0}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    auto makeNode = [&](const char* name, int wx, int wy, int ww, int wh,
                        bool sound, const char* vpName, const char* screenRef,
                        int rw, int rh) {
        Node n;
        n.name = name;
        n.host = "127.0.0.1";
        n.window = {wx, wy, ww, wh};
        n.sound = sound;
        n.full_screen = true;
        n.viewports.push_back({vpName, "DefaultViewPoint", {0, 0, rw, rh},
                               {ProjectionType::kSimple, screenRef}});
        cfg.nodes.push_back(n);
    };

    makeNode("node_left", 0, 300, 600, 400, true, "vp_left", "scr_left", 600, 400);
    makeNode("node_front", 1200, 304, 600, 400, false, "vp_front", "scr_front", 600, 400);
    makeNode("node_right", 600, 900, 600, 400, false, "vp_right", "scr_right", 600, 400);
    makeNode("node_ceiling", 600, 0, 600, 300, false, "vp_ceiling", "scr_ceiling", 600, 300);
    makeNode("node_floor", 600, 300, 600, 600, false, "vp_floor", "scr_floor", 600, 600);

    cfg.primary_node = {"node_left", 41001, 41003, 41004};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_CaveUnwrap.ndisplay");
}

TEST_CASE("Golden: NDC_XRStage") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_XRStage.NDC_XRStage";

    cfg.xforms.push_back({"stage_origin", "", {0, 0, 10}, {0, 0, 0}});
    cfg.xforms.push_back({"cave_center", "stage_origin", {0, 0, 100}, {0, 0, 0}});
    cfg.xforms.push_back({"display_front", "cave_center", {150, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"display_floor", "cave_center", {0, 0, -100}, {-90, 0, 0}});
    cfg.xforms.push_back({"angle_left", "cave_center", {150, -150, 0}, {0, -89.999992F, 0}});
    cfg.xforms.push_back({"display_left", "angle_left", {-1.525879e-05F, -149.999969F, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_right", "cave_center", {150, 150, 0}, {0, 89.999992F, 0}});
    cfg.xforms.push_back({"display_right", "angle_right", {0, 150, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"angle_ceiling", "cave_center", {150, 0, 100}, {90, 0, 0}});
    cfg.xforms.push_back({"display_ceiling", "angle_ceiling", {-7.629395e-06F, 0, 74.999985F}, {0, 0, 0}});
    cfg.xforms.push_back({"ICVFXCamera", "", {-188.343582F, -132.228271F, 60}, {0, 40.000137F, 0}});

    cfg.screens.push_back({"scr_left", "display_front", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_right", "display_right", {300, 200}, {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr_floor", "display_floor", {300, 300}, {0, 0, 0}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    Node node;
    node.name = "node_main";
    node.host = "192.168.1.1";
    node.window = {0, 0, 3840, 2160};
    node.sound = true;
    node.full_screen = true;
    node.viewports.push_back({"vp_left", "DefaultViewPoint", {0, 0, 1350, 900},
                              {ProjectionType::kMesh, "", "", "", 0.1F, "scr_left"}});
    node.viewports.push_back({"vp_right", "DefaultViewPoint", {0, 900, 1350, 900},
                              {ProjectionType::kMesh, "", "", "", 0.1F, "scr_right"}});
    node.viewports.push_back({"vp_floor", "DefaultViewPoint", {1350, 0, 1350, 1350},
                              {ProjectionType::kMesh, "", "", "", 0.1F, "scr_floor"}});
    cfg.nodes.push_back(node);

    cfg.primary_node = {"node_main", 41001, 41003, 41004};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_XRStage.ndisplay");
}

TEST_CASE("Golden: NDC_MultiViewports") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/NDC_MultiViewports.NDC_MultiViewports";
    cfg.override_viewports_from_external_config = false;

    cfg.xforms.push_back({"cave_origin", "", {0, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"screens", "cave_origin", {0, 0, 170}, {0, 0, 0}});

    cfg.screens.push_back({"scr_LT", "screens", {95, 95}, {100, -50, 50}, {0, 0, 0}});
    cfg.screens.push_back({"scr_LB", "screens", {95, 95}, {100, -50, -50}, {0, 0, 0}});
    cfg.screens.push_back({"scr_RT", "screens", {95, 95}, {100, 50, 50}, {0, 0, 0}});
    cfg.screens.push_back({"scr_RB", "screens", {95, 95}, {100, 50, -50}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    {
        Node n;
        n.name = "Node_0_LT";
        n.host = "127.0.0.1";
        n.window = {0, 0, 640, 480};
        n.sound = true;
        n.full_screen = false;
        n.viewports.push_back({"VP_0_LT", "DefaultViewPoint", {10, 10, 300, 220},
                               {ProjectionType::kSimple, "scr_LT"}});
        cfg.nodes.push_back(n);
    }
    {
        Node n;
        n.name = "node_1_LB";
        n.host = "127.0.0.1";
        n.window = {0, 482, 640, 480};
        n.sound = false;
        n.full_screen = false;
        n.viewports.push_back({"VP_1_LT", "DefaultViewPoint", {10, 10, 300, 220},
                               {ProjectionType::kSimple, "scr_LT"}});
        n.viewports.push_back({"VP_1_LB", "DefaultViewPoint", {10, 250, 300, 220},
                               {ProjectionType::kSimple, "scr_LB"}});
        cfg.nodes.push_back(n);
    }
    {
        Node n;
        n.name = "node_2_RT";
        n.host = "127.0.0.1";
        n.window = {642, 0, 640, 480};
        n.sound = false;
        n.full_screen = false;
        n.viewports.push_back({"VP_2_LB", "DefaultViewPoint", {10, 250, 300, 220},
                               {ProjectionType::kSimple, "scr_LB"}});
        n.viewports.push_back({"VP_2_RT", "DefaultViewPoint", {330, 10, 300, 220},
                               {ProjectionType::kSimple, "scr_RT"}});
        cfg.nodes.push_back(n);
    }
    {
        Node n;
        n.name = "node_3_RB";
        n.host = "127.0.0.1";
        n.window = {642, 482, 640, 480};
        n.sound = false;
        n.full_screen = false;
        n.viewports.push_back({"VP_3_LT", "DefaultViewPoint", {10, 10, 300, 220},
                               {ProjectionType::kSimple, "scr_LT"}});
        n.viewports.push_back({"VP_3_RB", "DefaultViewPoint", {330, 250, 300, 220},
                               {ProjectionType::kSimple, "scr_RB"}});
        n.viewports.push_back({"VP_3_LB", "DefaultViewPoint", {10, 250, 300, 220},
                               {ProjectionType::kSimple, "scr_LB"}});
        n.viewports.push_back({"VP_3_RT", "DefaultViewPoint", {330, 10, 300, 220},
                               {ProjectionType::kSimple, "scr_RT"}});
        cfg.nodes.push_back(n);
    }

    cfg.primary_node = {"Node_0_LT", 41001, 41003, 41004};
    cfg.failover = Failover{"Disabled"};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_MultiViewports.ndisplay");
}

TEST_CASE("Golden: NDC_DemoEasyBlend1") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/EasyBlend/NDC_DemoEasyBlend1.NDC_DemoEasyBlend1";

    cfg.xforms.push_back({"cave_origin", "", {0, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"easyblend_origin_1", "", {0, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"easyblend_origin_2", "", {0, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"easyblend_origin_3", "", {0, 0, 0}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    Node node;
    node.name = "node_main";
    node.host = "127.0.0.1";
    node.window = {0, 0, 1920, 360};
    node.sound = false;
    node.full_screen = false;
    node.viewports.push_back({"vp_1", "DefaultViewPoint", {0, 0, 640, 360},
                              {ProjectionType::kEasyBlend, "", "LocalCalibrationCylinder\\ScalableData.pol", "easyblend_origin_1", 0.1F, ""}});
    node.viewports.push_back({"vp_2", "DefaultViewPoint", {640, 0, 640, 360},
                              {ProjectionType::kEasyBlend, "", "LocalCalibrationCylinder\\ScalableData.pol_1", "easyblend_origin_1", 0.1F, ""}});
    node.viewports.push_back({"vp_3", "DefaultViewPoint", {1280, 0, 640, 360},
                              {ProjectionType::kEasyBlend, "", "LocalCalibrationCylinder\\ScalableData.pol_2", "easyblend_origin_1", 0.1F, ""}});
    cfg.nodes.push_back(node);

    cfg.primary_node = {"node_main", 41001, 41003, 41004};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_DemoEasyBlend1.ndisplay");
}

TEST_CASE("Golden: NDC_DemoEasyBlend2") {
    Configuration cfg;
    cfg.description = "nDisplay configuration";
    cfg.asset_path = "/Game/ExampleConfigs/EasyBlend/NDC_DemoEasyBlend2.NDC_DemoEasyBlend2";

    cfg.xforms.push_back({"cave_origin", "", {0, 0, 0}, {0, 0, 0}});
    cfg.xforms.push_back({"easyblend_origin", "", {0, 0, 0}, {0, 0, 0}});

    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});

    auto makeNode = [&](const char* name, int wx, int wy, const char* vpName,
                        const char* file) {
        Node n;
        n.name = name;
        n.host = "127.0.0.1";
        n.window = {wx, wy, 640, 360};
        n.sound = false;
        n.full_screen = false;
        n.viewports.push_back({vpName, "DefaultViewPoint", {0, 0, 640, 360},
                               {ProjectionType::kEasyBlend, "", file, "easyblend_origin", 0.1F, ""}});
        cfg.nodes.push_back(n);
    };

    makeNode("node_1", 0, 0, "vp_1", "LocalCalibrationFlat\\ScalableData.pol");
    makeNode("node_2", 640, 0, "vp_2", "LocalCalibrationFlat\\ScalableData.pol_1");
    makeNode("node_3", 1280, 0, "vp_3", "LocalCalibrationFlat\\ScalableData.pol_2");

    cfg.primary_node = {"node_1", 41001, 41003, 41004};
    cfg.custom_params["SampleArg1"] = "SampleVal1";
    cfg.custom_params["SampleArg2"] = "SampleVal2";
    cfg.diagnostics = {false, 0.01F, 0.5F};

    CheckGolden(cfg, "NDC_DemoEasyBlend2.ndisplay");
}
