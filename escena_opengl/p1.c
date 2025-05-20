#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int activeCamera = 0;

const char* vertex_shader =
    "#version 130\n"
    "in vec3 v_pos;\n"
    "uniform mat4 MVP;\n"
    "void main() {\n"
    "  gl_Position = MVP * vec4(v_pos, 1.0);\n"
    "}\n";

const char* fragment_shader =
    "#version 130\n"
    "uniform vec3 color;\n"
    "out vec4 frag_col;\n"
    "void main() {\n"
    "  frag_col = vec4(color, 1.0);\n"
    "}\n";


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_1) activeCamera = 0;
        else if (key == GLFW_KEY_2) activeCamera = 1;
        else if (key == GLFW_KEY_3) activeCamera = 2;
    }
}


void create_object(GLuint* vao, GLuint* vbo, float* vertices, size_t size) {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

float* generate_sphere(int lat_segments, int lon_segments, int* out_vertex_count) {
    int vertices_count = lat_segments * lon_segments * 6;
    float* vertices = malloc(vertices_count * 3 * sizeof(float));
    if (!vertices) return NULL;

    int idx = 0;
    float radius = 0.5f;

    for (int lat = 0; lat < lat_segments; lat++) {
        float theta1 = (float)lat / lat_segments * M_PI;
        float theta2 = (float)(lat + 1) / lat_segments * M_PI;

        for (int lon = 0; lon < lon_segments; lon++) {
            float phi1 = (float)lon / lon_segments * 2.0f * M_PI;
            float phi2 = (float)(lon + 1) / lon_segments * 2.0f * M_PI;

           
            float x1 = radius * sinf(theta1) * cosf(phi1);
            float y1 = radius * cosf(theta1);
            float z1 = radius * sinf(theta1) * sinf(phi1);

            float x2 = radius * sinf(theta2) * cosf(phi1);
            float y2 = radius * cosf(theta2);
            float z2 = radius * sinf(theta2) * sinf(phi1);

            float x3 = radius * sinf(theta2) * cosf(phi2);
            float y3 = radius * cosf(theta2);
            float z3 = radius * sinf(theta2) * sinf(phi2);

            float x4 = radius * sinf(theta1) * cosf(phi2);
            float y4 = radius * cosf(theta1);
            float z4 = radius * sinf(theta1) * sinf(phi2);

            
            vertices[idx++] = x1; vertices[idx++] = y1; vertices[idx++] = z1;
            vertices[idx++] = x2; vertices[idx++] = y2; vertices[idx++] = z2;
            vertices[idx++] = x3; vertices[idx++] = y3; vertices[idx++] = z3;

            
            vertices[idx++] = x1; vertices[idx++] = y1; vertices[idx++] = z1;
            vertices[idx++] = x3; vertices[idx++] = y3; vertices[idx++] = z3;
            vertices[idx++] = x4; vertices[idx++] = y4; vertices[idx++] = z4;
        }
    }
    *out_vertex_count = vertices_count;
    return vertices;
}


int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: No se pudo iniciar GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "parte1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: No se pudo crear ventana\n");
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    glfwSetKeyCallback(window, key_callback);

    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    float cube_vertices[] = {
        // Frente
        -0.5, -0.5,  0.5,   0.5, -0.5,  0.5,   0.5,  0.5,  0.5,
        -0.5, -0.5,  0.5,   0.5,  0.5,  0.5,  -0.5,  0.5,  0.5,
        // Atrás
        -0.5, -0.5, -0.5,  -0.5,  0.5, -0.5,   0.5,  0.5, -0.5,
        -0.5, -0.5, -0.5,   0.5,  0.5, -0.5,   0.5, -0.5, -0.5,
        // Izquierda
        -0.5, -0.5, -0.5,  -0.5, -0.5,  0.5,  -0.5,  0.5,  0.5,
        -0.5, -0.5, -0.5,  -0.5,  0.5,  0.5,  -0.5,  0.5, -0.5,
        // Derecha
         0.5, -0.5, -0.5,   0.5,  0.5, -0.5,   0.5,  0.5,  0.5,
         0.5, -0.5, -0.5,   0.5,  0.5,  0.5,   0.5, -0.5,  0.5,
        // Superior
        -0.5,  0.5, -0.5,  -0.5,  0.5,  0.5,   0.5,  0.5,  0.5,
        -0.5,  0.5, -0.5,   0.5,  0.5,  0.5,   0.5,  0.5, -0.5,
        // Inferior
        -0.5, -0.5, -0.5,   0.5, -0.5, -0.5,   0.5, -0.5,  0.5,
        -0.5, -0.5, -0.5,   0.5, -0.5,  0.5,  -0.5, -0.5,  0.5,
    };

    GLuint cube_vao, cube_vbo;
    create_object(&cube_vao, &cube_vbo, cube_vertices, sizeof(cube_vertices));

    float pyramid_vertices[] = {
        // Base
        -0.5, -0.5, -0.5,   0.5, -0.5, -0.5,   0.5, -0.5, 0.5,
        -0.5, -0.5, -0.5,   0.5, -0.5, 0.5,   -0.5, -0.5, 0.5,
    
        // Lado 1
        -0.5, -0.5,  0.5,   0.0, 0.5,  0.0,    0.5, -0.5,  0.5,
    
        // Lado 2 
         0.5, -0.5,  0.5,   0.0, 0.5,  0.0,    0.5, -0.5, -0.5,
    
        // Lado 3
         0.5, -0.5, -0.5,   0.0, 0.5,  0.0,   -0.5, -0.5, -0.5,
    
        // Lado 4
        -0.5, -0.5, -0.5,   0.0, 0.5,  0.0,   -0.5, -0.5,  0.5
    };

    GLuint pyramid_vao, pyramid_vbo;
    create_object(&pyramid_vao, &pyramid_vbo, pyramid_vertices, sizeof(pyramid_vertices));


    int sphere_vertex_count;
    float* sphere_vertices = generate_sphere(20, 20, &sphere_vertex_count);
    if (!sphere_vertices) {
        fprintf(stderr, "ERROR: No se pudo crear la esfera\n");
        glfwTerminate();
        return -1;
    }

    GLuint sphere_vao, sphere_vbo;
    create_object(&sphere_vao, &sphere_vbo, sphere_vertices, sphere_vertex_count * 3 * sizeof(float));

    free(sphere_vertices);


    GLint mvp_loc = glGetUniformLocation(shader_program, "MVP");
    GLint color_loc = glGetUniformLocation(shader_program, "color");


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(shader_program);

        // Matrices
        vec3 eye, center = {0,0,0}, up = {0,1,0};
        if (activeCamera == 0) {
            glm_vec3_copy((vec3){0.0f, 0.0f, 4.0f}, eye);
        } else if (activeCamera == 1) {
            glm_vec3_copy((vec3){0.0f, 5.0f, 0.01f}, eye);
        } else {
            glm_vec3_copy((vec3){4.0f, 4.0f, 4.0f}, eye);
        }

        mat4 view, proj, model, mvp;
        glm_lookat(eye, center, up, view);
        glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, proj);


        glm_mat4_identity(model);
        glm_translate(model, (vec3){-1.5f, 0.0f, 0.0f});
        glm_mat4_mulN((mat4 *[]){&proj, &view, &model}, 3, mvp);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
        glUniform3f(color_loc, 1.0f, 0.0f, 0.0f);
        glBindVertexArray(cube_vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices)/sizeof(float)/3);


        glm_mat4_identity(model);
        glm_translate(model, (vec3){0.0f, 0.0f, 0.0f});
        glm_mat4_mulN((mat4 *[]){&proj, &view, &model}, 3, mvp);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
        glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
        glBindVertexArray(pyramid_vao);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(pyramid_vertices)/sizeof(float)/3);


        glm_mat4_identity(model);
        glm_translate(model, (vec3){1.5f, 0.0f, 0.0f});
        glm_mat4_mulN((mat4 *[]){&proj, &view, &model}, 3, mvp);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
        glUniform3f(color_loc, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(sphere_vao);
        glDrawArrays(GL_TRIANGLES, 0, sphere_vertex_count);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cube_vao);
    glDeleteBuffers(1, &cube_vbo);
    glDeleteVertexArrays(1, &pyramid_vao);
    glDeleteBuffers(1, &pyramid_vbo);
    glDeleteVertexArrays(1, &sphere_vao);
    glDeleteBuffers(1, &sphere_vbo);
    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
