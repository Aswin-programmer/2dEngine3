#version 460 core

layout(location = 0) in vec3 aPos;       // Particle position
layout(location = 1) in vec3 aVelocity;  // (optional, unused here)
layout(location = 2) in float aLife;
layout(location = 3) in float aMaxLife;

out float vLife;
out float vMaxLife;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vLife = aLife;
    vMaxLife = aMaxLife;

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Set size based on life — max 10.0
    float lifeRatio = clamp(aLife / aMaxLife, 0.0, 1.0);
    gl_PointSize = mix(1.0, 2.0, lifeRatio); // Shrinks as it fades
}
