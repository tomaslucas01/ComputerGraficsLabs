varying vec2 v_uv;
uniform float width;
uniform float height;
uniform vec2 center;

void main()
{	
	// Remember the UV's range [0.0, 1.0]
	v_uv = gl_MultiTexCoord0.xy;
	gl_Position = gl_Vertex;
}