
// This is an output resource (RW = read/write)
RWTexture3D<float4> gOutput : register(u0);

// The amount of threads in the thread group
[numthreads(8, 8, 8)]
void main(int3 dispatchThreadID : SV_DispatchThreadID)
{
    gOutput[dispatchThreadID.xyz] = float4(0.0f, 1.0f, 0.0f, 1.0f);
}