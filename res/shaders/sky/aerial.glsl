//Physically based sky rendering following the paper 
//"Production Ready Atmosphere Rendering" by Sebastien Hillaire.

//This shader generates the aerial perspective lut
//Work in progress

#version 450 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(rgba16, binding = 0) uniform image3D aerialLUT;

uniform sampler2D transLUT;

#include "common.glsl"

uniform float uHeight;

uniform vec3 uSunDir;

uniform float uFar;
uniform float uNear;

uniform vec3 uFront;

uniform vec3 uBotLeft;
uniform vec3 uBotRight;
uniform vec3 uTopLeft;
uniform vec3 uTopRight;

uniform float uBrightness;
uniform float uDistScale;

float MiePhase(float cosTheta) {
    const float g = 0.8;
    const float scale = 3.0/(8.0*PI);
    
    float num = (1.0-g*g)*(1.0+cosTheta*cosTheta);
    float denom = (2.0+g*g)*pow((1.0 + g*g - 2.0*g*cosTheta), 1.5);
    
    return scale*num/denom;
}

float RayleighPhase(float cosTheta) {
    const float k = 3.0/(16.0*PI);
    return k*(1.0+cosTheta*cosTheta);
}

float mean(vec3 v){
    return 0.33*(v.x + v.y + v.z);
}

void main() {
    ivec3 texelCoord = ivec3(gl_GlobalInvocationID.xyz);

    //Normalized 3d coordinates [0,1]
    vec3 coord = (vec3(texelCoord)+0.5)/imageSize(aerialLUT);

    //Construct ray origin
    float h = max(uHeight, 0.000001);
    vec3 org = vec3(0.0, ground_rad + h, 0.0);

    //Retrieve ray direction
    vec3 dir = mix(mix(uBotLeft, uBotRight, coord.x), mix(uTopLeft, uTopRight, coord.x), coord.y);
    dir = normalize(dir);

    //Calculate initial and final distances

    //uNear/uFar are along z axis, so we need to project onto the ray
    //We also change units to megameters
    //And we also consider distance scale parameter
    float proj = 1.0/dot(uFront, dir);

    float t0    =            proj*0.000001*uNear;
    float t_end = uDistScale*proj*0.000001*uFar;

    float tf = t0 + coord.z*(t_end-t0);

    //Calculate step size
    const int samples_per_voxel = 5;
    
    int num_steps = samples_per_voxel * int(gl_GlobalInvocationID.z) //uint->int (small so should be fine)
                  + (samples_per_voxel + (samples_per_voxel % 2))/2;
    
    float dt = (tf - t0)/float(num_steps);

    //Consider intersections with the planet/atmosphere
    float atm_dist = IntersectSphere(org, dir, atmosphere_rad);
    float gnd_dist = IntersectSphere(org, dir, ground_rad);

    //If we didn't hit the atmosphere we may as well exit early
    if (atm_dist < 0.0)
    {
        vec4 res = vec4(vec3(0.0), 1.0);
        imageStore(aerialLUT, texelCoord, res);
        return;
    }

    float t_cutoff = atm_dist;

    //Select minimum for the cutoff dist
    if (gnd_dist > 0.0)
    {
        t_cutoff = min(t_cutoff, gnd_dist);
    }

    //Phase functions
    vec3 sun_dir = vec3(-1,1,-1) * uSunDir;
    float cosSunAngle = dot(dir, sun_dir);        
    float mie_phase      = MiePhase(-cosSunAngle);
    float rayleigh_phase = RayleighPhase(cosSunAngle);

    //Raymarching
    float transmittance = 1.0;
    vec3 in_scatter = vec3(0.0);

    //Start at the beginning of the frustum
    float t = t0;

    for (int i=0; i<= num_steps; i++)
    {
        vec3 p = org + t*dir;

        //Scattering values
        float mie_s;
        vec3 rayleigh_s, extinction;

        getScatteringValues(p, rayleigh_s, mie_s, extinction);

        vec3 rayleigh_in_s = rayleigh_s * rayleigh_phase;
        float mie_in_s     = mie_s      * mie_phase;

        //Transmittance
        vec3 sample_trans = exp(-dt*extinction);
        vec3 sun_trans = getValueFromLUT(transLUT, p, sun_dir);

        //Earth shadow
        float earth_dist = IntersectSphere(p, sun_dir, ground_rad);
        float earth_shadow = float(earth_dist < 0.0);

        //Integration
        vec3 S = earth_shadow * sun_trans * (rayleigh_in_s + mie_in_s);
        vec3 IntS = S*(1.0 - sample_trans)/extinction;

        in_scatter += transmittance * IntS;
        transmittance *= mean(sample_trans);

        t += dt;

        if (t >= t_cutoff) break;
    }

    //Save
    vec4 res = vec4(uBrightness*in_scatter, transmittance);

    imageStore(aerialLUT, texelCoord, res);
}