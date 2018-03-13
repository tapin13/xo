#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>

#include "loadShader.h"
#include "tgaLoad.h"

#define DELAY_BEFORE_MENU_DRAW 3

#define gamerX 1
#define gamerO 2

GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
int windowWeight, windowHeight;

char gameLoop = 0;
char gameMatrix[3][3] = { { 0 }, { 0 }, { 0 } };
char gamer = gamerX;

GLuint vertexArrayID; // vao
GLuint program; // shaderProgram

// field
GLuint vertexbufferField;
GLuint vbo_texcoords_field;
GLuint fieldTexture = 0;

// menu
GLuint vertexbufferMenu; // vbo
GLuint vbo_texcoords_menu;
GLuint menuTexture = 0;

// elements
GLuint vertexbufferElements; // vbo
GLuint vbo_texcoords_elements;
GLuint elementTexture = 0;

void initMenu() {
    menuTexture = TextureFromTGA("images/menu.tga");
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
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferMenu);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data), gl_vertex_buffer_data, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_texcoords_menu);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_menu);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data), gl_texcoords_buffer_data, GL_STATIC_DRAW);
    
    return;
}

void drawMenu(char wait) {
    if(wait == 1) {
        double time = glfwGetTime();
        while(gameLoop == 0 && glfwGetTime() - time < DELAY_BEFORE_MENU_DRAW) {}
    }
    
    printf("drawMenu\n");
    //glfwSwapBuffers(window); // have to output all graphics that draw before
    
    GLuint vertexId, texcoordId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position");
    texcoordId = glGetAttribLocation(program, "texcoord");
    
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
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
    
    glBindTexture(GL_TEXTURE_2D, menuTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6
    );
    glfwSwapBuffers(window);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);    
}

void drawField() {
    GLuint vertexId, texcoordId;
    vertexId = glGetAttribLocation(program, "vertex_position");
    texcoordId = glGetAttribLocation(program, "texcoord");
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferField);
    glVertexAttribPointer(
        vertexId,
        2, // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        2 * sizeof(GL_FLOAT), // stride - sdvig - shag
        (void*)0 // array buffer offset - sdvig ot nachala massiva, esli est' naprimer v nachale tochki a potom tsveta
    );

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_field);
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0);
    
    glBindTexture(GL_TEXTURE_2D, fieldTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6
    );
    glfwSwapBuffers(window);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);
}

void initGame() {
    printf("initGame\n");
    gameLoop = 1;

    // clean game table
    memset(gameMatrix, 0, 3 * 3 * sizeof(char));
    
    gamer = gamerX;
}

void drawXO(unsigned char position, char gamer) {
    GLuint vertexId, texcoordId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position");
    texcoordId = glGetAttribLocation(program, "texcoord");
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferElements);
    glVertexAttribPointer(vertexId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *) (2 * sizeof(GL_FLOAT) * 6 * position));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_elements);
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *)(2 * sizeof(GL_FLOAT) * 6 * (gamer - 1)));

    glBindTexture(GL_TEXTURE_2D, elementTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6 // total points.
    ); 
    glfwSwapBuffers(window);    
}

int setXO(double xpos, double ypos) {
    //printf("setXO %f %f\n", xpos, ypos);

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
    unsigned char gameMatrixPos = gameMatrixYpos * 3 + gameMatrixXpos;
    
    drawXO(gameMatrixPos, gamer);
    
    return 1;
}

void drawHorizontalCrossLine(int line) {
    printf("drawHorizontalCrossLine\n");
    
    GLuint vertexId, texcoordId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position");
    texcoordId = glGetAttribLocation(program, "texcoord");    
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferElements);
    glVertexAttribPointer(vertexId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *) (2 * sizeof(GL_FLOAT) * 6 * (9 + line)));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_elements);
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *)(2 * sizeof(GL_FLOAT) * 6 * 4));

    glBindTexture(GL_TEXTURE_2D, elementTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6 // total points.
    );     
    glfwSwapBuffers(window);
}

void drawVerticalCrossLine(int line) {
    printf("drawVerticalCrossLine %d\n", line);
    
    GLuint vertexId, texcoordId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position");
    texcoordId = glGetAttribLocation(program, "texcoord");    
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferElements);
    glVertexAttribPointer(vertexId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *) (2 * sizeof(GL_FLOAT) * 6 * (12 + line)));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_elements);
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *)(2 * sizeof(GL_FLOAT) * 6 * 4));

    glBindTexture(GL_TEXTURE_2D, elementTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6 // total points.
    );     
    glfwSwapBuffers(window);
}

void drawXCrossLine(int line) {
    printf("drawXCrossLine %d\n", line);
    
    GLuint vertexId, texcoordId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position");
    texcoordId = glGetAttribLocation(program, "texcoord");    
    
    glBindVertexArray(vertexArrayID);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferElements);
    glVertexAttribPointer(vertexId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *) (2 * sizeof(GL_FLOAT) * 6 * 15));

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_elements);
    glVertexAttribPointer(texcoordId, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *)(2 * sizeof(GL_FLOAT) * 6 * (2 + line)));

    glBindTexture(GL_TEXTURE_2D, elementTexture);
    
    glDrawArrays(GL_TRIANGLES
            , 0 // start from 0
            , 6 // total points.
    );     
    glfwSwapBuffers(window);
}

char checkWin() {
    int i = 0;
    
    for(i = 0; i < 3; i++) {
        if(
                (gameMatrix[i][0] == gamerX && gameMatrix[i][1] == gamerX && gameMatrix[i][2] == gamerX)
                || (gameMatrix[i][0] == gamerO && gameMatrix[i][1] == gamerO && gameMatrix[i][2] == gamerO)) {
            drawHorizontalCrossLine(i);
            return 1;
        }
    }
    
    for(i = 0; i < 3; i++) {
        if(
                (gameMatrix[0][i] == gamerX && gameMatrix[1][i] == gamerX && gameMatrix[2][i] == gamerX)
                || (gameMatrix[0][i] == gamerO && gameMatrix[1][i] == gamerO && gameMatrix[2][i] == gamerO)) {
            drawVerticalCrossLine(i);
            return 1;
        }
    }
    
    if(
            (gameMatrix[0][0] == gamerX && gameMatrix[1][1] == gamerX && gameMatrix[2][2] == gamerX)
            || (gameMatrix[0][0] == gamerO && gameMatrix[1][1] == gamerO && gameMatrix[2][2] == gamerO)) {
        drawXCrossLine(0);
        return 1;
    }
    
    if(
            (gameMatrix[0][2] == gamerX && gameMatrix[1][1] == gamerX && gameMatrix[2][0] == gamerX)
            || (gameMatrix[0][2] == gamerO && gameMatrix[1][1] == gamerO && gameMatrix[2][0] == gamerO)) {
        drawXCrossLine(1);
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
        drawField();
    }
    
    if(xFloats < 0.9 && xFloats > 0.1 && yFloats < -0.5 && yFloats > -0.9) {
        printf("exit\n");
        glfwSetWindowShouldClose(window, 1);
    }
}

void redrawAll() {
    if(gameLoop == 0) {
        drawMenu(0);
        return;
    }
        
    drawField();

    // draw x - o on field
    int i = 0, j = 0;
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            drawXO(i * 3 + j, gameMatrix[i][j]);
        }
    }
    
    if(checkWin() == 1) {
        gameLoop = 0;
        drawMenu(0);
        return;
    }

    if(checkDraw() == 1) {
        gameLoop = 0;
        drawMenu(0);
        return;
    }
}

/*
void window_refresh_callback(GLFWwindow* window) {
    printf("window_refresh_callback\n");
}
*/

void window_size_callback(GLFWwindow* window, int width, int height) {
    printf("window_size_callback %d, %d\n", width, height);
    windowWeight = width;
    windowHeight = height;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    redrawAll();
    
    glViewport(0, 0, width, height);
    
    glfwSwapBuffers(window);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if(gameLoop == 1) {
            printf("gameLoop - left pressed\n" );

            if(setXO(xpos, ypos) != 1) {
                return;
            }

            if(checkWin() == 1) {
                printf("Win: %d\n", gamer);
                gameLoop = 0;
                drawMenu(1);
                return;
            }

            if(checkDraw() == 1) {
                printf("Draw\n" );
                gameLoop = 0;
                drawMenu(1);
                return;
            }

            if(gamer == gamerX) {
                gamer = gamerO;
            } else {
                gamer = gamerX;
            }        
        } else {
            printf("menu - left pressed\n" );
            
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

    // horizontal lines
    
    GLfloat crossLineVertex[] = {
        //horizontal 1
        -0.9, 0.75,
        0.9, 0.75,
        0.9, 0.65,
        0.9, 0.65,
        -0.9, 0.65,
        -0.9, 0.75,
        
        //horizontal 2
        -0.9, 0.05,
        0.9, 0.05,
        0.9, -0.05,
        0.9, -0.05,
        -0.9, -0.05,
        -0.9, 0.05,
        
        //horizontal 3
        -0.9, -0.65,
        0.9, -0.65,
        0.9, -0.75,
        0.9, -0.75,
        -0.9, -0.75,
        -0.9, -0.65,  
        
        //vertical 1
        -0.75, -0.9,
        -0.65, -0.9,
        -0.65, 0.9,
        -0.65, 0.9,
        -0.75, 0.9,
        -0.75, -0.9,  
        
        //vertical 2
        -0.05, -0.9,
        0.05, -0.9,
        0.05, 0.9,
        0.05, 0.9,
        -0.05, 0.9,
        -0.05, -0.9,  
        
        //vertical 3
        0.65, -0.9,
        0.75, -0.9,
        0.75, 0.9,
        0.75, 0.9,
        0.65, 0.9,
        0.65, -0.9,  
        
        // cross
        -1.0, -1.0,
        1.0, -1.0,
        1.0, 1.0,
        1.0, 1.0,
        -1.0, 1.0,
        -1.0, -1.0        
    };
    
    memcpy(&vertex_elements[9 * 2 * 6], crossLineVertex, 7 * 2 * 6 * sizeof(GLfloat));
    
    return;
}

void fillElementsTexCoords(GLfloat texcoords_elements[]) {
    GLfloat gl_texcoords_buffer_data_elements[6 * 2 * 6] = {
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
        
        // texture 3 - slash from left to right
        0.4, 0.8,
        0.6, 0.8,
        0.6, 1.0,
        0.6, 1.0,
        0.4, 1.0,
        0.4, 0.8,
        
        // texture 4 - /
        0.6, 0.8,
        0.8, 0.8,
        0.8, 1.0,
        0.8, 1.0,
        0.6, 1.0,
        0.6, 0.8,
        
        // texture 5 - |
        0.8, 0.8,
        1.0, 0.8,
        1.0, 1.0,
        1.0, 1.0,
        0.8, 1.0,
        0.8, 0.8,
        
        // texture 6 - -
        0.0, 0.6,
        0.2, 0.6,
        0.2, 0.8,
        0.2, 0.8,
        0.0, 0.8,
        0.0, 0.6,        
    };
    
    memcpy(texcoords_elements, gl_texcoords_buffer_data_elements, 6 * 2 * 6 * sizeof(GLfloat));
    
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
    
    window = glfwCreateWindow(100, 100, "XO", NULL, NULL);
    if(window == NULL) {
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetWindowAspectRatio(window, 1, 1); // in general can be removed, just for beauty
    
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

    //glfwSetWindowRefreshCallback(window, window_refresh_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwGetWindowSize(window, &windowWeight, &windowHeight);
    
    program = loadShader();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(program);

    // Init VAO
    glGenVertexArrays(1, &vertexArrayID); // VAO
    glBindVertexArray(vertexArrayID);

    // Init VBO
    GLuint vertexId; // vbo
    vertexId = glGetAttribLocation(program, "vertex_position");
    glEnableVertexAttribArray(vertexId);
    
    GLint texcoordId = -1;
    texcoordId = glGetAttribLocation(program, "texcoord");
    glEnableVertexAttribArray(texcoordId);

    //
    
    GLfloat gl_vertex_buffer_data_field[2 * 6];
    GLfloat gl_texcoords_buffer_data_field[2 * 6];
    fillFieldVertex(gl_vertex_buffer_data_field, gl_texcoords_buffer_data_field);
    
    glGenBuffers(1, &vertexbufferField);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferField);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data_field), gl_vertex_buffer_data_field, GL_STATIC_DRAW);
    
    fieldTexture = TextureFromTGA("images/field.tga");

    if(fieldTexture == -1) {
        printf("fieldTexture - fail\n");
        return EXIT_FAILURE;
    }
    
    glGenBuffers(1, &vbo_texcoords_field);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_field);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data_field), gl_texcoords_buffer_data_field, GL_STATIC_DRAW);
    
    elementTexture = TextureFromTGA("images/elements.tga");
    if(elementTexture == -1) {
        printf("elementTexture - fail\n");
        return EXIT_FAILURE;
    }

    GLfloat gl_vertex_buffer_data_elements[16 * 2 * 6] = { 0 };
    fillElementsVertex(gl_vertex_buffer_data_elements);
    
    GLfloat gl_texcoords_buffer_data_elements[25 * 2 * 6] = { 0 };
    fillElementsTexCoords(gl_texcoords_buffer_data_elements);
    
    glGenBuffers(1, &vertexbufferElements);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferElements);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_buffer_data_elements), gl_vertex_buffer_data_elements, GL_STATIC_DRAW);

    
    glGenBuffers(1, &vbo_texcoords_elements);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords_elements);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_texcoords_buffer_data_elements), gl_texcoords_buffer_data_elements, GL_STATIC_DRAW);
    
    initMenu();
    
    // finish init
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind
    glBindVertexArray(0);

    
    initGame();
    drawField();
    
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        //glfwSwapBuffers(window);

        glfwPollEvents();
    }

    //glDisableVertexAttribArray(vertexId);
    //glDisableVertexAttribArray(texcoordId);
    
    glfwDestroyWindow(window);
    glfwTerminate();
        
    return EXIT_SUCCESS;
}
