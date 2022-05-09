#version 460

layout (location = 0) in uvec2 data;

uniform vec2 screen_size;
uniform mat4 view_projection_matrix;

//Fast Quadric Proj: "GPU-Based Ray-Casting of Quadratic Surfaces" http://dl.acm.org/citation.cfm?id=2386396
void quadricProj(in vec3 osPosition, in float voxelSize, in mat4 objectToScreenMatrix, in vec2 halfScreenSize,
                    inout vec4 position, inout float pointSize) {
    const vec4 quadricMat = vec4(1.0, 1.0, 1.0, -1.0);
    float sphereRadius = voxelSize * 1.732051;
    vec4 sphereCenter = vec4(osPosition.xyz, 1.0);
    mat4 modelViewProj = transpose(objectToScreenMatrix);
    mat3x4 matT = mat3x4( mat3(modelViewProj[0].xyz, modelViewProj[1].xyz, modelViewProj[3].xyz) * sphereRadius);
    matT[0].w = dot(sphereCenter, modelViewProj[0]);
    matT[1].w = dot(sphereCenter, modelViewProj[1]);
    matT[2].w = dot(sphereCenter, modelViewProj[3]);
    mat3x4 matD = mat3x4(matT[0] * quadricMat, matT[1] * quadricMat, matT[2] * quadricMat);
    vec4 eqCoefs = vec4(dot(matD[0], matT[2]), dot(matD[1], matT[2]), dot(matD[0], matT[0]), dot(matD[1], matT[1]))
                    / dot(matD[2], matT[2]);
    vec4 outPosition = vec4(eqCoefs.x, eqCoefs.y, 0.0, 1.0);
    vec2 AABB = sqrt(eqCoefs.xy*eqCoefs.xy - eqCoefs.zw);
    AABB *= halfScreenSize * 2.0f;
    position.xy = outPosition.xy * position.w;
    pointSize = max(AABB.x, AABB.y);
}

out vec3 color;
out vec3 world_pos;
out vec3 clip_pos;

void main(){
    uint u_x = data.x & uint(2047); // 11 bits
    uint u_y = (data.x & uint(2095104)) >> uint(11); // 10 bits
    uint u_z = (data.x & (uint(2047) << uint(21))) >> uint(21); // 11 bits
    vec3 world_position = vec3(float(u_x), float(u_y), float(u_z))*0.1;
    uint u_albedo = data.y & uint(65535);
    uint u_red = u_albedo & uint(31);
    uint u_green = (u_albedo & uint(2016))>>uint(5);
    uint u_blue = (u_albedo & uint(63488))>>uint(11);
    vec3 albedo = vec3(float(u_red)/32.0, float(u_green)/64.0, float(u_blue)/32.0);

    vec4 position = (view_projection_matrix * vec4(world_position, 1.0));
    float point_size = 100.0;
    if (position.z > 0.0) {
        quadricProj(world_position, 0.05, view_projection_matrix, screen_size * 0.5, position, point_size);

        // Square area
        float stochasticCoverage = point_size * point_size;
        if ((stochasticCoverage < 0.8) && ((gl_VertexID & 0xffff) > stochasticCoverage * (0xffff / 0.8))) {
            // "Cull" small voxels in a stable, stochastic way by moving past the z = 0 plane.
            // Assumes voxels are in randomized order.
            position = vec4(-1,-1,-1,-1);
            point_size = 0.0;
        }
    } else {
        position = vec4(-1,-1,-1,-1);
        point_size = 0.0;
    }

    clip_pos = position.xyz;
    world_pos = world_position;
    gl_Position = position;
    gl_PointSize = point_size;
    color = albedo;
}
