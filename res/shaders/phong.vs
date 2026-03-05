// Global variables from the CPU
uniform mat4 u_model;
uniform mat4 u_viewprojection;
uniform int use_specular_text;

// Variables to pass to the fragment shader
varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

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

	// Project the vertex using the model view projection matrix
	gl_Position = u_viewprojection * vec4(P, 1.0); //output of the vertex shader


}