#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D image;
uniform vec4 parameters;

void main()
{
	vec2 tex_size = textureSize(image, 0);
	vec2 texelSize = global.SourceSize.zw;
	vec2 range = vec2(
		abs(global.SourceSize.x / (global.OutputSize.x * global.SourceSize.x)),
		abs(global.SourceSize.y / (global.OutputSize.y * global.SourceSize.y)));
	range = range / 2.0 * 0.999;

	float left   = vTexCoord.x - range.x;
	float top    = vTexCoord.y + range.y;
	float right  = vTexCoord.x + range.x;
	float bottom = vTexCoord.y - range.y;

	Vec3 topLeftColor = texture(Source,
		(floor(vec2(left, top) / texelSize) + 0.5) * texelSize).rgb;
	Vec3 bottomRightColor = texture(Source,
		(floor(vec2(right, bottom) / texelSize) + 0.5) * texelSize).rgb;
	Vec3 bottomLeftColor = texture(Source,
		(floor(vec2(left, bottom) / texelSize) + 0.5) * texelSize).rgb;
	Vec3 topRightColor = texture(Source,
		(floor(vec2(right, top) / texelSize) + 0.5) * texelSize).rgb;

	vec2 border = clamp(
		round(vTexCoord / texelSize) * texelSize,
		vec2(left, bottom),
		vec2(right, top));
	float totalArea = 4.0 * range.x * range.y;

	vec3 averageColor;
	averageColor  = ((border.x - left)  * (top - border.y)
		/ totalArea) * topLeftColor;
	averageColor += ((right - border.x) * (border.y - bottom)
		/ totalArea) * bottomRightColor;
	averageColor += ((border.x - left)  * (border.y - bottom)
		/ totalArea) * bottomLeftColor;
	averageColor += ((right - border.x) * (top - border.y)
		/ totalArea) * topRightColor;

	color = vec4(averageColor, 1.0);
}

