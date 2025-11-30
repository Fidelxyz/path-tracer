# CSC317 Showcase

Modified from A3: ray-tracing, but modified almost all files.

## Performance Improvement

- OpenMP
- Use `std::unique_ptr` instead of `std::shared_ptr` when possible
- Modernize codes (e.g. Move data in constructor)
- float instead of double to save memory
- BVH

## Features

- Anti-alising
- PBR
- Multiple Importance Sampling
- Explicit direct light sampling
- Gamma correction

## TODO

- Texture Mapping
- DOF

https://www.reddit.com/r/GraphicsProgramming/comments/ox0lvm/reducing_fireflies_in_path_tracing/

## References

Models downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data)
