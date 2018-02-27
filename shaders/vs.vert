#version 330 core
in vec2 vertex_position;
in vec2 texcoord;
//out vec3 color;
out vec2 fragTexcoord;
void main() {
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    fragTexcoord = vec2(texcoord.x, texcoord.y);
}