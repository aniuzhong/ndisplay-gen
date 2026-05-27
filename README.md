# ndisplay-gen

C++ library to build Unreal Engine `.ndisplay` JSON (serialize, optional deserialize, and a small generator from simplified JSON input).

## License

Source code in this repository is licensed under the [MIT License](LICENSE). Golden template files under `tests/templates/` may include or derive from Unreal/Epic content; see [`tests/templates/README.md`](tests/templates/README.md).

## Build

```bash
cmake -B build
cmake --build build
```

Run tests (after build):

```bash
ctest --test-dir build -C Release
```

## Usage

```cpp
#include "model.h"
#include "serialize.h"

using namespace ndisplay;

int main() {
    Configuration cfg;
    cfg.description = "My config";
    cfg.asset_path = "/Game/NDisplay/MyConfig.MyConfig";
    cfg.xforms.push_back({"root", "", {0, 0, 0}, {0, 0, 0}});
    cfg.screens.push_back({"screen_0", "root", {200, 112.5}, {100, 0, 50}, {}});
    cfg.cameras.push_back({"DefaultViewPoint", "", {0, 0, 50}, {}});

    Viewport vp;
    vp.name = "vp_0";
    vp.camera_ref = "DefaultViewPoint";
    vp.region = {0, 0, 1280, 720};
    vp.projection.type = ProjectionType::kSimple;
    vp.projection.screen_ref = "screen_0";

    Node node;
    node.name = "node_0";
    node.host = "127.0.0.1";
    node.window = {100, 100, 1280, 720};
    node.viewports.push_back(vp);
    cfg.nodes.push_back(node);
    cfg.primary_node.id = "node_0";

    ToFile(cfg, "output.ndisplay");
}
```

## Golden tests

Expected outputs live under `tests/templates/` (`NDC_*.ndisplay`). See [`tests/templates/README.md`](tests/templates/README.md) for provenance and licensing notes.
