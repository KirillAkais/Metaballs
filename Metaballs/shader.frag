#version 450
out vec4 fragColor;
uniform vec2 iResolution;
uniform int iTime;
uniform vec2 iPartPos[100];
uniform float iPartRad[100];
void main (void)
{
	vec2 coord = gl_FragCoord.xy / iResolution - vec2(0.5);
	coord.x /= iResolution.y / iResolution.x;
	vec4 color = vec4(0);
	for(int i = 0; i < iPartPos.length(); i++)
	{
		color += vec4(iPartRad[i] / distance(iPartPos[i], coord));
	}
	color /= 250;
	if(color.r <= 0.91 && color.r >= 0.89) color = vec4(1, 1, 1, 0);
	else color = vec4(0, 0, 0, 1);
	fragColor = color;
}