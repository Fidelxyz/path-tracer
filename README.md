# CSC317 Showcase

Modified based on [Assignment 3: Ray Tracing](https://github.com/ohnooj/computer-graphics-ray-tracing), but rewrote most of the codes.

## Features

- Monto Carlo path tracing.
  - With Russian Roulette method.
- Multiple importance sampling.
  - Explicit light sampling.
  - BSDF importance sampling (Both of cosine-weighted hemisphere distribution and GGX distribution).
- Physically-based BRDF (Cook-Torrance Model).
- BVH Accelerated.
- Anti-alising by dithering sensor pixels.
- Gamma correction.
- Loading objects and materials from OBJ and MTL files.

## Improvements

- Parallelized with OpenMP.
- Better Polymorphism architecture.
  - Eliminated all necessity for `dynamic_cast`.
- Performance improvements.
  - Replaced `std::shared_ptr` with `std::unique_ptr` or raw pointer where possible.
  - Use move semantics where possible.
  - Replaced 64-bit `double` with 32-bit `float` for floating point data.
  - Utilized Eigen's vectorized operations and lazy evaluation where possible.
- Modernized codes based on C++20 standard.

## TODO

- Texture Mapping
- DOF
- Adaptive Sampling

https://www.reddit.com/r/GraphicsProgramming/comments/ox0lvm/reducing_fireflies_in_path_tracing/

## References

Models downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data)

Monte Carlo path tracing: https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_15.pdf

Multiple Importance Sampling: https://computergraphics.stackexchange.com/questions/5152/progressive-path-tracing-with-explicit-light-sampling

Cook-Torrance BRDF: https://learnopengl.com/PBR/Theory

Cosine-weighted Importance Sampling: https://ameye.dev/notes/sampling-the-hemisphere/

Importance Sampling for GGX Distribution:

- https://agraphicsguynotes.com/posts/sample_microfacet_brdf/
- https://schuttejoe.github.io/post/ggximportancesamplingpart1/
