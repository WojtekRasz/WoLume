struct VSInput {
    float3 inPosition;
    float3 inColor;
    float2 inUv;
};

struct VSOutput {
    float4 pos : SV_Position;
    float3 color;
    float2 uv;
};

[[vk::binding(0, 0)]]
struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};
ConstantBuffer<UniformBuffer> ubo;

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPosition, 1.0))));
    output.color = input.inColor;
    output.uv = input.inUv;
    return output;
}

[[vk::binding(1, 0)]]
Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    return texture.Sample(vertIn.uv);
}