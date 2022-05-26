vec2[ 2 ] getOverSizedTriangle( in int vertexIndex )
{
    vec2 result[ 2 ];
    result[ 0 ] = vec2(0); // Position
    result[ 1 ] = vec2(0); // Texture Coordinates

    result[ 1 ] .x = (vertexIndex == 2) ?  2.0 :  0.0;
    result[ 1 ] .y = (vertexIndex == 1) ?  2.0 :  0.0;

    result[ 0 ] = result[ 1 ] * vec2(2.0, -2.0);
    result[ 0 ] += vec2(-1.0, 1.0);
    result[ 1 ].y = result[ 1 ].y * -1.0f + 1.0f;

    return result;
}