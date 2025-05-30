#version 120

uniform float zoom;

varying vec3 ec_pos;


void main() {
    //Color of the cutting plane
    vec3 cbase3 = vec3(1.0, 0.0, 0.0);
    vec3 cbase2 = vec3(1.0, 0.0, 0.0);
    vec3 cbase00 = vec3(0.5, 0.0, 0.0);

    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    ec_normal.z *= zoom;
    ec_normal = normalize(ec_normal);

    float a = dot(ec_normal, vec3(0.0, 0.0, 1.0));
    float b = dot(ec_normal, vec3(-0.57, -0.57, 0.57));


	gl_FragColor = vec4((a*cbase2 + (1-a)*cbase00)*0.5 +
                        (b*cbase3 + (1-b)*cbase00)*0.5, 1.0);
}
