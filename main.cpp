#include "model.h"
#include "camera.h"
#include "light.h"
#include "shader.h"

#define pi 3.141592653589793238462643383279502884197

float moveSpeed = 0.001f;
float camSpeed = 0.001f;

std::unordered_map<int, bool> keyStates;

float x_mod = 0;
float y_mod = 0;
float z_mod = -20;

float x_cam = 0;
float y_cam = 0;
float z_cam = 0;

float yaw = 270;
float pitch = 0;

float scaleVal = 1.5;

float thetaX = 0;
float thetaY = 0;
float thetaZ = 0;

float axisX = 1;
float axisY = 0;

//glm::mat4 identity_matrix(1.0);

bool isDaySkybox = true;

bool isPressedD, isPressedA, isPressedW, isPressedS, 
isPressedQ, isPressedE, isPressedZ, isPressedX,
isPressedF, isPressedH, isPressedT, isPressedG, isPressedC, isPressedV,
isPressedLEFT, isPressedRIGHT, isPressedUP, isPressedDOWN;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        keyStates[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}

// Function to process input
void ProcessInput() {

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front); 

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Object movement
    if (keyStates[GLFW_KEY_H]) { 
        x_mod += moveSpeed; 
        std::cout << "CurrX: " << x_mod << std::endl;
    }
    if (keyStates[GLFW_KEY_F]) {
        x_mod -= moveSpeed; 
        std::cout << "CurrX: " << x_mod << std::endl;
    } 
    if (keyStates[GLFW_KEY_T]) {
        y_mod += moveSpeed; 
        std::cout << "CurrY: " << y_mod << std::endl;
    } 
    if (keyStates[GLFW_KEY_G]) {
        y_mod -= moveSpeed; 
        std::cout << "CurrY: " << y_mod << std::endl;
    } 
    if (keyStates[GLFW_KEY_Z]) {
        z_mod -= moveSpeed; 
        std::cout << "CurrZ: " << z_mod << std::endl;
    } 
    if (keyStates[GLFW_KEY_X]) {
        z_mod += moveSpeed; 
        std::cout << "CurrZ: " << z_mod << std::endl;
    } 

    if (keyStates[GLFW_KEY_W]) {
        x_cam += front.x * camSpeed;
        z_cam += front.z * camSpeed;
    };
    if (keyStates[GLFW_KEY_S]) {
        x_cam -= front.x * camSpeed;
        z_cam -= front.z * camSpeed;
    };
    if (keyStates[GLFW_KEY_A]) {
        x_cam -= right.x * camSpeed;
        z_cam -= right.z * camSpeed;
    };
    if (keyStates[GLFW_KEY_D]) {
        x_cam += right.x * camSpeed;
        z_cam += right.z * camSpeed;
    };

    if (keyStates[GLFW_KEY_SPACE]) {
        
    };

    // Scaling
    if (keyStates[GLFW_KEY_Q]) scaleVal -= 0.0001f;
    if (keyStates[GLFW_KEY_E]) scaleVal += 0.0001f;

    // Rotation
    if (keyStates[GLFW_KEY_UP]) pitch += 0.01;
    if (keyStates[GLFW_KEY_DOWN]) pitch -= 0.01;

    if (keyStates[GLFW_KEY_LEFT]) yaw -= 0.01;
    if (keyStates[GLFW_KEY_RIGHT]) yaw += 0.01;
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float windowWidth = 640;
    float windowHeight = 640;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "GDGRAP1 Machine Project-Ruiz_Marlou Vincent-Obcena_Zion", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //          MinX MinY Width Height
    //glViewport(320, 0, 640/2, 480);

    glfwSetKeyCallback(window, Key_Callback);


    Shader mainShader("Shaders/sample.vert", "Shaders/sample.frag");
    Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");

    Model3D testModel(
        "3D/plane.obj",          // Model path
        "3D/brickwall.jpg",        // Texture path (optional)
        "3D/brickwall_normal.jpg",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(1.0f, 1.0f, 1.0f)   // Initial scale
    );


    //Vertices for the cube
    float skyboxVertices[]{
    -1.f, -1.f, 1.f, //0
    1.f, -1.f, 1.f,  //1
    1.f, -1.f, -1.f, //2
    -1.f, -1.f, -1.f,//3
    -1.f, 1.f, 1.f,  //4
    1.f, 1.f, 1.f,   //5
    1.f, 1.f, -1.f,  //6
    -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
    1,2,6,
    6,5,1,

    0,4,7,
    7,3,0,

    4,5,6,
    6,7,4,

    0,3,2,
    2,1,0,

    0,1,5,
    5,4,0,

    3,7,6,
    6,2,3
    };

    /////////
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenVertexArrays(1, &skyboxVBO);
    glGenVertexArrays(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    //////////////////////////////////////////////////////////////////////////////////////////////

    //Day time skybox
    std::string dayFacesSkybox[]{
    "Skybox/Day/day_rt.png",
    "Skybox/Day/day_lf.png",
    "Skybox/Day/day_up.png",
    "Skybox/Day/day_dn.png",
    "Skybox/Day/day_ft.png",
    "Skybox/Day/day_bk.png"
    };

    //night time skybox
    std::string nightFacesSkybox[]{
    "Skybox/Night/night_rt.png",
    "Skybox/Night/night_lf.png",
    "Skybox/Night/night_up.png",
    "Skybox/Night/night_dn.png",
    "Skybox/Night/night_ft.png",
    "Skybox/Night/night_bk.png"
    };

    unsigned int skyboxTex;

    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCChannel;
        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(
            nightFacesSkybox[i].c_str(),
            &w,
            &h,
            &skyCChannel,
            0
        );

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
        }
        stbi_image_free(data);

    }
    stbi_set_flip_vertically_on_load(true);

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(60.0f), //FOV
        windowHeight / windowWidth, //Aspect Ratio
        0.1f, //Z near should never be <= 0
        100.f //Z far
    );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    //Light position
    glm::vec3 lightPos = glm::vec3(-10, 3, 0);
    //light color
    glm::vec3 lightColor = glm::vec3(1, 1, 1); 
    float brightness = 10;
    //Ambient Color
    glm::vec3 ambientColor = lightColor;
    //Ambient Str
    float ambientStr = 0.5f;
    
    float specStr = 3.0f;
    float specPhong = 20;


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        ProcessInput();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Position Matrix
        glm::vec3 cameraPos = glm::vec3(0 + x_cam, 0 + y_cam, 0 + z_cam); //eye
        glm::mat4 cameraPosMatrix = glm::translate(glm::mat4(1.0f), cameraPos * -1.0f);

        //Orientation
        glm::vec3 worldUp = glm::normalize(glm::vec3(0.0f, 1.0f , 0.0f )); //Pointing upwards

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);

 
        glm::vec3 cameraCenter = cameraPos + front;

        //Forward
        glm::vec3 F = cameraCenter - cameraPos;
        F = glm::normalize(F);

        //R = F X WorldUp
        glm::vec3 R = glm::cross(F, worldUp);
        //U = R X F
        glm::vec3 U = glm::cross(R, F);

        glm::mat4 cameraOrientation = glm::mat4(1.);
        //Matrix [Column][Row]
        //Row 1 = R
        cameraOrientation[0][0] = R.x;
        cameraOrientation[1][0] = R.y;
        cameraOrientation[2][0] = R.z;

        //Row 2 = U
        cameraOrientation[0][1] = U.x;
        cameraOrientation[1][1] = U.y;
        cameraOrientation[2][1] = U.z;

        //Row 3 = -F
        cameraOrientation[0][2] = -F.x;
        cameraOrientation[1][2] = -F.y;
        cameraOrientation[2][2] = -F.z;
        //end of Orientation Matrix

        glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraCenter, worldUp);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glm::mat4 sky_view = glm::mat4(1.0);
        sky_view = glm::mat4(
            glm::mat3(viewMatrix)
        );

        skyboxShader.use();
        unsigned int viewLoc = glGetUniformLocation(skyboxShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));
        unsigned int projLoc = glGetUniformLocation(skyboxShader.ID, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        //translation
        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x_mod, y_mod, z_mod));

        //scale
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scaleVal, scaleVal, scaleVal));

        //rotation
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaX), glm::vec3(1, 0, 0)); 
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaY), glm::vec3(0, 1, 0)); 
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaZ), glm::vec3(0, 0, 1)); 


        mainShader.use();
        viewLoc = glGetUniformLocation(mainShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        projLoc = glGetUniformLocation(mainShader.ID, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        GLuint lightAddress = glGetUniformLocation(mainShader.ID, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));
        GLuint lightColorAddress = glGetUniformLocation(mainShader.ID, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));
        GLuint ambientStrAddress = glGetUniformLocation(mainShader.ID, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);
        GLuint ambientColorAddress = glGetUniformLocation(mainShader.ID, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));
        GLuint cameraPosAddress = glGetUniformLocation(mainShader.ID, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));
        GLuint specStrAddress = glGetUniformLocation(mainShader.ID, "specStr");
        glUniform1f(specStrAddress, specStr);
        GLuint specPhongAddress = glGetUniformLocation(mainShader.ID, "specPhong");
        glUniform1f(specPhongAddress, specPhong);
        GLuint tex0Address = glGetUniformLocation(mainShader.ID, "tex0");
        glUniform1i(tex0Address, 0);        
        GLuint normTexAddress = glGetUniformLocation(mainShader.ID, "norm_tex");
        glUniform1i(normTexAddress, 1);

        testModel.updateTransform(
            thetaX, thetaY, thetaZ,  // Rotation
            x_mod, y_mod, z_mod,     // Position
            scaleVal                 // Scale
        );

        testModel.draw(mainShader.ID);  // Pass the shader program ID

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
};