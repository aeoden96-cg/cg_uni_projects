#version 420 core

in vec3 Normal;
in vec4 Position;

out vec4 FragColor;

struct LightProperties {
	vec3 ambient;
	float spotCosCutoff;
	vec3 diffColor;
	float spotExponent;
	vec3 specColor;
	float constantAttenuation;
	vec3 position;
	float linearAttenuation;
	vec3 halfVector;
	float quadraticAttenuation;
	vec3 coneDirection;
	bool isEnabled;
	bool isPositional;
	bool isSpot;
};

struct MaterialProperties {
	vec4 materialColor;
	vec3 emission;
	float shininess;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// the set of lights to apply, per invocation of this shader
const int MaxLights = 3;
layout (std140, binding = 1) uniform LightBlock {
	LightProperties Lights[MaxLights];
};

// a set of materials to select between, per shader invocation
const int NumMaterials = 5;
layout (std140, binding = 2) uniform MaterialBlock {
	MaterialProperties Material[NumMaterials];
};

uniform vec3 Ambiental;
uniform vec3 EyeDirection;
uniform bool IsLocal;
uniform bool TwoSided;
uniform int MatIndex;

vec4 LightAtPoint (vec4 Position, vec3 Normal)
{
	//	
	// Position  = točka u kojoj se računa osvjetljavanje
	// Normal    = normala u točki u kojoj se računa osvjetljavanje, mora biti normalizirana
	// 
	// Potrebno je definirati globalne varijable:
	//		const int MaxLights = ....; // maksimalan broj svjetala
	//		const int NumMaterials = ...; // maksimalan broj materijala
	//
	// Potrebno je definirati uniformne varijable:
	//		uniform vec3 Ambiental; // globalna ambijentalna boja
	//		uniform vec3 EyeDirection; // položaj promatrača, mora biti normaliziran vektor za IsLocal = false;
	//		uniform bool IsLocal;  // IsLocal = true; za lokalnog promatrača
	//		uniform bool TwoSided; // dvostrano osvjetljavanje
	//		uniform int MatIndex; // indeks u polju Material
	//
	// Potrebno je još definirati uniform buffere:
	//		layout (std140, binding = 1) uniform LightBlock {LightProperties Lights[MaxLights];};
	//		layout (std140, binding = 2) uniform MaterialBlock {MaterialProperties Material[NumMaterials];};
	// gdje su:
	//		struct LightProperties {
	//			vec3 ambient;
	//			float spotCosCutoff;
	//			vec3 diffColor;
	//			float spotExponent;
	//			vec3 specColor;
	//			float constantAttenuation;
	//			vec3 position;  // mora biti normaliziran vektor za isPositional = false;
	//			float linearAttenuation;
	//			vec3 halfVector;  // mora biti normaliziran vektor
	//			float quadraticAttenuation;
	//			vec3 coneDirection;  // mora biti normaliziran vektor
	//			bool isEnabled;
	//			bool isPositional;
	//			bool isSpot;
	//		};
	//
	//		struct MaterialProperties {
	//			vec4 materialColor;
	//			vec3 emission;
	//			float shininess;
	//			vec3 ambient;
	//			vec3 diffuse;
	//			vec3 specular;
	//		};
	//

	vec3 scatteredLight = Ambiental * Material[MatIndex].ambient; 
	vec3 reflectedLight = vec3(0.0);
	vec4 Color = Material[MatIndex].materialColor;
	vec3 halfVector;
	vec3 rgb;

	vec3 EyeVector = EyeDirection;
	if (IsLocal) {
		vec3 EyeVector = normalize(EyeDirection - vec3(Position)/Position.w);
	}

	vec3 twoNormal;
    if (gl_FrontFacing || !TwoSided) {  // Front face
		twoNormal = Normal;  
    }
    else {  // Back face
		twoNormal = -Normal;  // Korekcija za dvostrano osvjetljavanje!
    }

	// loop over all the lights
	for (int light = 0; light < MaxLights; ++light) { 
		if (!Lights[light].isEnabled) {
		continue; }

		vec3 lightDirection = Lights[light].position;
		float attenuation = 1.0;

		// for positional lights, compute per-point direction,
		// halfVector, and attenuation    

		if (Lights[light].isPositional) {
			lightDirection = lightDirection - Position.xyz/Position.w; 
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;

			attenuation = 1.0 /
			(Lights[light].constantAttenuation
			+ Lights[light].linearAttenuation	* lightDistance
			+ Lights[light].quadraticAttenuation * lightDistance * lightDistance);

			if (Lights[light].isSpot) {
				float spotCos = dot(lightDirection, -Lights[light].coneDirection); 
				if (spotCos < Lights[light].spotCosCutoff)
					attenuation = 0.0;
				else
					attenuation *= pow(spotCos,Lights[light].spotExponent);
			}
		}		

		if (!(IsLocal || Lights[light].isPositional)) {
			halfVector = Lights[light].halfVector;
		} else {
			halfVector = normalize(lightDirection + EyeVector);
		}

        float diffuse = max(0.0, dot(twoNormal, lightDirection));
		float specular =  max(0.0, dot(twoNormal, halfVector)); 

		if (diffuse == 0.0) 
			specular = 0.0;
		else
			specular = pow(specular, Material[MatIndex].shininess); 

		// Accumulate all the lights’ effects
		scatteredLight += Lights[light].ambient * Material[MatIndex].ambient * attenuation +
					  	  Lights[light].diffColor * Material[MatIndex].diffuse * diffuse * attenuation;
		reflectedLight += Lights[light].specColor * Material[MatIndex].specular * specular * attenuation;
	}

    rgb = min(Material[MatIndex].emission + Color.rgb * scatteredLight + reflectedLight, vec3(1.0));

	return vec4(rgb, Color.a);

}

void main(void)
{

	FragColor = LightAtPoint (Position, normalize(Normal));

}
