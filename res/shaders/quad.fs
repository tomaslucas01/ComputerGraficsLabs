varying vec2 v_uv;
uniform float height;
uniform float width;
uniform vec2 center;

void main()
{
    vec2 pixel = gl_FragCoord.xy;


    // UV coordinates
	// gl_FragColor = vec4(v_uv, 0.0, 1.0);

    // Blue to red gradient
    // gl_FragColor = mix(vec4(0.0, 0.0, 1.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), v_uv[0]);

    // Distance from center
    // gl_FragColor = mix(vec4(0.0), vec4(1.0), distance(v_uv, vec2(0.5)));

    // Red blue lines


    // Chessboard
    vec2 grid = floor(v_uv * 8.0); // 8x8 grid
    float pattern = mod(grid.x + grid.y, 2.0);
    vec3 color = mix(vec3(1.0), vec3(0.0), pattern);

    gl_FragColor = vec4(color, 1.0);




    
}
