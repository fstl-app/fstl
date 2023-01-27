#version 120

uniform float zoom;

varying vec3 ec_pos;

void main() {
    // Light direction
    vec3 dir = vec3(-1,-1,0);
    dir = normalize(dir);
    // Light color
    vec3 color = vec3(1.0, 1.0, 1.0);

    // normal vector
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    ec_normal.z *= zoom;
    ec_normal = normalize(ec_normal);

    float lightcoeff = 0.5 * dot(ec_normal,dir);
    float diffusecoeff = 0.5;
    gl_FragColor = vec4((diffusecoeff + lightcoeff) * color, 1.0);  
}

// dir 1,0,0 eclairage à gauche
// dir -1,0,0 eclairage à droite
// dir 0,1,0 eclairage en dessous
// dir 0,-1,0 eclairage par dessus
// dir 0,0,1 eclairage par devant
// dir 0,0,-1 eclairage par derriere
