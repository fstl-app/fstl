#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 ec_pos;
noperspective out vec3 altitude;

uniform vec2 portSize;

void main() {
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    vec2 p0_f = p0.xy/p0.w;
    vec2 p1_f = p1.xy/p1.w;
    vec2 p2_f = p2.xy/p2.w;

    // Altitude calculation :
    // vp0p1 is the p0p1 vector
    // vp0p2 is the p0p2 vector
    // det(vp0p1,vp0p2) is the area of the parallelogram defined by the two vectors vp0p1 and vp0p2
    // h0 is the altitude from p0 in the triangle (p0 p1 p2)
    // h0 multiplied by p1p2 which is the length of vp0p1-vp0p2 is also the area of the parallelogram defined by the two vectors vp0p1 and vp0p2
    // this leads to h0
    //
    // portSize is used to have an altitude in pixel
    //

    // Calculate h0 altitude from p0
    vec2 vp0p1 = portSize*(p1_f-p0_f);
    vec2 vp0p2 = portSize*(p2_f-p0_f);
    float h0 = abs(determinant(mat2(vp0p1,vp0p2))) / length(vp0p1-vp0p2);
    // release values
    gl_Position = p0;
    ec_pos = gl_Position.xyz;
    altitude = vec3(h0*p0.w, 0.0, 0.0);
    EmitVertex();

    // calculate h1 altitude from p1
    vec2 vp1p0 = portSize*(p0_f-p1_f);
    vec2 vp1p2 = portSize*(p2_f-p1_f);
    float h1 = abs(determinant(mat2(vp1p0,vp1p2))) / length(vp1p0-vp1p2);
    // release values
    gl_Position = p1;
    ec_pos = gl_Position.xyz;
    altitude = vec3(0.0, h1*p1.w, 0.0);
    EmitVertex();

    // calculate h2 altitude from p2
    vec2 vp2p0 = portSize*(p0_f-p2_f);
    vec2 vp2p1 = portSize*(p1_f-p2_f);
    float h2 = abs(determinant(mat2(vp2p0,vp2p1))) / length(vp2p0-vp2p1);
    // release values
    gl_Position = p2;
    ec_pos = gl_Position.xyz;
    altitude = vec3(0.0, 0.0, h2*p2.w);
    EmitVertex();

    EndPrimitive();

}

