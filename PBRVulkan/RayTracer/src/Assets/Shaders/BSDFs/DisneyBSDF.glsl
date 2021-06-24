/*
 * MIT License
 *
 * Copyright(c) 2019-2021 Asif Ali
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


vec3 EvalDielectricReflection(in Material material, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    if (dot(N, L) <= 0.0)
        return vec3(0.0);

    float eta = payload.eta;

    float F = DielectricFresnel(dot(V, H), eta);
    float D = GTR2(dot(N, H), material.roughness);

    pdf = D * dot(N, H) * F / (4.0 * abs(dot(V, H)));

    float G = SmithGGX(abs(dot(N, L)), material.roughness) * SmithGGX(abs(dot(N, V)), material.roughness);
    
    return material.albedo.xyz * F * D * G;
}


vec3 EvalDielectricRefraction(in Material material, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    float eta = payload.eta;

    pdf = 0.0;
    if (dot(N, L) >= 0.0)
        return vec3(0.0);

    float F = DielectricFresnel(abs(dot(V, H)), eta);
    float D = GTR2(dot(N, H), material.roughness);

    float denomSqrt = dot(L, H) + dot(V, H) * eta;
    pdf = D * dot(N, H) * (1.0 - F) * abs(dot(L, H)) / (denomSqrt * denomSqrt);

    float G = SmithGGX(abs(dot(N, L)), material.roughness) * SmithGGX(abs(dot(N, V)), material.roughness);

    return material.albedo.xyz * (1.0 - F) * D * G * abs(dot(V, H)) * abs(dot(L, H)) * 4.0 * eta * eta / (denomSqrt * denomSqrt);
}

vec3 EvalSpecular(in Material material, in vec3 Cspec0, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    if (dot(N, L) <= 0.0)
        return vec3(0.0);

    float D = GTR2(dot(N, H), material.roughness);
    pdf = D * dot(N, H) / (4.0 * dot(V, H));

    float FH = SchlickFresnel(dot(L, H));
    vec3 F = mix(Cspec0, vec3(1.0), FH);
    float G = SmithGGX(abs(dot(N, L)), material.roughness) * SmithGGX(abs(dot(N, V)), material.roughness);

    return F * D * G;
}

vec3 EvalClearcoat(in Material material, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    if (dot(N, L) <= 0.0)
        return vec3(0.0);

    float D = GTR1(dot(N, H), mix(0.1, 0.001, material.clearcoatGloss));
    pdf = D * dot(N, H) / (4.0 * dot(V, H));

    float FH = SchlickFresnel(dot(L, H));
    float F = mix(0.04, 1.0, FH);
    float G = SmithGGX(dot(N, L), 0.25) * SmithGGX(dot(N, V), 0.25);

    return vec3(0.25 * material.clearcoat * F * D * G);
}


vec3 EvalDiffuse(in Material material, in vec3 Csheen, vec3 V, vec3 N, vec3 L, vec3 H, inout float pdf)
{
    pdf = 0.0;
    if (dot(N, L) <= 0.0)
        return vec3(0.0);

    pdf = dot(N, L) * (1.0 / PI);

    // Diffuse
    float FL = SchlickFresnel(dot(N, L));
    float FV = SchlickFresnel(dot(N, V));
    float FH = SchlickFresnel(dot(L, H));
    float Fd90 = 0.5 + 2.0 * dot(L, H) * dot(L, H) * material.roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);

    // Fake Subsurface TODO: Replace with volumetric scattering
    float Fss90 = dot(L, H) * dot(L, H) * material.roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1.0 / (dot(N, L) + dot(N, V)) - 0.5) + 0.5);

    vec3 Fsheen = FH * material.sheen * Csheen;

    return ((1.0 / PI) * mix(Fd, ss, material.subsurface) * material.albedo.xyz + Fsheen) * (1.0 - material.metallic);
}


vec3 DisneySample(in Material material, inout vec3 L, inout float pdf)
{
    pdf = 0.0;
    vec3 f = vec3(0.0);

    float diffuseRatio = 0.5 * (1.0 - material.metallic);
    float transWeight = (1.0 - material.metallic) * material.transmission;

    vec3 Cdlin = material.albedo.xyz;
    float Cdlum = 0.3 * Cdlin.x + 0.6 * Cdlin.y + 0.1 * Cdlin.z; // luminance approx.

    vec3 Ctint = Cdlum > 0.0 ? Cdlin / Cdlum : vec3(1.0f); // normalize lum. to isolate hue+sat
    vec3 Cspec0 = mix(material.albedo.w * 0.08 * mix(vec3(1.0), Ctint, material.specularTint), Cdlin, material.metallic);
    vec3 Csheen = mix(vec3(1.0), Ctint, material.sheenTint);
    float eta = payload.eta;

    vec3 N = payload.ffnormal;
    vec3 V = -gl_WorldRayDirectionEXT;

    mat3 frame = localFrame(N);

    float r1 = rnd(seed);
    float r2 = rnd(seed);

    if (rnd(seed) < transWeight)
    {
        vec3 H = ImportanceSampleGTR2(material.roughness, r1, r2);
        H = frame * H;

        if (dot(V, H) < 0.0)
            H = -H;

        vec3 R = reflect(-V, H);
        float F = DielectricFresnel(abs(dot(R, H)), eta);

        // Reflection/Total internal reflection
        if (r2 < F)
        {
            L = normalize(R);
            f = EvalDielectricReflection(material, V, N, L, H, pdf);
        }
        else // Transmission
        {
            L = normalize(refract(-V, H, eta));
            f = EvalDielectricRefraction(material, V, N, L, H, pdf);
        }

        f *= transWeight;
        pdf *= transWeight;
    }
    else
    {
        if (rnd(seed) < diffuseRatio)
        {
            L = cosineSampleHemisphere();
            L = frame * L;

            vec3 H = normalize(L + V);

            f = EvalDiffuse(material, Csheen, V, N, L, H, pdf);
            pdf *= diffuseRatio;
        }
        else // Specular
        {
            float primarySpecRatio = 1.0 / (1.0 + material.clearcoat);

            // Sample primary specular lobe
            if (rnd(seed) < primarySpecRatio)
            {
                // TODO: Implement http://jcgt.org/published/0007/04/01/
                vec3 H = ImportanceSampleGTR2(material.roughness, r1, r2);
                H = frame * H;

                if (dot(V, H) < 0.0)
                    H = -H;

                L = normalize(reflect(-V, H));

                f = EvalSpecular(material, Cspec0, V, N, L, H, pdf);
                pdf *= primarySpecRatio * (1.0 - diffuseRatio);
            }
            else // Sample clearcoat lobe
            {
                vec3 H = ImportanceSampleGTR1(mix(0.1, 0.001, material.clearcoatGloss), r1, r2);
                H = frame * H;

                if (dot(V, H) < 0.0)
                    H = -H;

                L = normalize(reflect(-V, H));

                f = EvalClearcoat(material, V, N, L, H, pdf);
                pdf *= (1.0 - primarySpecRatio) * (1.0 - diffuseRatio);
            }
        }

        f *= (1.0 - transWeight);
        pdf *= (1.0 - transWeight);
    }

    return f;
}
    
vec3 DisneyEval(Material material, vec3 L, inout float pdf)
{
    vec3 N = payload.ffnormal;
    vec3 V = -gl_WorldRayDirectionEXT;
    float eta = payload.eta;

    vec3 H;
    bool refl = dot(N, L) > 0.0;

    if (refl)
        H = normalize(L + V);
    else
        H = normalize(L + V * eta);

    if (dot(V, H) < 0.0)
        H = -H;

    float diffuseRatio = 0.5 * (1.0 - material.metallic);
    float primarySpecRatio = 1.0 / (1.0 + material.clearcoat);
    float transWeight = (1.0 - material.metallic) * material.transmission;

    vec3 brdf = vec3(0.0);
    vec3 bsdf = vec3(0.0);
    float brdfPdf = 0.0;
    float bsdfPdf = 0.0;

    if (transWeight > 0.0)
    {
        // Reflection
        if (refl)
        {
            bsdf = EvalDielectricReflection(material, V, N, L, H, bsdfPdf);
        }
        else // Transmission
        {
            bsdf = EvalDielectricRefraction(material, V, N, L, H, bsdfPdf);
        }
    }

    float m_pdf;

    if (transWeight < 1.0)
    {
        vec3 Cdlin = material.albedo.xyz;
        float Cdlum = 0.3 * Cdlin.x + 0.6 * Cdlin.y + 0.1 * Cdlin.z; // luminance approx.

        vec3 Ctint = Cdlum > 0.0 ? Cdlin / Cdlum : vec3(1.0f); // normalize lum. to isolate hue+sat
        vec3 Cspec0 = mix(material.albedo.w * 0.08 * mix(vec3(1.0), Ctint, material.specularTint), Cdlin, material.metallic);
        vec3 Csheen = mix(vec3(1.0), Ctint, material.sheenTint);

        // Diffuse
        brdf += EvalDiffuse(material, Csheen, V, N, L, H, m_pdf);
        brdfPdf += m_pdf * diffuseRatio;

        // Specular
        brdf += EvalSpecular(material, Cspec0, V, N, L, H, m_pdf);
        brdfPdf += m_pdf * primarySpecRatio * (1.0 - diffuseRatio);

        // Clearcoat
        brdf += EvalClearcoat(material, V, N, L, H, m_pdf);
        brdfPdf += m_pdf * (1.0 - primarySpecRatio) * (1.0 - diffuseRatio);
    }

    pdf = mix(brdfPdf, bsdfPdf, transWeight);

    return mix(brdf, bsdf, transWeight);
}
