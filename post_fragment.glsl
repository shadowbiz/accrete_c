#version 400

in vec2 Texcoord;
in vec4 pos;

out vec4 outColor;

uniform sampler2D u_diffuseMap;

const float step_w  = 1.0 / 400.0;
const float step_h  = 1.0 / 300.0;

vec4 Bloom()
{
/*	
	Step = vec2(0.1,0.1);
	color  =  texture2DRect(Tex, Texcoord.xy) * 0.282095;
    vec2 offs = Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.265004;
    offs += Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.219696;
    offs += Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.160733;
    offs += Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.103777;
    offs += Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.0591303;
    offs += Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.0297326;
    offs += Step;
    color += (texture2DRect(Tex, Texcoord.xy + offs) + texture2DRect(Tex, Texcoord.xy - offs))* 0.0131937;
*/
    vec4 color  =  texture2D(u_diffuseMap, Texcoord.xy) * 0.56419;
    vec2 Step = vec2(0.02);
	vec2 offs = Step;
    color += (texture2D(u_diffuseMap, Texcoord.xy + offs) + texture2D(u_diffuseMap, Texcoord.xy - offs))* 0.439391;
    offs += Step;
    color += (texture2D(u_diffuseMap, Texcoord.xy + offs) + texture2D(u_diffuseMap, Texcoord.xy - offs))* 0.207554;
    offs += Step;
    color += (texture2D(u_diffuseMap, Texcoord.xy + offs) + texture2D(u_diffuseMap, Texcoord.xy - offs))* 0.0594651;

	return color;
}


float Gaussian (float x, float deviation)
{
    return (1.0 / sqrt(2.0 * 3.141592 * deviation)) * exp(-((x * x) / (2.0 * deviation)));  
}

vec4 blur()
{
	vec4 sum = vec4( 0.0 );
	for (int x = -4; x <= 4; x++)
    {
	    for (int y = -4; y <= 4; y++)
        {
		    sum += texture(u_diffuseMap, vec2( Texcoord.x + x * step_w, Texcoord.y + y * step_h)) / 81.0;
						
		}
	}
	return sum;
}

const float sampleDist = 1.0;
const float sampleStrength = 2.2; 
float samples[10];

vec4 radialBlur()
{
	samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;

	vec2 dir = 0.5 - Texcoord; 
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; 

	vec4 color = texture2D(u_diffuseMap,Texcoord); 
    vec4 sum = color;

	for (int i = 0; i < 10; i++)
    {
	    sum += texture2D( u_diffuseMap, Texcoord + dir * samples[i] * sampleDist );
	}

	sum *= 1.0/11.0;
    float t = dist * sampleStrength;
    t = clamp(t ,0.0,1.0);

	return mix(color, sum, t);
}

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }
float snoise (vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626, // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0

    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);

    // Other corners
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
        + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    // Compute final noise value at P
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

vec3 Overlay (vec3 src, vec3 dst)
{
    // if (dst <= Ω) then: 2 * src * dst
    // if (dst > Ω) then: 1 - 2 * (1 - dst) * (1 - src)
    return vec3((dst.x <= 0.5) ? (2.0 * src.x * dst.x) : (1.0 - 2.0 * (1.0 - dst.x) * (1.0 - src.x)),
                (dst.y <= 0.5) ? (2.0 * src.y * dst.y) : (1.0 - 2.0 * (1.0 - dst.y) * (1.0 - src.y)),
                (dst.z <= 0.5) ? (2.0 * src.z * dst.z) : (1.0 - 2.0 * (1.0 - dst.z) * (1.0 - src.z)));
}

float bgl_TextureWidth = 800;
float bgl_TextureHeight = 600;

vec4 aa()
{
	//tile *should* contain 1.0/bgl_TextureWidth and 1.0/bgl_TextureHeight but that doesn't work :(
    vec2 Tile = vec2(1/bgl_TextureWidth,1/bgl_TextureHeight);
	//vec2 Tile = vec2(0.0005,0.001);
    
	vec2 Blur = vec2(0.3,0.3);
    // Current texture position (normalised)
    vec2 texCoord = Texcoord;
    // Normalised position of current pixel in its tile
    vec2 tilePos = vec2(fract(texCoord.x / Tile.x), fract(texCoord.y / Tile.y));
    
    // Bottom-left of current tile
    vec2 p0 = vec2(floor(texCoord.x / Tile.x) * Tile.x, floor(texCoord.y / Tile.y) * Tile.y);
    // Bottom-left of tile to Left of current tile
    vec2 p1 = vec2(clamp(p0.x - Tile.x, 0.0, 1.0), p0.y);
    // Bottom-left of tile Below current tile
    vec2 p2 = vec2(p0.x, clamp(p0.y - Tile.y, 0.0, 1.0));
    // Bottom-left of tile Below and Left of current tile
    vec2 p3 = vec2(p1.x, p2.y);

    vec2 mixFactors;
    mixFactors.x =  min(tilePos.x / Blur.x, 1.0);
    mixFactors.y =  min(tilePos.y / Blur.y, 1.0);

    vec4 tmp1 = mix(texture2D(u_diffuseMap, p1+(Tile/2.0)), texture2D(u_diffuseMap, p0+(Tile/2.0)), mixFactors.x);
    vec4 tmp2 = mix(texture2D(u_diffuseMap, p3 +(Tile/2.0)), texture2D(u_diffuseMap, p2+(Tile/2.0)), mixFactors.x);
    
	return mix(tmp2, tmp1, mixFactors.y);

}

void main()
{
	outColor = texture2D(u_diffuseMap, Texcoord );
	//outColor = aa();
}