#version 330 core
//in vec3 color;
//out vec4 frag_color;
uniform sampler2D fieldTexture;
in vec2 fragTexcoord;
out vec4 color;
void main() {
    color = texture(fieldTexture, fragTexcoord);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}