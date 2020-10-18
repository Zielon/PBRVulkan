#extension GL_EXT_control_flow_attributes : require

// https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl

uint random(inout uint seed)
{
	// LCG values from Numerical Recipes
    return (seed = 1664525 * seed + 1013904223);
}

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
// Pseudo-random value in half-open range [0:1].
float rand(inout uint seed) 
{ 
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32
    uint m = random(seed) >> 9;

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat(m); // Range [1:2]
    return f - 1.0;                // Range [0:1]
}