uniform mat4x4 cameraMatrix;
uniform float fogStart;
uniform float fogEnd;
uniform vec3 data;
uniform vec4 fogColor;
uniform int usesLight;
uniform int userDefinedTextureId;
uniform sampler2D colorMap;

varying vec3 vertexToFogReferencePos;
varying float lightMagnitude;

void main(void)
{
	vec2 cPos;
	
	vec4 vertexPos4 = gl_ModelViewMatrix * gl_Vertex;
    vertexToFogReferencePos = vec3(vertexPos4.x, vertexPos4.y, vertexPos4.z);

    vec3 normal = normalize(gl_NormalMatrix * vec3(gl_Normal.x, gl_Normal.y, gl_Normal.z));
	vec4 normalWorldCoord = vec4(normal.x, normal.y, normal.z, 0.0) * cameraMatrix;
	normal = normalize(normalWorldCoord.xyz);
	
	vec3 lightDirection = normalize(vec3(-0.7, -1.0, -0.5));// direction of the sun
	lightMagnitude = 0.45 + clamp(dot(normal, -lightDirection), 0.0, 1.0) * 0.55;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;

	if (userDefinedTextureId>0)
	{
		if (userDefinedTextureId==1) //mushrooms mode
		{
			vec4 worldPos = inverse(cameraMatrix) * gl_ModelViewMatrix * gl_Vertex;
			float timeDur = data.x;
			vec4 displacedVertex;
			displacedVertex.x = worldPos.x + 3*cos(worldPos.x/100+timeDur/1000) + 3*sin(worldPos.y/100+timeDur/1000);
			displacedVertex.y = worldPos.y + 3*cos(worldPos.y/100+timeDur/1000) + 3*sin(worldPos.z/100+timeDur/1000);
			displacedVertex.z = worldPos.z + 3*cos(worldPos.z/100+timeDur/1000) + 3*sin(worldPos.x/100+timeDur/1000);
			displacedVertex.w = 1.0;
			gl_Position = gl_ProjectionMatrix * cameraMatrix * displacedVertex;
		}
		else if (userDefinedTextureId==2) // water
		{
			vec4 worldPos = inverse(cameraMatrix) * gl_ModelViewMatrix * gl_Vertex;
			float timeDur = data.x;
			vec2 cPos = -1.0 + 2.0 * gl_TexCoord[0].xy / vec2(10.0,10.0);
			float cLength = length(cPos);
			gl_TexCoord[0].xy = gl_TexCoord[0].xy + (cPos/cLength)*cos(cLength*12.0-timeDur*0.005)*0.03;
			gl_Position.y = gl_Position.y + 1.0*cos( worldPos.x*1000+timeDur/100 ) + 1.0*sin( worldPos.z*1000+timeDur/100 );
		}
		else if (userDefinedTextureId==3) //reduced mushrooms mode
		{
			vec4 worldPos = inverse(cameraMatrix) * gl_ModelViewMatrix * gl_Vertex;
			float timeDur = data.x;
			vec4 displacedVertex;
			displacedVertex.x = worldPos.x + 0.3*sin(worldPos.x*10+timeDur/1000);
			displacedVertex.y = worldPos.y + 0.3*sin(worldPos.y*10+timeDur/1000);
			displacedVertex.z = worldPos.z + 0.3*sin(worldPos.z*10+timeDur/1000);
			displacedVertex.w = 1.0;
			gl_Position = gl_ProjectionMatrix * cameraMatrix * displacedVertex;
		}
	}

	gl_FrontColor = gl_Color;
}
