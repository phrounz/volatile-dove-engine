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
    vec4 diffuseTexel = texture2D(colorMap, gl_TexCoord[0].st) * gl_Color;
	vec4 diffuseWithLight = diffuseTexel;

	if (usesLight != 0)
	{
		float finalAlpha = diffuseTexel.a;
		diffuseWithLight = diffuseTexel * lightMagnitude;
		diffuseWithLight.a = finalAlpha;
	}

	float distToFogReferencePos = length(vertexToFogReferencePos);
    float fogFactor = clamp((1.0 - (fogEnd-distToFogReferencePos)) / (fogEnd - fogStart), 0.0, 1.0);
	vec4 diffuseFinal  = diffuseWithLight * (1.0-fogFactor) + fogColor * fogFactor;
    diffuseFinal.a = diffuseFinal.a;
	gl_FragColor = diffuseFinal;
}
