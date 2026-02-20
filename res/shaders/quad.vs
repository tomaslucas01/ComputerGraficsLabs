varying vec2 v_uv;
uniform sampler2D u_texture;

uniform float mode;
uniform float type;

void main()
{	
	// Remember the UV's range [0.0, 1.0]
	v_uv = gl_MultiTexCoord0.xy;
	gl_Position = gl_Vertex;
}