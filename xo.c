#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "loadShader.h"
#include "tgaLoad.h"

#define gamerX 1
#define gamerO 2

char gameMatrix[3][3] = { { 0 }, { 0 }, { 0 } };
char gamer = gamerX;

int setXO(GLFWwindow* window, double xpos, double ypos) {
    printf("setXO %f %f\n", xpos, ypos);

    unsigned char gameMatrixXpos = 0;
    unsigned char gameMatrixYpos = 0;

    int windowWeight, windowHeight;

    glfwGetWindowSize(window, &windowWeight, &windowHeight);
    
    if(xpos < windowWeight / 3) {
        gameMatrixXpos = 0;
    } else if(xpos > windowWeight * 2 / 3) {
        gameMatrixXpos = 2;
    } else { // center
        gameMatrixXpos = 1;
    }
    
    if(ypos < windowHeight / 3) {
        gameMatrixYpos = 0;
    } else if(ypos > windowHeight * 2 / 3) {
        gameMatrixYpos = 2;
    } else { // center
        gameMatrixYpos = 1;
    }
    
    printf("gameMatrixPos: %d %d\n", gameMatrixXpos, gameMatrixYpos);
    
    if(gameMatrix[gameMatrixYpos][gameMatrixXpos] != 0) {
        return 0;
    }
    
    gameMatrix[gameMatrixYpos][gameMatrixXpos] = gamer;
    
    printf("gameMatrixPos: %d %d gamer: %d\n", gameMatrixXpos, gameMatrixYpos, gamer);
    
    glBindTexture(GL_TEXTURE_2D, 2);

    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6 // total points.
    ); 
    
    if(gamer == gamerX) {
        gamer = gamerO;
    } else {
        gamer = gamerX;
    }
    
    return 1;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        printf("left pressed\n" );
        
        glfwGetCursorPos(window, &xpos, &ypos);

        setXO(window, xpos, ypos);
    }
}

int main(int argv, char *argc[]) {

    if(!glfwInit()){
        printf("Failed to initialize GLFW\n" );
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
    
    
    GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow( 1024, 768, "XO", NULL, NULL);
    if(window == NULL) {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent(window); // Initialize GLEW
    
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return EXIT_FAILURE;
    }    
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GL_TRUE);
    glfwSwapInterval(1);
    
    GLuint program;
    program = loadShader();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);

    GLuint vertexArrayID; // vao
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    GLfloat gl_vertex_buffer_data[] = {
        -1.0, -1.0,
        1.0, -1.0,
        1.0, 1.0,
        1.0, 1.0,
        -1.0, 1.0,
        -1.0, -1.0
    };
    GLfloat gl_texcoords_buffer_data[] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0
    };
    
    //fillMap(squares, gl_vertex_buffer_data, gl_texcoords_buffer_data);
    
    GLuint vertexbuffer, vertexId; // vbo
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);

    vertexId = glGetAttribLocation(program, "vertex_position"); // number of array attributes
    
    glVertexAttribPointer(
        vertexId,
        2, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        2 * sizeof(GL_FLOAT), // stride - sdvig - shag
        (void*)0 // array buffer offset - sdvig ot nachala massiva, esli est' naprimer v nachale tochki a potom tsveta
    );
    glEnableVertexAttribArray(vertexId);
    
    /* teksturki miltso... start */

    GLuint colorTexture = 0;
    colorTexture = TextureFromTGA("images/field.tga");

    if(colorTexture == -1) {
        printf("colorTexture - fail\n");
        return EXIT_FAILURE;
    }
    
    glActiveTexture(GL_TEXTURE0);
    
    GLint textureLocation = -1;
    textureLocation = glGetUniformLocation(program, "colorTexture");

    printf("textureLocation %hi\n", textureLocation);

    //glUniform1i(textureLocation, 0);
    
    GLint texcoordLocation = -1;
    texcoordLocation = glGetAttribLocation(program, "texcoord"); // number of array attributes
    printf("texcoordLocation %hi\n", texcoordLocation);
    
    GLuint vbo2;
    glGenBuffers(1, &vbo2);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data), gl_texcoords_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(texcoordLocation);
    
    /* teksturki miltso... end */
    
    //printf("total points: %d\n", (int)((sizeof(gl_vertex_buffer_data) / sizeof(GLfloat)) / 2));
    
    GLuint elementTexture = 0;
    elementTexture = TextureFromTGA("images/textures.tga");

    if(elementTexture == -1) {
        printf("colorTexture - fail\n");
        return EXIT_FAILURE;
    }

//    GLfloat gl_vertex_buffer_data[] = {
//        -1.0, -1.0,
//        1.0, -1.0,
//        1.0, 1.0,
//        1.0, 1.0,
//        -1.0, 1.0,
//        -1.0, -1.0
//    };
    GLfloat gl_texcoords_buffer_data_elements[] = {
        0.0, 0.8,
        0.2, 0.8,
        0.2, 1.0,
        0.2, 1.0,
        0.0, 1.0,
        0.2, 0.8
    };
    
    GLuint vbo3;
    glGenBuffers(1, &vbo3);
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data_elements), gl_texcoords_buffer_data_elements, GL_STATIC_DRAW);
//    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
//    glEnableVertexAttribArray(texcoordLocation);

    // finish init
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);

    // start draw field
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , (int)((sizeof(gl_vertex_buffer_data) / sizeof(GLfloat)) / 2) // total points.
    ); 

    
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // start from 0. total 3 points.
        
        //printf("mousePositionX: %f\n", mousePositionX[0]);
        //printf("mousePositionY: %f\n", mousePositionY[0]);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    //glDisableVertexAttribArray(vertexId);
    //glDisableVertexAttribArray(texcoordLocation);
    
    glfwDestroyWindow(window);
    glfwTerminate();
        
    return EXIT_SUCCESS;
}