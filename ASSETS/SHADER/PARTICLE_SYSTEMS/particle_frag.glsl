#version 460 core

in float vLife;
in float vMaxLife;

out vec4 FragColor;

void main()
{
    float lifeRatio = clamp(vLife / vMaxLife, 0.0, 1.0);

    // Create a circular point using gl_PointCoord
    float dist = length(gl_PointCoord - vec2(0.5));
    if (dist > 0.5)
        discard;

    // Fade from orange to yellow as life decreases
    vec3 color = mix(vec3(1.0, 0.3, 0.0), vec3(1.0, 1.0, 0.0), lifeRatio);

    // Fade out over time using lifeRatio
    float alpha = lifeRatio;

    FragColor = vec4(color, alpha);
}
