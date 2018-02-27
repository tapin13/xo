#include "loadShader.h"
#include "loadFile.h"

GLuint loadShader() {
    GLchar *vertex_shader_text = NULL;
    GLchar *fragment_shader_text = NULL;

    vertex_shader_text = (GLchar *)readFile("shaders/vs.vert", 0);
    
    if(vertex_shader_text == NULL) {
        printf("vertex_shader_text - bad\n");
    }
    
    GLuint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar **)&vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    GLint result = GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        printf("vertex_shader - bad\n");
    }

    free(vertex_shader_text);
    
    fragment_shader_text = (GLchar *)readFile("shaders/fs.frag", 0);
    
    if(fragment_shader_text == NULL) {
        printf("fragment_shader_text - bad\n");
    }    
    
    GLuint fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const GLchar **)&fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        printf("fragment_shader - bad\n");
    }
    
    free(fragment_shader_text);
    
    GLuint program;
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if(result == GL_FALSE) {
        printf("glLinkProgram - bad\n");
    }
    
    return program;
}