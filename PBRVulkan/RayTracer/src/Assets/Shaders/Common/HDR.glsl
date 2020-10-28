// HDR specific functions

float envPdf()
{
	float theta = acos(clamp(payload.direction.y, -1.0, 1.0));
	vec2 uv = vec2((PI + atan(payload.direction.z, payload.direction.x)) * (1.0 / TWO_PI), theta * (1.0 / PI));
	float pdf = texture(HDRs[1], uv).y * texture(HDRs[2], vec2(uv.y, 0.)).y;
	return (pdf * ubo.hdrResolution) / (2.0 * PI * PI * sin(theta));
}

vec4 envSample(inout vec3 color)
{
	float r1 = rnd(payload.seed);
	float r2 = rnd(payload.seed);

	float v = texture(HDRs[2], vec2(r1, 0.)).x;
	float u = texture(HDRs[1], vec2(r2, v)).x;

	color = texture(HDRs[0], vec2(u, v)).xyz * 2.f;
	float pdf = texture(HDRs[1], vec2(u, v)).y * texture(HDRs[2], vec2(v, 0.)).y;

	float phi = u * TWO_PI;
	float theta = v * PI;

	if (sin(theta) == 0.0)
		pdf = 0.0;

	return vec4(-sin(theta) * cos(phi), cos(theta), -sin(theta)*sin(phi), (pdf * ubo.hdrResolution) / (2.0 * PI * PI * sin(theta)));
}