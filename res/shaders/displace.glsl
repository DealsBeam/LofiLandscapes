#version 450 core

struct Vert {
    vec4 pos;
};

layout(std430, binding = 1) buffer vertexBuffer
{
    Vert verts[];
};

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

uniform sampler2D heightmap;

uniform vec2 uPos;
uniform float uScaleXZ;
uniform float uScaleY;

void main() {
    uint i = gl_GlobalInvocationID.x;

    verts[i].pos.xz += uPos;

    vec2 uv = (2.0/uScaleXZ) * verts[i].pos.xz;
    uv = 0.5*uv + 0.5;
    
    float height = 0.5 * uScaleY * texture(heightmap, uv).r;

    verts[i].pos.y = height;
}
