#version 120

uniform float zoom;

varying vec3 ec_pos;

void main() {
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    ec_normal.z *= zoom;
    ec_normal = normalize(ec_normal);
    //rotated 10deg around the red axis for better color match
    float x = dot(ec_normal, vec3(1.0, 0.0, 0.0));
    float y = dot(ec_normal, vec3(0.0, 0.985, 0.174));
    float z = dot(ec_normal, vec3(0.0, -0.174, 0.985));

    gl_FragColor = vec4(0.5-0.5*x, 0.5-0.5*y, 0.5+0.5*z, 1.0);
}
