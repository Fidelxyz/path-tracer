# CSC317 Showcase

Modified from A3: ray-tracing, but modified almost all files.

## Performance Improvement

- OpenMP
- Use `std::unique_ptr` instead of `std::shared_ptr` when possible
- Move data in constructor
- More functional to allow better compiler optimization (const instead of variable)
- float instead of double to save memory
- BVH

## TODO

- Texture Mapping
- PBR
- DOF
- Anti-alising

## References

Models downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data)
