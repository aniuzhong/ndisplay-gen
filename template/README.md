# Golden templates

`NDC_*.ndisplay` files are golden references for `test/test_golden.cpp`: output of `ToJson(Generate(LoadInputParams(fixture)))` is compared against them.

## Provenance

Aligned with (or exported from) Unreal sample content, e.g.:

`<Path to Epic Games>\UE_5.3\Templates\TP_nDisplayBP\Content`

## License

- **Generator and tests in this repo**: follow the root `LICENSE` if present.
- **Unreal / Epic sample material**: subject to the [Unreal Engine EULA](https://www.unrealengine.com/eula) and any other Epic agreements. Redistribution of engine or marketplace assets must comply with those terms.
- Third-party assets in the source configs require your own clearance before redistributing derived `.ndisplay` files here.

This file is attribution only, not legal advice.
