uniform sampler2D texture0; // Diffuse
uniform sampler2D texture1; // Shadowmap

uniform vec2 agk_resolution;
uniform vec2 agk_spritesize;
uniform vec2 playerOffset;
uniform float blurSize;

varying vec2 uvVarying;
varying vec4 colorVarying;


void main()
{
	vec4 diffuse = texture2D(texture0, uvVarying) * colorVarying;

	if(diffuse.a < 0.5)
		discard;
	
	// Precomputed Gaussian weights
	float weights[16];
	weights[0]  = 0.0895;
	weights[1]  = 0.0840;
	weights[2]  = 0.0730;
	weights[3]  = 0.0582;
	weights[4]  = 0.0410;
	weights[5]  = 0.0247;
	weights[6]  = 0.0127;
	weights[7]  = 0.0055;
	weights[8]  = 0.0019;
	weights[9]  = 0.0006;
	weights[10] = 0.0002;
	weights[11] = 0.00005;
	weights[12] = 0.00001;
	weights[13] = 0.000003;
	weights[14] = 0.000001;
	weights[15] = 0.0000005;

    vec2 pixelSize = 1.0 / agk_resolution;
	vec2 uvPlayer = uvVarying * agk_spritesize * pixelSize + playerOffset * pixelSize;

    vec4 shadow = texture2D(texture1, uvPlayer) * weights[0];
    // Sum both horizontal and vertical blur directions
    for (int i = 1; i < 16; ++i)
    {
        float weight = weights[i];
        vec2 offset = float(i) * pixelSize * blurSize;

		//axis aligned blur
        //shadow += texture2D(texture1, uvPlayer + vec2(offset.x, 0.0)) * weight;
        //shadow += texture2D(texture1, uvPlayer - vec2(offset.x, 0.0)) * weight;
        //shadow += texture2D(texture1, uvPlayer + vec2(0.0, offset.y)) * weight;
        //shadow += texture2D(texture1, uvPlayer - vec2(0.0, offset.y)) * weight;
		
		//diagonal blur
		shadow += texture2D(texture1, uvPlayer + vec2( offset.x,  offset.y)) * weight;
		shadow += texture2D(texture1, uvPlayer + vec2(-offset.x,  offset.y)) * weight;
		shadow += texture2D(texture1, uvPlayer + vec2( offset.x, -offset.y)) * weight;
		shadow += texture2D(texture1, uvPlayer + vec2(-offset.x, -offset.y)) * weight;
    }

    // Normalize with total weight times 2 if both axis aligned and diagonal, this needs to be recalculated with new weights
    shadow /= 1.298958;// * 2.0;

    gl_FragColor = shadow * diffuse;
}
