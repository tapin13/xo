#version 330 core
//in vec3 color;
//out vec4 frag_color;
uniform sampler2D colorTexture;
in vec2 fragTexcoord;
out vec4 color;
void main() {
    color = texture(colorTexture, fragTexcoord);
    //color = vec4(1.0, 0.0, 0.0, 1.0);
}