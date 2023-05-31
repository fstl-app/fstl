#version 120

uniform float zoom;
uniform vec4 ambient_light_color;
uniform vec4 directive_light_color;
uniform vec3 directive_light_direction;

varying vec3 ec_pos;

void main() {
    // Normalize light direction
    vec3 dir = normalize(directive_light_direction);

    // vec3 a = vec3(0.0, 1.0, 1.0);
    // normal vector
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    ec_normal.z *= zoom;
    ec_normal = normalize(ec_normal);


    vec3 color =  ambient_light_color.w * ambient_light_color.xyz + directive_light_color.w * dot(ec_normal,dir) * directive_light_color.xyz;

    // float coef = dot(ec_normal,dir);
    // vec3 color = coef * lightcolor + (1.0 - coef) * objectcolor;
    gl_FragColor = vec4(color, 1.0);
}
