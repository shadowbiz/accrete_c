#version 410 core

layout (location = 0) out vec4 fragColor;

//in vec3 gPosition;
//in vec3 gNormal;

in vec3 vPosition;
in vec3 vLightDirection;
in vec3 vEyeVector;
in vec3 vNormal;

//in LightInfo gLight;

//uniform LightInfo Light;

//in vec3 gLightDirection;
//in vec3 gEyeVector;

void main()
{

    const vec4 AmbientColor = vec4(0.1, 0.0, 0.0, 1.0);
    const vec4 DiffuseColor = vec4(1.0, 0.0, 0.0, 1.0);
 
    // Scaling The Input Vector To Length 1
    vec3 n = normalize(vNormal);
    vec3 l = normalize(vLightDirection);

    float lambert = dot(n,l);

    finalColor = AmbientColor + DiffuseColor;

    if (lambert > 0.0)
    {
        fragColor += DiffuseColor * lambert;
    }

    //float DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);
 
    // Calculating The Final Color
    

	//fragColor = vec4(vPosition, 1.0);
	//fragColor = vec4(toonShade(),1);
	//fragColor = vec4(phongModel(gNormal.xyz, vec3(1)), 1.0 );
}