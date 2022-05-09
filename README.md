A short test project in efficient voxel rendering

Main.cpp contains two different macro defines: ``MODE_QUAD``, and ``MODE_CUBE``

``MODE_QUAD``: Uses the ray-box intersection method described in https://jcgt.org/published/0007/03/04/. Each voxel is represented by a single uvec2 in a large vertex array with one vertex per voxel. Voxels are rendered with a single ``glDrawArrays`` call using the ``GL_POINTS`` primitive. The point primitive is placed and given a size to cover the area of the cube and is then ray-traced in the fragment shader using an optimized ray-box intersection test.

``MODE_CUBE``: rasterizes cubes by drawing 3 cube faces per voxel as described by Sebastian Aaltonen: https://twitter.com/SebAaltonen/status/1315982782439591938?s=20&t=OT9EAP_VgMDqngheDqBcbA. A single large index buffer that contains 18 indices for each instance is created and bound at startup, then the actual data used for drawing the voxels is put into a large SSBO (1 uvec2 per voxel). 

## Relative Performance

Testing on Intel Integrated graphics, the two techniques are able to render approximately the same number of cubes. Using a dedicated graphics card however results in ``MODE_QUAD`` being about twice as fast as ``MODE_CUBE``.


Build instructions:

Only made for linux right now, just ``make`` and run. Relies on GLAD (bundled), GLM (bundled), GLFW (install with ``apt``) 

