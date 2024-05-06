#include "CsmOpaque.hlsli"

[maxvertexcount(3)]
void main(triangle VS_OUT_CSM input[3], inout TriangleStream<GS_OUTPUT_CSM> output)
{
    GS_OUTPUT_CSM element;
    element.slice = input[0].slice;

    element.position = input[0].position;
    output.Append(element);
    element.position = input[1].position;
    output.Append(element);
    element.position = input[2].position;
    output.Append(element);

    output.RestartStrip();
}
