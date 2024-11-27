#version 410 core

out vec4 FragColor;

in vec4 color;
in float dist;

void main()
{
    // vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    // float d = dot( circCoord, circCoord );
    // if ( dot( circCoord, circCoord ) > 1.0 ) {
    //     discard;
    // }

    FragColor = color; // flat circle
    
    // FragColor = color * vec4( 1.0f, 1.0f, 1.0f, 1.0f - d ); // faded circle
    // FragColor = color * vec4( 1.0f, 1.0f, 1.0f, 1.0f / pow(dist, 0.9f) ); // light falloff
    // FragColor = color * vec4( 1.0f, 1.0f, 1.0f, (1.0f - d) / pow(dist, 1.0f) ); // faded and falloff
}
