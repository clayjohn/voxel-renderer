#version 460

layout(std430, binding = 0) restrict readonly buffer Instances {
    // .x = X:11, Y:10, Z:11
    // .y = 
    //  ALBEDO: 16 // R5G6B5
    //  METAL: 4
    //  ROUGH: 4
    //  INTENSITY: 4
    //  TYPE: 4 - 
    uvec2 data[];
};



uniform vec3 camera_origin;
uniform mat4 view_projection_matrix;

out vec3 color;
out vec3 world_pos;
out vec3 clip_pos;

void main(){
    uint instance = (gl_VertexID >> uint(3));
    uint u_x = data[instance].x & uint(2047); // 11 bits
    uint u_y = (data[instance].x & uint(2095104)) >> uint(11); // 10 bits
    uint u_z = (data[instance].x & (uint(2047) << uint(21))) >> uint(21); // 11 bits
    vec3 world_position = vec3(float(u_x), float(u_y), float(u_z))*0.1;
    uint u_albedo = data[instance].y & uint(65535);
    uint u_red = u_albedo & uint(31);
    uint u_green = (u_albedo & uint(2016))>>uint(5);
    uint u_blue = (u_albedo & uint(63488))>>uint(11);
    vec3 albedo = vec3(float(u_red)/32.0, float(u_green)/64.0, float(u_blue)/32.0);

    vec3 local_camera_pos= camera_origin - world_position;
    uvec3 xyz = uvec3(gl_VertexID & 0x1, (gl_VertexID & 0x2) >> 1, (gl_VertexID & 0x4) >> 2);
    if (local_camera_pos.x > 0.0) {
        xyz.x = 1 - xyz.x;
    }
    if (local_camera_pos.y > 0.0) {
        xyz.y = 1 - xyz.y;
    }
    if (local_camera_pos.z > 0.0) {
        xyz.z = 1 - xyz.z;
    }
    vec3 uvw = vec3(xyz);
    vec4 position = vec4(uvw * vec3(2.0) - vec3(1.0), 1.0);
    position = view_projection_matrix * vec4(position.xyz * 0.05 + world_position, 1.0);

    clip_pos = position.xyz;
    world_pos = world_position;
    gl_Position = position;
    color = albedo;
}