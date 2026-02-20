varying vec2 v_uv;
uniform sampler2D u_texture;
uniform float u_time;

uniform float mode;
uniform float type;

vec2 rotateUV(vec2 uv, float angle) {
    uv -= 0.5;

    mat2 rotation = mat2(   cos(angle), -sin(angle),
                            sin(angle), cos(angle)  );

    uv = rotation * uv;
    uv += 0.5;
    return uv;
}

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
    float n_lines = 10.0;
    float x_sin = sin(n_lines * 2.0 * 3.1415296 * v_uv.x);  
    float y_sin = sin(n_lines * 2.0 * 3.1415296 * v_uv.y);

    // gl_FragColor = vec4(x_sin, 0.0, y_sin, 1.0);


    // Sine effect

    if (mode == 1.0) {
        
    }
    
    float sin = 0.25 * sin(v_uv.x * 2.0 * 3.1415926) + 0.5;
        float mask = step(sin, v_uv.y);
        gl_FragColor = vec4(mix(vec3(0.0, v_uv.y, 0.0), vec3(0.0, 1 - v_uv.y, 0.0), mask), 1.0);

    // Chessboard
    // vec2 grid = floor(v_uv * 8.0); // 8x8 grid
    // float pattern = mod(grid.x + grid.y, 2.0);
    // vec3 color = mix(vec3(1.0), vec3(0.0), pattern);

    // gl_FragColor = vec4(color, 1.0);


    // Fruits image
    vec4 col = texture2D(u_texture, v_uv);

    // Grey scale
    float sum = (col.r + col.g + col.b) / 3.0;
    // gl_FragColor = vec4(vec3(sum), 1.0);

    // Inverted
    vec3 invert_col = vec3(1.0) - col.xyz;
    // gl_FragColor = vec4(invert_col, 1.0);

    // Mexico filter
    float yellow = (col.x + col.y) / 2.0;
    vec3 final_col = vec3(vec2(yellow), 0.0);
    // gl_FragColor = vec4(final_col, 1.0);

    // BW step
    final_col = vec3(step(0.5, sum));
    // gl_FragColor = vec4(final_col, 1.0);

    // Vignette
    float d = distance(v_uv, vec2(0.5));
    final_col = col.xyz - vec3(d);
    // gl_FragColor = vec4(final_col, 1.0);

    // Blur
    float factor = 0.001;
    final_col = col.xyz;
    int rad = 6;

    for (int i = -rad; i <= rad; ++i) {
        for (int j = -rad; j <= rad; ++j) {
            vec2 offset = vec2(factor * i, factor * j);
            final_col += texture2D(u_texture, v_uv + offset).rgb;
        }
    }
    final_col /= pow((2 * rad + 1), 2);
    // gl_FragColor = vec4(final_col, 1.0);


    // Pixelate
    float oscilation = 0.03; 
    float i = mix(10.0, 500.0, oscilation);
    vec2 pix_uv = floor(v_uv*i)/i;
	// gl_FragColor = vec4(pix_uv, 0.0, 1.0);

    // Rotate UV effect
    vec2 uv_rot = rotateUV(v_uv, u_time);
    vec4 rot_col = texture2D(u_texture, uv_rot);
    // gl_FragColor = rot_col;
}
