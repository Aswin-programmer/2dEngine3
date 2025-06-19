#version 450 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D tex;
void main() {
    FragColor = texture(tex, vUV);
    //FragColor = vec4(1.0, 1.0, 0.0, 1.f);
}