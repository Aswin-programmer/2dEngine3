#version 460 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT
{
    vec3 position;
    float life;
    float maxLife;
}gs_in[];

out GS_OUT
{
    float life;
    float maxLife;
    vec2 texCoord;
}gs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 center = gs_in[0].position;
    float size = 0.025; //Adjust as needed

    vec3 right = vec3(1.f, 0.f, 0.f) * size;
    vec3 up = vec3(0.f, 1.f, 0.f) * size;

    vec3 offsets[4] = 
    {
        -right-up, // bottom-left
        +right-up, // bottom-right
        -right+up, // top-left
        +right+up  // top-right
    };

    vec2 texCoords[4] =
    {
        vec2(0.f, 0.f),
        vec2(1.f, 0.f),
        vec2(0.f, 1.f),
        vec2(1.f, 1.f)
    };

    for(int i=0;i<4;i++)
    {
        gl_Position = projection * view * model * vec4(center + offsets[i], 1.0);
        gs_out.life = gs_in[0].life;
        gs_out.maxLife = gs_in[0].maxLife;
        gs_out.texCoord = texCoords[i];
        EmitVertex();
    }
    EndPrimitive();
}

