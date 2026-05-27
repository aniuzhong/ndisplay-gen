#include <catch2/catch_all.hpp>
#include "model.h"
#include "serialize.h"
#include "deserialize.h"
#include "test_json_compare.h"

using namespace ndisplay;

TEST_CASE("Serialize round-trip: Basic config", "[serialize]") {
    Configuration cfg;
    cfg.description = "test";
    cfg.asset_path = "/Game/Test.Test";
    cfg.xforms.push_back({"root", "", {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"scr", "root", {100, 56.25}, {0, 0, 0}, {0, 0, 0}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {0, 0, 0}});
    cfg.nodes.push_back({"node0", "127.0.0.1", {0, 0, 1920, 1080}, true, true, {}});
    cfg.nodes[0].viewports.push_back({
        "vp0", "DefaultViewPoint", {0, 0, 1920, 1080},
        {ProjectionType::kSimple, "scr", "", "", 0.1F, ""}
    });
    cfg.primary_node = {"node0", 41001, 41003, 41004};

    auto json = ToJson(cfg);
    auto restored = FromJson(json);

    REQUIRE(restored.description == cfg.description);
    REQUIRE(restored.asset_path == cfg.asset_path);
    REQUIRE(restored.xforms.size() == 1);
    REQUIRE(restored.xforms[0].name == "root");
    REQUIRE(restored.xforms[0].parent_id == "");
    REQUIRE(restored.screens.size() == 1);
    REQUIRE(restored.screens[0].name == "scr");
    REQUIRE(restored.screens[0].size.w == 100.0F);
    REQUIRE(restored.nodes.size() == 1);
    REQUIRE(restored.nodes[0].name == "node0");
    REQUIRE(restored.nodes[0].viewports.size() == 1);
    REQUIRE(restored.nodes[0].viewports[0].projection.type == ProjectionType::kSimple);
    REQUIRE(restored.nodes[0].viewports[0].projection.screen_ref == "scr");
    REQUIRE(restored.primary_node.id == "node0");
}

TEST_CASE("Serialize all projection types", "[serialize]") {
    // Simple
    {
        ProjectionPolicy p{ProjectionType::kSimple, "scr", "", "", 0.1F, ""};
        Configuration cfg;
        cfg.description = "";
        cfg.asset_path = "";
        cfg.xforms.push_back({"xf", "", {}, {}});
        cfg.screens.push_back({"scr", "xf", {100, 50}, {}, {}});
        cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
        cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
        cfg.nodes[0].viewports.push_back({"vp", "DefaultViewPoint", {0, 0, 100, 50}, p});
        cfg.primary_node = {"n", 41001, 41003, 41004};

        auto json = ToJson(cfg);
        auto vp = json["nDisplay"]["cluster"]["nodes"]["n"]["viewports"]["vp"];
        REQUIRE(vp["projectionPolicy"]["type"] == "simple");
        REQUIRE(vp["projectionPolicy"]["parameters"]["screen"] == "scr");
    }

    // EasyBlend
    {
        ProjectionPolicy p{ProjectionType::kEasyBlend, "", "LocalCalibration.pol", "eb_origin", 0.2F, ""};
        Configuration cfg;
        cfg.description = "";
        cfg.asset_path = "";
        cfg.xforms.push_back({"xf", "", {}, {}});
        cfg.xforms.push_back({"eb_origin", "", {}, {}});
        cfg.screens.clear();
        cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
        cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
        cfg.nodes[0].viewports.push_back({"vp", "DefaultViewPoint", {0, 0, 640, 360}, p});
        cfg.primary_node = {"n", 41001, 41003, 41004};

        auto json = ToJson(cfg);
        auto vp = json["nDisplay"]["cluster"]["nodes"]["n"]["viewports"]["vp"];
        REQUIRE(vp["projectionPolicy"]["type"] == "easyblend");
        REQUIRE(vp["projectionPolicy"]["parameters"]["file"] == "LocalCalibration.pol");
        REQUIRE(vp["projectionPolicy"]["parameters"]["origin"] == "eb_origin");
        REQUIRE(vp["projectionPolicy"]["parameters"]["scale"] == "0.2");
    }

    // Mesh
    {
        ProjectionPolicy p{ProjectionType::kMesh, "", "", "", 0.1F, "scr_left"};
        Configuration cfg;
        cfg.description = "";
        cfg.asset_path = "";
        cfg.xforms.push_back({"xf", "", {}, {}});
        cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
        cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
        cfg.nodes[0].viewports.push_back({"vp", "DefaultViewPoint", {0, 0, 1350, 900}, p});
        cfg.primary_node = {"n", 41001, 41003, 41004};

        auto json = ToJson(cfg);
        auto vp = json["nDisplay"]["cluster"]["nodes"]["n"]["viewports"]["vp"];
        REQUIRE(vp["projectionPolicy"]["type"] == "Mesh");
        REQUIRE(vp["projectionPolicy"]["parameters"]["mesh_component"] == "scr_left");
    }
}

TEST_CASE("Serialize/deserialize with custom_params", "[serialize]") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "";
    cfg.xforms.push_back({"xf", "", {}, {}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
    cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
    cfg.primary_node = {"n"};
    cfg.custom_params["key1"] = "val1";
    cfg.custom_params["key2"] = "val2";

    auto json = ToJson(cfg);
    auto restored = FromJson(json);

    REQUIRE(restored.custom_params.size() == 2);
    REQUIRE(restored.custom_params["key1"] == "val1");
    REQUIRE(restored.custom_params["key2"] == "val2");
}

TEST_CASE("Serialize empty custom_params produces empty object", "[serialize]") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "";
    cfg.xforms.push_back({"xf", "", {}, {}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
    cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
    cfg.primary_node = {"n"};

    auto json = ToJson(cfg);
    REQUIRE(json["nDisplay"]["customParameters"].is_object());
    REQUIRE(json["nDisplay"]["customParameters"].empty());
}

TEST_CASE("Serialize failover when present and absent", "[serialize]") {
    // With failover
    {
        Configuration cfg;
        cfg.description = "";
        cfg.asset_path = "";
        cfg.xforms.push_back({"xf", "", {}, {}});
        cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
        cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
        cfg.primary_node = {"n"};
        cfg.failover = Failover{"Disabled"};

        auto json = ToJson(cfg);
        REQUIRE(json["nDisplay"]["cluster"].contains("failover"));
        REQUIRE(json["nDisplay"]["cluster"]["failover"]["failoverPolicy"] == "Disabled");
    }

    // Without failover
    {
        Configuration cfg;
        cfg.description = "";
        cfg.asset_path = "";
        cfg.xforms.push_back({"xf", "", {}, {}});
        cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
        cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
        cfg.primary_node = {"n"};

        auto json = ToJson(cfg);
        REQUIRE_FALSE(json["nDisplay"]["cluster"].contains("failover"));
    }
}

TEST_CASE("Serialize stereo camera properties", "[serialize]") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "";
    cfg.xforms.push_back({"xf", "", {}, {}});
    cfg.cameras.push_back({"StereoCam", "", {0, 0, 50}, {0, 0, 0},
                                                  6.4F, false, StereoOffset::kLeft});
    cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
    cfg.primary_node = {"n"};

    auto json = ToJson(cfg);
    auto cam = json["nDisplay"]["scene"]["cameras"]["StereoCam"];
    REQUIRE(cam["stereoOffset"] == "left");
    REQUIRE(cam["swapEyes"] == false);
    REQUIRE(cam["interpupillaryDistance"].get<double>() == Catch::Approx(6.4));
}

TEST_CASE("Serialize node.sound and fullScreen", "[serialize]") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "";
    cfg.xforms.push_back({"xf", "", {}, {}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
    cfg.nodes.push_back({"n_sound", "127.0.0.1", {}, true, false, {}});
    cfg.nodes.push_back({"n_nosound", "127.0.0.1", {}, false, true, {}});
    cfg.primary_node = {"n_sound"};

    auto json = ToJson(cfg);
    auto n1 = json["nDisplay"]["cluster"]["nodes"]["n_sound"];
    auto n2 = json["nDisplay"]["cluster"]["nodes"]["n_nosound"];
    REQUIRE(n1["sound"] == true);
    REQUIRE(n1["fullScreen"] == false);
    REQUIRE(n2["sound"] == false);
    REQUIRE(n2["fullScreen"] == true);
}

TEST_CASE("Serialize diagnostics", "[serialize]") {
    Configuration cfg;
    cfg.description = "";
    cfg.asset_path = "";
    cfg.xforms.push_back({"xf", "", {}, {}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {}, {}});
    cfg.nodes.push_back({"n", "127.0.0.1", {}, true, true, {}});
    cfg.primary_node = {"n"};
    cfg.diagnostics.simulate_lag = true;
    cfg.diagnostics.min_lag_time = 0.01F;
    cfg.diagnostics.max_lag_time = 0.5F;

    auto json = ToJson(cfg);
    auto diag = json["nDisplay"]["diagnostics"];
    REQUIRE(diag["simulateLag"] == true);
    REQUIRE(diag["maxLagTime"].get<double>() == Catch::Approx(0.5));
}

TEST_CASE("Serialize then deserialize then serialize round-trip", "[serialize]") {
    Configuration config;
    config.description = "";
    config.asset_path = "/Game/ExampleConfigs/NDC_Basic.NDC_Basic";
    config.xforms.push_back({"nDisplayXform", "", {0, 0, 0}, {0, 0, 0}});
    config.screens.push_back({"nDisplayScreen", "nDisplayXform", {200, 112.5F}, {100, 0, 50}, {0, 0, 0}});
    config.cameras.push_back({"DefaultViewPoint", "", {-20, 0, 50}, {0, 0, 0}});
    Node node;
    node.name = "Node_0";
    node.host = "127.0.0.1";
    node.window = {0, 0, 1920, 1080};
    node.sound = true;
    node.full_screen = true;
    node.viewports.push_back({"VP_0", "DefaultViewPoint", {0, 0, 1920, 1080},
                              {ProjectionType::kSimple, "nDisplayScreen", "", "", 0.1F, ""}});
    config.nodes.push_back(node);
    config.primary_node = {"Node_0", 41001, 41003, 41004};

    auto first_json = ToJson(config);
    auto restored = FromJson(first_json);
    auto second_json = ToJson(restored);

    auto diffs = ndisplay::test::Compare(first_json, second_json);
    INFO("Round-trip diffs: " << diffs.size());
    for (const auto& d : diffs) {
        INFO(d.path << ": " << d.detail);
    }
    REQUIRE(diffs.empty());
}

TEST_CASE("FromJson handles non-string customParameters", "[deserialize]") {
    // Create a minimal JSON with mixed-type custom params
    nlohmann::json j;
    j["nDisplay"]["description"] = "";
    j["nDisplay"]["version"] = "5.00";
    j["nDisplay"]["assetPath"] = "/Game/Test.Test";
    j["nDisplay"]["misc"]["bFollowLocalPlayerCamera"] = false;
    j["nDisplay"]["misc"]["bExitOnEsc"] = true;
    j["nDisplay"]["scene"]["xforms"] = nlohmann::json::object();
    j["nDisplay"]["scene"]["cameras"] = nlohmann::json::object();
    j["nDisplay"]["scene"]["screens"] = nlohmann::json::object();
    j["nDisplay"]["cluster"]["primaryNode"]["id"] = "n";
    j["nDisplay"]["cluster"]["primaryNode"]["ports"]["ClusterSync"] = 41001;
    j["nDisplay"]["cluster"]["primaryNode"]["ports"]["ClusterEventsJson"] = 41003;
    j["nDisplay"]["cluster"]["primaryNode"]["ports"]["ClusterEventsBinary"] = 41004;
    j["nDisplay"]["cluster"]["sync"]["renderSyncPolicy"]["type"] = "ethernet";
    j["nDisplay"]["cluster"]["sync"]["renderSyncPolicy"]["parameters"] = nlohmann::json::object();
    j["nDisplay"]["cluster"]["sync"]["inputSyncPolicy"]["type"] = "ReplicatePrimary";
    j["nDisplay"]["cluster"]["sync"]["inputSyncPolicy"]["parameters"] = nlohmann::json::object();
    j["nDisplay"]["cluster"]["network"]["ConnectRetriesAmount"] = "300";
    j["nDisplay"]["cluster"]["network"]["ConnectRetryDelay"] = "1000";
    j["nDisplay"]["cluster"]["network"]["GameStartBarrierTimeout"] = "18000000";
    j["nDisplay"]["cluster"]["network"]["FrameStartBarrierTimeout"] = "1800000";
    j["nDisplay"]["cluster"]["network"]["FrameEndBarrierTimeout"] = "1800000";
    j["nDisplay"]["cluster"]["network"]["RenderSyncBarrierTimeout"] = "1800000";
    j["nDisplay"]["cluster"]["nodes"] = nlohmann::json::object();
    j["nDisplay"]["cluster"]["nodes"]["n"]["host"] = "127.0.0.1";
    j["nDisplay"]["cluster"]["nodes"]["n"]["sound"] = true;
    j["nDisplay"]["cluster"]["nodes"]["n"]["fullScreen"] = false;
    j["nDisplay"]["cluster"]["nodes"]["n"]["window"]["x"] = 0;
    j["nDisplay"]["cluster"]["nodes"]["n"]["window"]["y"] = 0;
    j["nDisplay"]["cluster"]["nodes"]["n"]["window"]["w"] = 1920;
    j["nDisplay"]["cluster"]["nodes"]["n"]["window"]["h"] = 1080;
    j["nDisplay"]["cluster"]["nodes"]["n"]["postprocess"] = nlohmann::json::object();
    j["nDisplay"]["cluster"]["nodes"]["n"]["viewports"] = nlohmann::json::object();
    j["nDisplay"]["cluster"]["nodes"]["n"]["outputRemap"]["bEnable"] = false;
    j["nDisplay"]["cluster"]["nodes"]["n"]["outputRemap"]["dataSource"] = "mesh";
    j["nDisplay"]["cluster"]["nodes"]["n"]["outputRemap"]["staticMeshAsset"] = "";
    j["nDisplay"]["cluster"]["nodes"]["n"]["outputRemap"]["externalFile"] = "";
    j["nDisplay"]["customParameters"]["string_val"] = "hello";
    j["nDisplay"]["customParameters"]["bool_val"] = true;
    j["nDisplay"]["customParameters"]["int_val"] = 42;
    j["nDisplay"]["customParameters"]["float_val"] = 3.14;
    j["nDisplay"]["diagnostics"]["simulateLag"] = false;
    j["nDisplay"]["diagnostics"]["minLagTime"] = 0.01;
    j["nDisplay"]["diagnostics"]["maxLagTime"] = 0.3;

    // Should not throw
    auto cfg = FromJson(j);

    REQUIRE(cfg.custom_params["string_val"] == "hello");
    REQUIRE(cfg.custom_params["bool_val"] == "true");
    REQUIRE(cfg.custom_params["int_val"] == "42");
    // float_val: accept either "3.14" or "3.140000" etc.
    REQUIRE(cfg.custom_params["float_val"].find("3.14") == 0);
}
