// Edge-Avoiding À-TrousWavelet Transform for denoising

vec4 denoise(ivec2 coords) 
{
    const float kernel[25] = float[25](
	    1.0/256.0, 1.0/64.0, 3.0/128.0, 1.0/64.0, 1.0/256.0,
	    1.0/64.0,  1.0/16.0, 3.0/32.0,  1.0/16.0, 1.0/64.0,
	    3.0/128.0, 3.0/32.0, 9.0/64.0,  3.0/32.0, 3.0/128.0,
	    1.0/64.0,  1.0/16.0, 3.0/32.0,  1.0/16.0, 1.0/64.0,
	    1.0/256.0, 1.0/64.0, 3.0/128.0, 1.0/64.0, 1.0/256.0 );

    const ivec2 offset[25] = ivec2[25]( 
	    ivec2(-2,-2), ivec2(-1,-2), ivec2(0,-2), ivec2(1,-2), ivec2(2,-2), 
	    ivec2(-2,-1), ivec2(-1,-1), ivec2(0,-2), ivec2(1,-1), ivec2(2,-1), 
	    ivec2(-2, 0), ivec2(-1, 0), ivec2(0, 0), ivec2(1, 0), ivec2(2, 0), 
	    ivec2(-2, 1), ivec2(-1, 1), ivec2(0, 1), ivec2(1, 1), ivec2(2, 1),
	    ivec2(-2, 2), ivec2(-1, 2), ivec2(0, 2), ivec2(1, 2), ivec2(2, 2) );

    vec2 fragCoord = coords;
    vec3 sum = vec3(0.0);
    float colorPhi = .9f; //ubo.colorPhi;
    float normalPhi = .3f; // ubo.normalPhi;
    float positionPhi = .6f; //ubo.positionPhi;

    vec3 cval = imageLoad(InputImage, ivec2(fragCoord)).rgb;     // color
    vec3 nval = imageLoad(NormalsImage, ivec2(fragCoord)).rgb;   // normal
    vec3 pval = imageLoad(PositionsImage, ivec2(fragCoord)).rgb; // position
    
    float cum_w = 0.0;

    for(uint i = 0; i < 25; ++i)
    {
        ivec2 uv = ivec2(fragCoord + offset[i] * 2);

        // Color
        vec3 ctmp = imageLoad(InputImage, uv).rgb;
		vec3 t = cval - ctmp;							// Ip - Iq		(color difference)
		float dist2 = dot(t, t);						// ||Ip - Iq||	(distance squared)
		float c_w = min(exp(-(dist2) / colorPhi), 1.0); // w(p,q)		(weight function)

		vec3 ntmp = imageLoad(NormalsImage, uv).rgb;
		t = nval - ntmp;
		dist2 = dot(t, t);
		float n_w = min(exp(-(dist2) / normalPhi), 1.0);

		vec3 ptmp = imageLoad(PositionsImage, uv).rgb;
		t = pval - ptmp;
		dist2 = dot(t, t);
		float p_w = min(exp(-(dist2) / positionPhi), 1.0);

		float weight = c_w * n_w * p_w;
		sum += ctmp * weight * kernel[i];
		cum_w += weight * kernel[i];
	}

    return vec4(sum / cum_w, 0.f);
}