const float gammaCorrection = 2.2;

vec4 gammaCorrectColor( in vec4 color )
{
//    return color;
    vec4 colorMapped = color / ( color + vec4( 1 ) );

    colorMapped = vec4(pow(color.rgb, vec3(1 / gammaCorrection)), color.a);
    return colorMapped;
}