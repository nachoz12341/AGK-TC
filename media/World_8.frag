uniform sampler2D texture0; // Diffuse
uniform sampler2D texture1; // Shadowmap

uniform vec2 agk_resolution;
uniform float blurSize;

varying vec2 uvVarying;
varying vec4 colorVarying;

void main()
{
	vec4 diffuse = texture2D(texture0, uvVarying) * colorVarying;

	if (diffuse.a < 0.5)
		discard;

	float weights[8];
	weights[0] = 0.2270;
	weights[1] = 0.1945;
	weights[2] = 0.1216;
	weights[3] = 0.0540;
	weights[4] = 0.0162;
	weights[5] = 0.0037;
	weights[6] = 0.0006;
	weights[7] = 0.00005;

	vec2 pixelSize = 1.0 / agk_resolution;

	vec4 shadow = texture2D(texture1, uvVarying) * weights[0];

	for (int i = 1; i < 8; ++i)
	{
		float weight = weights[i];
		vec2 offset = float(i) * pixelSize * blurSize;

		// Diagonal blur
		shadow += texture2D(texture1, uvVarying + vec2( offset.x,  offset.y)) * weight;
		shadow += texture2D(texture1, uvVarying + vec2(-offset.x,  offset.y)) * weight;
		shadow += texture2D(texture1, uvVarying + vec2( offset.x, -offset.y)) * weight;
		shadow += texture2D(texture1, uvVarying + vec2(-offset.x, -offset.y)) * weight;
	}
	
	shadow /= 1.7896;

	gl_FragColor = shadow * diffuse;
}
