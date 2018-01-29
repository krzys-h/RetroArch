struct Foo
{
    float3 a;
    float3 b;
    float3 c;
};

static float4 gl_Position;
static float4 Input2;
static float4 Input4;
static float4 Input0;
static float vLocation0;
static float vLocation1;
static float vLocation2[2];
static Foo vLocation4;
static float vLocation9;

struct VertexOut
{
    float3 color : TEXCOORD7;
    float3 foo : TEXCOORD8;
};

static VertexOut vout;

struct SPIRV_Cross_Input
{
    float4 Input0 : TEXCOORD0;
    float4 Input2 : TEXCOORD2;
    float4 Input4 : TEXCOORD4;
};

struct SPIRV_Cross_Output
{
    float vLocation0 : TEXCOORD0;
    float vLocation1 : TEXCOORD1;
    float vLocation2[2] : TEXCOORD2;
    Foo vLocation4 : TEXCOORD4;
    float vLocation9 : TEXCOORD9;
    float4 gl_Position : SV_Position;
};

Foo _70;

void vert_main()
{
    gl_Position = ((1.0f.xxxx + Input2) + Input4) + Input0;
    vLocation0 = 0.0f;
    vLocation1 = 1.0f;
    vLocation2[0] = 2.0f;
    vLocation2[1] = 2.0f;
    Foo _65 = _70;
    _65.a = 1.0f.xxx;
    Foo _67 = _65;
    _67.b = 1.0f.xxx;
    Foo _69 = _67;
    _69.c = 1.0f.xxx;
    vLocation4 = _69;
    vLocation9 = 9.0f;
    vout.color = 2.0f.xxx;
    vout.foo = 4.0f.xxx;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input, out VertexOut stage_outputvout)
{
    Input2 = stage_input.Input2;
    Input4 = stage_input.Input4;
    Input0 = stage_input.Input0;
    vert_main();
    stage_outputvout = vout;
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.vLocation0 = vLocation0;
    stage_output.vLocation1 = vLocation1;
    stage_output.vLocation2 = vLocation2;
    stage_output.vLocation4 = vLocation4;
    stage_output.vLocation9 = vLocation9;
    return stage_output;
}
