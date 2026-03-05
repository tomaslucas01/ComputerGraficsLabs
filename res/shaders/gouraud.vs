// Global variables from the CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;
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

// Variables to pass to the fragment shader
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec3 Ip;

//here create uniforms for all the data we need here

void main()
{	
	v_uv = gl_MultiTexCoord0.xy;

	// Convert local position to world space
	vec3 P = (u_model * vec4( gl_Vertex.xyz, 1.0)).xyz;

	// Convert local normal to world space
	vec3 N = (u_model * vec4( gl_Normal.xyz, 0.0)).xyz;

	// Pass them to the fragment shader interpolated
	v_world_position = P;
	v_world_normal = N;


    // Gouraud shading calculations

    N = normalize(N);
    vec3 L = normalize(light_pos - P);
    vec3 V = normalize(eye_pos - P);
    vec3 R = normalize(2 * (clamp(dot(L, N), 0.0, 1.0) * N) - L);

    float d = distance(P, light_pos);

    vec3 temp_Ip = k_diffuse * clamp(dot(L, N), 0.0, 1.0); // To avoid negative dot product
    temp_Ip += k_specular * pow(clamp(dot(R, V), 0.0, 1.0), shininess);
    temp_Ip = temp_Ip * (light_intensity / (d * d));
    temp_Ip = temp_Ip + k_ambient * ambient_intensity;

    Ip = temp_Ip;

	// Project the vertex using the model view projection matrix
	gl_Position = u_viewprojection * vec4(P, 1.0); //output of the vertex shader


}