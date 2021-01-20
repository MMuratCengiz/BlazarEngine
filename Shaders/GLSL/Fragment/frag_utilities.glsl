const float gammaCorrection = 2.2;

vec4 gammaCorrectColor( in vec4 color )
{
    return vec4(pow(color.rgb, vec3(1 / gammaCorrection)), color.a);
}