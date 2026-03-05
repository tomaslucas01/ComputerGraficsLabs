// Global variables from CPU
uniform mat4 u_model;
uniform sampler2D u_texture;
uniform sampler2D u_normal;

uniform vec3 k_ambient;
uniform vec3 k_diffuse;
uniform vec3 k_specular;
uniform int shininess;

uniform vec3 ambient_intensity;
uniform vec3 light_intensity;

uniform vec3 light_pos;
uniform vec3 eye_pos;

uniform int use_specular_text;

// This variables comes from the vertex shader
// They are baricentric interpolated by pixel according to the distance to every vertex
varying vec2 v_uv;
varying vec3 v_world_normal;
varying vec3 v_world_position;


void main()
{
    vec4 text_col = texture2D(u_texture, v_uv);
    
    vec3 text_norm = texture2D(u_normal, v_uv).xyz;
    vec3 local_norm = text_norm * 2.0 - 1.0; // Map to -1 to 1
    vec3 N = normalize(u_model * vec4(local_norm, 0.0)).xyz;

	// vec3 N = normalize(v_world_normal);
    vec3 P = v_world_position;

    vec3 L = normalize(light_pos - P);
    vec3 V = normalize(eye_pos - P);
    vec3 R = normalize(2.0 * (clamp(dot(L, N), 0.0, 1.0) * N) - L);

    float d = distance(P, light_pos);

    vec3 Ip = text_col.xyz * clamp(dot(L, N), 0.0, 1.0); // To avoid negative dot product
    
    vec3 spec_col = (use_specular_text == 1)
        ? vec3(text_col.a)
        : k_specular;

    Ip += spec_col * pow(clamp(dot(R, V), 0.0, 1.0), shininess);
    
    Ip *= (light_intensity / (d * d));
    Ip += (text_col.xyz * ambient_intensity);

    gl_FragColor = vec4(Ip, 1.0);
}
