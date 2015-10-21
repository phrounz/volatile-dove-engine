uniform mat4x4 cameraMatrix;
uniform float fogStart;
uniform float fogEnd;
uniform vec3 cameraPos;
uniform vec4 fogColor;
uniform int usesLight;
uniform sampler2D colorMap;

varying vec3 vertexToFogReferencePos;
varying float lightMagnitude;

void main(void)
{
	vec4 vertexPos4 = gl_ModelViewMatrix * gl_Vertex;
    vertexToFogReferencePos = vec3(vertexPos4.x, vertexPos4.y, vertexPos4.z);

    vec3 normal = normalize(gl_NormalMatrix * vec3(gl_Normal.x, gl_Normal.y, gl_Normal.z));
	vec4 normalWorldCoord = vec4(normal.x, normal.y, normal.z, 0.0) * cameraMatrix;
	normal = normalize(normalWorldCoord.xyz);
	
	vec3 lightDirection = normalize(vec3(-0.7, -1.0, -0.5));// direction of the sun
	lightMagnitude = 0.45 + clamp(dot(normal, -lightDirection), 0.0, 1.0) * 0.55;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_FrontColor = gl_Color;
}
