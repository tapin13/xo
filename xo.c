#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>

#include "loadShader.h"
#include "tgaLoad.h"

#define gamerX 1
#define gamerO 2

int windowWeight, windowHeight;

char gameLoop = 0;
char gameMatrix[3][3] = { { 0 }, { 0 }, { 0 } };
char gamer = gamerX;

GLuint vertexArrayID; // vao
GLuint program; // shaderProgram


// menu
GLuint menuTexture = 0;
GLuint vertexbufferMenu; // vbo
GLuint vbo_texcoords_menu;

void initMenu() {
    menuTexture = TextureFromTGA("images/menu.tga");

    printf("menuTexture Id %hi\n", menuTexture);
    
    if(menuTexture == -1) {
        printf("menuTexture - fail\n");
        return;
    }

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
    
    glGenBuffers(1, &vertexbufferMenu);
    
    printf("vertexbufferMenu Id: %d\n", vertexbufferMenu);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferMenu);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_texcoords_menu);
    
    printf("vbo_texcoords_menu Id: %d\n", vbo_texcoords_menu);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_menu);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data), gl_texcoords_buffer_data, GL_STATIC_DRAW);
    
    return;
}

void drawMenu() {
    GLuint vertexId, texcoordLocation; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position"); // number of array attributes
    texcoordLocation = glGetAttribLocation(program, "texcoord"); // number of array attributes
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferMenu);
    glVertexAttribPointer(
        vertexId,
        2, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        2 * sizeof(GL_FLOAT), // stride - sdvig - shag
        (void*)0 // array buffer offset - sdvig ot nachala massiva, esli est' naprimer v nachale tochki a potom tsveta
    );

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_menu);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
    
    glBindTexture(GL_TEXTURE_2D, menuTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6
    );
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);    
}

void drawField(const GLuint vertexArrayID, const GLuint vertexbuffer
                , const GLuint vertexId, const GLuint vbo_texcoords_field
                , const GLuint texcoordLocation, const GLuint fieldTexture
                , const int totalPoints) {
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        vertexId,
        2, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        2 * sizeof(GL_FLOAT), // stride - sdvig - shag
        (void*)0 // array buffer offset - sdvig ot nachala massiva, esli est' naprimer v nachale tochki a potom tsveta
    );

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_field);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
    
    glBindTexture(GL_TEXTURE_2D, fieldTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , totalPoints
    );
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);
}

void initGame() {
    gameLoop = 1;
    char tmp[3][3] = { { 0 }, { 0 }, { 0 } };
    
    memcpy(gameMatrix, tmp, 3 * 3 * sizeof(char));
    
    gamer = gamerX;    
}

int setXO(double xpos, double ypos) {
    printf("setXO %f %f\n", xpos, ypos);

    unsigned char gameMatrixXpos = 0;
    unsigned char gameMatrixYpos = 0;
    
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
    
    if(gameMatrix[gameMatrixYpos][gameMatrixXpos] != 0) {
        return 0;
    }
    
    gameMatrix[gameMatrixYpos][gameMatrixXpos] = gamer;
    
    glBindVertexArray(1); // vertexArrayID
    
    glBindBuffer(GL_ARRAY_BUFFER, 3); // vertexbufferElements
    
    unsigned char gameMatrixPos = gameMatrixYpos * 3 + gameMatrixXpos;
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *) (2 * sizeof(GL_FLOAT) * 6 * gameMatrixPos)); // 0 - vertexId
    glBindBuffer(GL_ARRAY_BUFFER, 4); // vbo_texcoords_elements

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *)(2 * sizeof(GL_FLOAT) * 6 * (gamer - 1))); // 0 - texcoordLocation
    glBindTexture(GL_TEXTURE_2D, 2); // elementTexture
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6 // total points.
    ); 

    return 1;
}

char checkWin() {
    int i = 0;
    
    for(i = 0; i < 3; i++) {
        if(
                (gameMatrix[i][0] == gamerX && gameMatrix[i][1] == gamerX && gameMatrix[i][2] == gamerX)
                || (gameMatrix[i][0] == gamerO && gameMatrix[i][1] == gamerO && gameMatrix[i][2] == gamerO)) {
            return 1;
        }
    }
    
    for(i = 0; i < 3; i++) {
        if(
                (gameMatrix[0][i] == gamerX && gameMatrix[1][i] == gamerX && gameMatrix[2][i] == gamerX)
                || (gameMatrix[0][i] == gamerO && gameMatrix[1][i] == gamerO && gameMatrix[2][i] == gamerO)) {
            return 1;
        }
    }
    
    if(
            (gameMatrix[0][0] == gamerX && gameMatrix[1][1] == gamerX && gameMatrix[2][2] == gamerX)
            || (gameMatrix[0][0] == gamerO && gameMatrix[1][1] == gamerO && gameMatrix[2][2] == gamerO)) {
        return 1;
    }
    
    if(
            (gameMatrix[0][2] == gamerX && gameMatrix[1][1] == gamerX && gameMatrix[2][0] == gamerX)
            || (gameMatrix[0][2] == gamerO && gameMatrix[1][1] == gamerO && gameMatrix[2][0] == gamerO)) {
        return 1;
    }
    
    return 0;
}

char checkDraw() {
    unsigned char i = 0, j = 0;
    
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            if(gameMatrix[i][j] == 0) {
                return 0;
            }
        }
    }
    
    return 1;
}

void menuSelect(double xpos, double ypos) {

    float xFloats = (xpos / windowWeight);

    xFloats *= 2;
    xFloats -= 1;

    float yFloats = (ypos / windowHeight);

    yFloats *= 2;
    yFloats = 1 - yFloats;

    //printf("Floats: %f, %f\n", xFloats, yFloats);

    if(xFloats > -0.9 && xFloats < -0.1 && yFloats < -0.5 && yFloats > -0.9) {
        printf("go next\n");
        initGame();
    }
    
    if(xFloats < 0.9 && xFloats > 0.1 && yFloats < -0.5 && yFloats > -0.9) {
        printf("exit\n");
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if(gameLoop == 1) {
            printf("gameLoop left pressed\n" );

            if(setXO(xpos, ypos) != 1) {
                return;
            }

            if(checkWin() == 1) {
                printf("Win: %d\n", gamer);
                gameLoop = 0;
                drawMenu();
                return;
            }

            if(checkDraw() == 1) {
                printf("Draw\n" );
                gameLoop = 0;
                drawMenu();
                return;
            }

            if(gamer == gamerX) {
                gamer = gamerO;
            } else {
                gamer = gamerX;
            }        
        } else {
            printf("menu left pressed\n" );
            
            menuSelect(xpos, ypos);
        }
    }
}

void fillElementsVertex(GLfloat vertex_elements[]) {
    GLfloat gl_vertex_buffer_data_elements[9 * 2 * 6] = {
        // element position 1
        -0.9, 0.5,
        -0.5, 0.5,
        -0.5, 0.9,
        -0.5, 0.9,
        -0.9, 0.9,
        -0.9, 0.5,
                
        // element position 2
        -0.2, 0.5,
        0.2, 0.5,
        0.2, 0.9,
        0.2, 0.9,
        -0.2, 0.9,
        -0.2, 0.5,
                
        // element position 3
        0.5, 0.5,
        0.9, 0.5,
        0.9, 0.9,
        0.9, 0.9,
        0.5, 0.9,
        0.5, 0.5                
    };
    
    int j = 0;
    
    for(j = 0; j < 6 * 2 * 3; j++) {
        if(j % 2 == 0) {
            gl_vertex_buffer_data_elements[(3 * 2 * 6) + j] = gl_vertex_buffer_data_elements[j];
        } else {
            gl_vertex_buffer_data_elements[(3 * 2 * 6) + j] = gl_vertex_buffer_data_elements[j] - 0.7;
        }
    }

    for(j = 0; j < 6 * 2 * 3; j++) {
        if(j % 2 == 0) {
            gl_vertex_buffer_data_elements[(6 * 2 * 6) + j] = gl_vertex_buffer_data_elements[j];
        } else {
            gl_vertex_buffer_data_elements[(6 * 2 * 6) + j] = gl_vertex_buffer_data_elements[j] - 1.4;
        }
    }
    
    memcpy(vertex_elements, gl_vertex_buffer_data_elements, 9 * 2 * 6 * sizeof(GLfloat));
    
    return;
}

void fillElementsTexCoords(GLfloat texcoords_elements[]) {
    GLfloat gl_texcoords_buffer_data_elements[2 * 2 * 6] = {
        // texture 1
        0.0, 0.8,
        0.2, 0.8,
        0.2, 1.0,
        0.2, 1.0,
        0.0, 1.0,
        0.0, 0.8,
                
        // texture 2
        0.2, 0.8,
        0.4, 0.8,
        0.4, 1.0,
        0.4, 1.0,
        0.2, 1.0,
        0.2, 0.8,
    };
    
    memcpy(texcoords_elements, gl_texcoords_buffer_data_elements, 2 * 2 * 6 * sizeof(GLfloat));
    
    return;
}

void fillFieldVertex(GLfloat vertex_buffer_data[], GLfloat texcoords_buffer_data[]) {
    GLfloat gl_vertex_buffer_data_field[2 * 6] = {
        -1.0, -1.0,
        1.0, -1.0,
        1.0, 1.0,
        1.0, 1.0,
        -1.0, 1.0,
        -1.0, -1.0
    };
    
    memcpy(vertex_buffer_data, gl_vertex_buffer_data_field, 2 * 6 * sizeof(GLfloat));
    
    GLfloat gl_texcoords_buffer_data_field[2 * 6] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0
    };
    
    memcpy(texcoords_buffer_data, gl_texcoords_buffer_data_field, 2 * 6 * sizeof(GLfloat));
    
    return;
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
    window = glfwCreateWindow(100, 100, "XO", NULL, NULL);
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

    glfwGetWindowSize(window, &windowWeight, &windowHeight);
    
    program = loadShader();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    
    GLuint vertexId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position"); // number of array attributes
    glEnableVertexAttribArray(vertexId);
    printf("vertexId (shader ID of vertex_position): %d\n", vertexId);
    
    GLint texcoordLocation = -1;
    texcoordLocation = glGetAttribLocation(program, "texcoord"); // number of array attributes
    glEnableVertexAttribArray(texcoordLocation);
    printf("texcoordLocation (shader ID of texcoord): %hi\n", texcoordLocation);

    //
    
    glGenVertexArrays(1, &vertexArrayID);
    
    printf("vertexArrayID Id (VAO): %d\n", vertexArrayID);
    
    glBindVertexArray(vertexArrayID);
    
    GLfloat gl_vertex_buffer_data_field[2 * 6];
    GLfloat gl_texcoords_buffer_data_field[2 * 6];
    fillFieldVertex(gl_vertex_buffer_data_field, gl_texcoords_buffer_data_field);
    
    GLuint vertexbuffer;

    glGenBuffers(1, &vertexbuffer);
    
    printf("vertexbuffer Id (field): %d\n", vertexbuffer);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data_field), gl_vertex_buffer_data_field, GL_STATIC_DRAW);
    
    /* teksturki miltso... start */

    GLuint fieldTexture = 0;
    fieldTexture = TextureFromTGA("images/field.tga");

    printf("fieldTexture Id %hi\n", fieldTexture);
    
    if(fieldTexture == -1) {
        printf("fieldTexture - fail\n");
        return EXIT_FAILURE;
    }
    
    GLuint vbo_texcoords_field;
    glGenBuffers(1, &vbo_texcoords_field);
    
    printf("vbo_texcoords_field Id: %d\n", vbo_texcoords_field);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_field);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data_field), gl_texcoords_buffer_data_field, GL_STATIC_DRAW);
    
    //glBindTexture(GL_TEXTURE_2D, 0); // unbind
    
    /* teksturki miltso... end */
    
    //printf("total points: %d\n", (int)((sizeof(gl_vertex_buffer_data_field) / sizeof(GLfloat)) / 2));
    
    GLuint elementTexture = 0;
    elementTexture = TextureFromTGA("images/elements.tga");

    printf("elementTexture Id %hi\n", elementTexture);
    
    if(elementTexture == -1) {
        printf("elementTexture - fail\n");
        return EXIT_FAILURE;
    }

    GLfloat gl_vertex_buffer_data_elements[9 * 2 * 6] = { 0 };
    fillElementsVertex(gl_vertex_buffer_data_elements);
    
    GLfloat gl_texcoords_buffer_data_elements[2 * 2 * 6] = { 0 };
    fillElementsTexCoords(gl_texcoords_buffer_data_elements);
    
    GLuint vertexbufferElements; // vbo
    glGenBuffers(1, &vertexbufferElements);
    
    printf("vertexbufferElements Id: %d\n", vertexbufferElements);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferElements);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data_elements), gl_vertex_buffer_data_elements, GL_STATIC_DRAW);

    
    GLuint vbo_texcoords_elements;
    glGenBuffers(1, &vbo_texcoords_elements);
    
    printf("vbo_texcoords_elements Id: %d\n", vbo_texcoords_elements);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_elements);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data_elements), gl_texcoords_buffer_data_elements, GL_STATIC_DRAW);
    
    initMenu();
    
    // finish init
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);

    drawField(vertexArrayID, vertexbuffer
                , vertexId, vbo_texcoords_field
                , texcoordLocation, fieldTexture
                , (int)((sizeof(gl_vertex_buffer_data_field) / sizeof(GLfloat)) / 2));
    
    gameLoop = 1;
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //glDisableVertexAttribArray(vertexId);
    //glDisableVertexAttribArray(texcoordLocation);
    
    glfwDestroyWindow(window);
    glfwTerminate();
        
    return EXIT_SUCCESS;
}