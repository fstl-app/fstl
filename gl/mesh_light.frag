#version 330

uniform float zoom;
uniform vec4 ambient_light_color;
uniform vec4 directive_light_color;
uniform vec3 directive_light_direction;
uniform bool useWire;
uniform vec3 wireColor;
uniform float wireWidth;

in vec3 ec_pos;
noperspective in vec3 altitude;

void main() {
    // Normalize light direction
    vec3 dir = normalize(directive_light_direction);

    // normal vector
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    ec_normal.z *= zoom;
    ec_normal = normalize(ec_normal);


    vec3 color =  ambient_light_color.w * ambient_light_color.xyz + directive_light_color.w * dot(ec_normal,dir) * directive_light_color.xyz;

    if (useWire) {
        float d = min(min(altitude.x, altitude.y),altitude.z);
        float mixVal = smoothstep(wireWidth-1.0, wireWidth+1.0,d);
        color = mix(wireColor,color,mixVal);
    }

    gl_FragColor = vec4(color, 1.0);
}
