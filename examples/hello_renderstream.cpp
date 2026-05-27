// hello_renderstream.cpp — self-hosted RenderStream minimal config (UE 5.x)
//
// Generates an .ndisplay equivalent to the self_host_ue5.ndisplay template:
// single node on 127.0.0.1, "renderstream" projection policy,
// "renderstream_capture" post-process, camera at (-2020, -30, 100).

#include "model.h"
#include "serialize.h"

using namespace ndisplay;

int main() {
    Configuration cfg;

    cfg.description = "Self-hosted RenderStream minimal config";
    cfg.asset_path = "";

    // Camera
    cfg.cameras.push_back({"camera_static", "",
                           {-2020.0F, -30.0F, 100.0F},
                           {0.0F, 0.0F, 0.0F},
                           0.064F, false, StereoOffset::kNone});

    // Viewport with renderstream projection
    Viewport vp;
    vp.name = "layer0";
    vp.camera_ref = "camera_static";
    vp.region = {0, 0, 1920, 1080};
    vp.buffer_ratio = 1.0F;
    vp.gpu_index = -1;
    vp.allow_cross_gpu_transfer = true;
    vp.is_shared = false;
    vp.projection.type = ProjectionType::kCustom;
    vp.projection.custom_type = "renderstream";

    // Node with renderstream_capture post-process
    Node node;
    node.name = "node0";
    node.host = "127.0.0.1";
    node.window = {0, 0, 1920, 1080};
    node.sound = false;
    node.full_screen = false;
    node.viewports.push_back(vp);
    node.postprocess["rs"] = {"renderstream_capture", {}};

    cfg.nodes.push_back(node);

    // Primary node
    cfg.primary_node.id = "node0";
    cfg.primary_node.port_cluster_sync = 27010;
    cfg.primary_node.port_cluster_events_json = 27012;
    cfg.primary_node.port_cluster_events_binary = 27013;

    // Network
    cfg.network.connect_retries_amount = "10";
    cfg.network.connect_retry_delay = "1000";
    cfg.network.game_start_barrier_timeout = "60000";
    cfg.network.frame_start_barrier_timeout = "10000";
    cfg.network.frame_end_barrier_timeout = "10000";
    cfg.network.render_sync_barrier_timeout = "10000";

    // Sync — single-node self-hosted: no render sync needed
    cfg.render_sync_policy = "None";

    ToFile(cfg, "hello_renderstream.ndisplay");
    return 0;
}
