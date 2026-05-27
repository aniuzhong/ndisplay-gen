#include "deserialize.h"

namespace ndisplay {

namespace {

Vec3 ReadVec3(const nlohmann::json& j) {
    return {j.value("x", 0.0F), j.value("y", 0.0F), j.value("z", 0.0F)};
}

Rot3 ReadRot3(const nlohmann::json& j) {
    return {j.value("pitch", 0.0F), j.value("yaw", 0.0F), j.value("roll", 0.0F)};
}

StereoOffset ParseStereoOffset(const std::string& s) {
    if (s == "left")  return StereoOffset::kLeft;
    if (s == "right") return StereoOffset::kRight;
    return StereoOffset::kNone;
}

Overscan ParseOverscan(const nlohmann::json& j) {
    Overscan os;
    os.enabled = j.value("bEnabled", false);
    os.left = j.value("left", 0.0F);
    os.right = j.value("right", 0.0F);
    os.top = j.value("top", 0.0F);
    os.bottom = j.value("bottom", 0.0F);
    os.oversize = j.value("oversize", true);
    return os;
}

ProjectionPolicy ParseProjectionPolicy(const nlohmann::json& j) {
    ProjectionPolicy p;
    std::string t = j.value("type", "simple");
    const auto& params = j.value("parameters", nlohmann::json::object());

    if (t == "easyblend") {
        p.type = ProjectionType::kEasyBlend;
        p.file_path = params.value("file", "");
        p.origin_ref = params.value("origin", "");
        if (params.contains("scale")) {
            const auto& s = params["scale"];
            if (s.is_string()) {
                p.scale = std::stof(s.get<std::string>());
            } else if (s.is_number()) {
                p.scale = s.get<float>();
            }
        }
    } else if (t == "Mesh") {
        p.type = ProjectionType::kMesh;
        p.mesh_component = params.value("mesh_component", "");
    } else {
        p.type = ProjectionType::kSimple;
        p.screen_ref = params.value("screen", "");
    }
    return p;
}

Viewport ReadViewport(const std::string& name, const nlohmann::json& j) {
    Viewport vp;
    vp.name = name;
    vp.camera_ref = j.value("camera", "DefaultViewPoint");
    vp.buffer_ratio = j.value("bufferRatio", 1.0F);
    vp.gpu_index = j.value("gPUIndex", -1);
    vp.allow_cross_gpu_transfer = j.value("allowCrossGPUTransfer", false);
    vp.is_shared = j.value("isShared", false);
    if (j.contains("overscan")) {
        vp.overscan = ParseOverscan(j["overscan"]);
    }
    if (j.contains("region")) {
        const auto& r = j["region"];
        vp.region = {r.value("x", 0), r.value("y", 0), r.value("w", 0), r.value("h", 0)};
    }
    if (j.contains("projectionPolicy")) {
        vp.projection = ParseProjectionPolicy(j["projectionPolicy"]);
    }
    return vp;
}

std::string CustomParamValue(const nlohmann::json& v) {
    if (v.is_string()) return v.get<std::string>();
    if (v.is_boolean()) return v.get<bool>() ? "true" : "false";
    if (v.is_number()) {
        if (v.is_number_integer()) return std::to_string(v.get<int>());
        return std::to_string(v.get<double>());
    }
    return v.dump();
}

}  // namespace

Configuration FromJson(const nlohmann::json& outer) {
    Configuration cfg;
    const auto& j = outer.at("nDisplay");

    cfg.description = j.value("description", "");
    cfg.asset_path = j.value("assetPath", "");

    // misc
    if (j.contains("misc")) {
        const auto& misc = j["misc"];
        if (misc.contains("bOverrideViewportsFromExternalConfig")) {
            cfg.override_viewports_from_external_config =
                misc.value("bOverrideViewportsFromExternalConfig", false);
        }
    }

    // scene
    if (j.contains("scene")) {
        const auto& scene = j["scene"];

        if (scene.contains("xforms")) {
            for (const auto& [name, xj] : scene["xforms"].items()) {
                Xform xf;
                xf.name = name;
                xf.parent_id = xj.value("parentId", "");
                if (xj.contains("location")) xf.location = ReadVec3(xj["location"]);
                if (xj.contains("rotation")) xf.rotation = ReadRot3(xj["rotation"]);
                cfg.xforms.push_back(xf);
            }
        }

        if (scene.contains("cameras")) {
            for (const auto& [name, cj] : scene["cameras"].items()) {
                Camera cam;
                cam.name = name;
                cam.parent_id = cj.value("parentId", "");
                cam.interpupillary_distance = cj.value("interpupillaryDistance", 6.4F);
                cam.swap_eyes = cj.value("swapEyes", false);
                if (cj.contains("stereoOffset")) {
                    cam.stereo_offset = ParseStereoOffset(cj["stereoOffset"]);
                }
                if (cj.contains("location")) cam.location = ReadVec3(cj["location"]);
                if (cj.contains("rotation")) cam.rotation = ReadRot3(cj["rotation"]);
                cfg.cameras.push_back(cam);
            }
        }

        if (scene.contains("screens")) {
            for (const auto& [name, sj] : scene["screens"].items()) {
                Screen sc;
                sc.name = name;
                sc.parent_id = sj.value("parentId", "");
                if (sj.contains("size")) {
                    const auto& sz = sj["size"];
                    sc.size = {sz.value("width", 0.0F), sz.value("height", 0.0F)};
                }
                if (sj.contains("location")) sc.location = ReadVec3(sj["location"]);
                if (sj.contains("rotation")) sc.rotation = ReadRot3(sj["rotation"]);
                cfg.screens.push_back(sc);
            }
        }
    }

    // cluster
    if (j.contains("cluster")) {
        const auto& cluster = j["cluster"];

        if (cluster.contains("primaryNode")) {
            const auto& pn = cluster["primaryNode"];
            cfg.primary_node.id = pn.value("id", "");
            if (pn.contains("ports")) {
                const auto& pts = pn["ports"];
                cfg.primary_node.port_cluster_sync = pts.value("ClusterSync", 41001);
                cfg.primary_node.port_cluster_events_json = pts.value("ClusterEventsJson", 41003);
                cfg.primary_node.port_cluster_events_binary = pts.value("ClusterEventsBinary", 41004);
            }
        }

        if (cluster.contains("failover")) {
            Failover fo;
            fo.policy = cluster["failover"].value("failoverPolicy", "Disabled");
            cfg.failover = fo;
        }

        if (cluster.contains("nodes")) {
            for (const auto& [name, nj] : cluster["nodes"].items()) {
                Node node;
                node.name = name;
                node.host = nj.value("host", "127.0.0.1");
                node.sound = nj.value("sound", true);
                node.full_screen = nj.value("fullScreen", true);
                if (nj.contains("window")) {
                    const auto& w = nj["window"];
                    node.window = {w.value("x", 0), w.value("y", 0),
                                   w.value("w", 1920), w.value("h", 1080)};
                }
                if (nj.contains("viewports")) {
                    for (const auto& [vpname, vpj] : nj["viewports"].items()) {
                        node.viewports.push_back(ReadViewport(vpname, vpj));
                    }
                }
                cfg.nodes.push_back(node);
            }
        }
    }

    // customParameters
    if (j.contains("customParameters")) {
        for (const auto& [k, v] : j["customParameters"].items()) {
            cfg.custom_params[k] = CustomParamValue(v);
        }
    }

    // diagnostics
    if (j.contains("diagnostics")) {
        const auto& d = j["diagnostics"];
        cfg.diagnostics.simulate_lag = d.value("simulateLag", false);
        cfg.diagnostics.min_lag_time = d.value("minLagTime", 0.01F);
        cfg.diagnostics.max_lag_time = d.value("maxLagTime", 0.3F);
    }

    return cfg;
}

Configuration FromFile(const std::string& path) {
    std::ifstream f(path);
    nlohmann::json j;
    f >> j;
    return FromJson(j);
}

}  // namespace ndisplay
