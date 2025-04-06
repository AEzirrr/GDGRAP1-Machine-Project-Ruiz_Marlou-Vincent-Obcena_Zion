#include "model.h"
#include "camera.h"
#include "light.h"
#include "shader.h"

#define pi 3.141592653589793238462643383279502884197

float moveSpeed = 0.055f;
float turnSpeed = 0.05f;
float camSpeed = 0.01f;

float ghost1Speed = 0.07f;
float ghost2Speed = 0.04f;

std::unordered_map<int, bool> keyStates;

///////// PLAYER VARIABLES /////////

float x_mod = -20;
float y_mod = 0;
float z_mod = 0;

float thetaX = 0;
float thetaY = 0;
float thetaZ = 0;

float x_cam = 0;
float y_cam = 0;
float z_cam = 0;

float yaw = 0;
float pitch = -15;

// 1st person camera
float frontCamX = 0;
float frontCamY = 0;
float frontCamZ = 0;
float frontCamYaw = 0;
float frontCamPitch = 0;

////////////////////////////////////

///////// GHOST VARIABLES /////////

float ghost1x_mod = -20;
float ghost1y_mod = 0;
float ghost1z_mod = -20;

float ghost2x_mod = -20;
float ghost2y_mod = 0;
float ghost2z_mod = 20;

bool ghostPaused = false;
bool spaceKeyPressed = false;

////////////////////////////////////

float scaleVal = 1.5;
float roadScaleVal = 50;

float axisX = 1;
float axisY = 0;

double lastMouseX = 320.0f, lastMouseY = 320.0f; // Last mouse position
bool isMouseActive = true; // bool to check if the mouse is active
bool mouseControlEnabled = true; // bool to check if mouse control is enabled

bool useFrontCamera = false;
bool zKeyPressed = false;

///////// RACE VARIABLES /////////

double startTime;
bool countdownActive = true;

double raceTime;
bool raceFinished = false;

bool playerFinished = false;
bool playerTimeRecorded = false;
double playerLapTime;

bool ghost1Finished = false;
bool ghost1TimeRecorded = false;
double ghost1LapTime;

bool ghost2Finished = false;
bool ghost2TimeRecorded = false;
double ghost2LapTime;

//////////////////////////////////

bool isPressedD, isPressedA, isPressedW, isPressedS, 
isPressedQ, isPressedE, isPressedZ, isPressedX,
isPressedF, isPressedH, isPressedT, isPressedG, isPressedC, isPressedV,
isPressedLEFT, isPressedRIGHT, isPressedUP, isPressedDOWN;

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
std::string* currentSkybox = nightFacesSkybox;
bool isDaySkybox = true;

void LoadSkyboxTextures(std::string skyboxFaces[]) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCChannel;
        stbi_set_flip_vertically_on_load(false);

        unsigned char* data = stbi_load(
            skyboxFaces[i].c_str(), &w, &h, &skyCChannel, 0
        );

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        stbi_image_free(data);
    }

    stbi_set_flip_vertically_on_load(true);
}

void Mouse_Callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = 320.0f, lastY = 320.0f; // Initial mouse position

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f; // Adjust this value to control the sensitivity
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Constrain the pitch to prevent screen flipping
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    else {
        firstMouse = true; // Reset the flag when the left mouse button is released
    }
}

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
    if (countdownActive) {
        return; // Do not process input if countdown is active
    }

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Object movement

	if (playerFinished == false) {
        //move forward
        if (keyStates[GLFW_KEY_W]) {
            x_mod += moveSpeed;
            if (!keyStates[GLFW_KEY_A] && !keyStates[GLFW_KEY_D]) { //if not turning just go straight
                if (thetaY > 0) {
                    thetaY -= 0.1f;
                    frontCamYaw += 0.1f;
                }
                else if (thetaY < 0) {
                    thetaY += 0.1f;
                    frontCamYaw -= 0.1f;
                }
            }
        }
        //move backwards
        if (keyStates[GLFW_KEY_S]) {
            x_mod -= moveSpeed;
            if (!keyStates[GLFW_KEY_A] && !keyStates[GLFW_KEY_D]) { //if not turning just go straight
                if (thetaY > 0) {
                    thetaY -= 0.1f;
                    frontCamYaw += 0.1f;
                }
                else if (thetaY < 0) {
                    thetaY += 0.1f;
                    frontCamYaw -= 0.1f;
                }
            }
        }
        //turn left
        if (keyStates[GLFW_KEY_A]) {
            if (keyStates[GLFW_KEY_W] || keyStates[GLFW_KEY_S]) { //only be able to turn if moving
                z_mod -= moveSpeed;
            }

            if (thetaY < 8.0f) {
                thetaY += 0.1f;
                frontCamYaw -= 0.1f;
            }
            std::cout << "CurrZ: " << z_mod << std::endl;
        }
        //turn right
        if (keyStates[GLFW_KEY_D]) {
            if (keyStates[GLFW_KEY_W] || keyStates[GLFW_KEY_S]) { //only be able to turn if moving
                z_mod += moveSpeed;
            }

            if (thetaY > -8.0f) {
                thetaY -= 0.1f;
                frontCamYaw += 0.1f;
            }
            std::cout << "CurrZ: " << z_mod << std::endl;
        }
	}

    // Ghost pausing
    if (keyStates[GLFW_KEY_SPACE] && !spaceKeyPressed) {
        ghostPaused = !ghostPaused; // Toggle camera
        spaceKeyPressed = true;
    }
    else if (!keyStates[GLFW_KEY_SPACE]) {
        spaceKeyPressed = false;
    }

    // Camera switching
    if (keyStates[GLFW_KEY_Z] && !zKeyPressed) {
        useFrontCamera = !useFrontCamera; // Toggle camera
        zKeyPressed = true;
    }
    else if (!keyStates[GLFW_KEY_Z]) {
        zKeyPressed = false;
    }

    // Lighting
    // Morning
    if (keyStates[GLFW_KEY_Q]) {
        currentSkybox = dayFacesSkybox;
        LoadSkyboxTextures(dayFacesSkybox);
        lightColor = warmLightColor;  // Set to warm light
        ambientColor = warmLightColor * 0.5f;  // Match ambient to light color
        pointLightBrightness = 0; // Set point light brightness to 0
    };
    // Night
    if (keyStates[GLFW_KEY_E]) {
        currentSkybox = nightFacesSkybox;
        LoadSkyboxTextures(nightFacesSkybox);
        lightColor = coldLightColor;  // Set to cold light
        ambientColor = coldLightColor * 0.5f;  // Match ambient to light color
        pointLightBrightness = 2; // Set point light brightness to 2
    };

    // Rotation
    if (keyStates[GLFW_KEY_UP]) pitch += 0.05;
    if (keyStates[GLFW_KEY_DOWN]) pitch -= 0.05;

    if (keyStates[GLFW_KEY_LEFT]) yaw -= 0.05;
    if (keyStates[GLFW_KEY_RIGHT]) yaw += 0.05;
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float windowWidth = 1080;
    float windowHeight = 1080;

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

    startTime = glfwGetTime();

    glfwSetCursorPosCallback(window, Mouse_Callback);
    glfwSetKeyCallback(window, Key_Callback);

    // Shaders
    Shader mainShader("Shaders/sample.vert", "Shaders/sample.frag");
    Shader skyboxShader("Shaders/skybox.vert", "Shaders/skybox.frag");
    Shader noNormalShader("Shaders/nonormal.vert", "Shaders/nonormal.frag");

    LightManager lightManager;

    // Lights
    PointLight pLight1(
        glm::vec3(0.0f, 5.0f, 0.0f), // Position
        glm::vec3(1.0f, 1.0f, 1.0f), // Color
        1.f                          // Brightness
    );

    PointLight pLight2(
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        1.f
    );

    PointLight pLight3(
        glm::vec3(0.0f, 5.0f, 0.0f), // Position
        glm::vec3(1.0f, 1.0f, 1.0f), // Color
        1.f                          // Brightness
    );

    PointLight pLight4(
        glm::vec3(0.0f, 5.0f, 0.0f), // Position
        glm::vec3(1.0f, 1.0f, 1.0f), // Color
        1.f                          // Brightness
    );

    PointLight pLight5(
        glm::vec3(0.0f, 5.0f, 0.0f), // Position
        glm::vec3(1.0f, 1.0f, 1.0f), // Color
        1.f                          // Brightness
    );

    PointLight pLight6(
        glm::vec3(0.0f, 5.0f, 0.0f), // Position
        glm::vec3(1.0f, 1.0f, 1.0f), // Color
        1.f                          // Brightness
    );

    // Add point lights
    lightManager.AddPointLight(pLight1);
    lightManager.AddPointLight(pLight2);
    lightManager.AddPointLight(pLight3);
    lightManager.AddPointLight(pLight4);
    lightManager.AddPointLight(pLight5);
    lightManager.AddPointLight(pLight6);

    DirectionalLight directionalLight(
        glm::vec3(0.0f, 30.0f, -50.0f),   // Position
        glm::vec3(5.0f, -10.0f, -50.0f),  // Target
        lightColor,                       // Color
        1.f                               // Brightness
    );

    Model3D kartModel(
        "3D/Kart/GoKart.obj",          // Model path
        "3D/Kart/GoKart.jpg",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D ghostModel1(
        "3D/Kart2/ghostKart1.obj",          // Model path
        "3D/Kart2/ghostKart1.png",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D ghostModel2(
        "3D/Kart3/ghostKart2.obj",          // Model path
        "3D/Kart3/ghostKart2.png",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D roadModel1(
        "3D/plane.obj",          // Model path
        "3D/road-texture.jpg",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D roadModel2(
        "3D/plane.obj",          // Model path
        "3D/road-texture.jpg",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D roadModel3(
        "3D/plane.obj",          // Model path
        "3D/road-texture.jpg",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D roadModel4(
        "3D/plane.obj",          // Model path
        "3D/road-texture.jpg",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D roadModel5(
        "3D/plane.obj",          // Model path
        "3D/roadFinish-texture.jpg",        // Texture path (optional)
        "",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D landmark1(
        "3D/Landmark/landmark1.obj",          // Model path
        "3D/Landmark/landmark1.jpeg",        // Texture path (optional)
        "3D/Landmark/landmark1_norm.jpeg",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
    );

    Model3D landmark2(
        "3D/Landmark/landmark2.obj",          // Model path
        "3D/Landmark/landmark2.jpg",        // Texture path (optional)
        "3D/Landmark/landmark2_norm.jpg",     // Normal map path (optional)
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Initial rotation (degrees)
        glm::vec3(50.0f, 50.0f, 50.0f)   // Initial scale
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

    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set skybox to morning
    currentSkybox = dayFacesSkybox;
    LoadSkyboxTextures(dayFacesSkybox);
    lightColor = warmLightColor;  // Set to warm light
    ambientColor = warmLightColor * 0.5f;  // Match ambient to light color
    isWarmLight = true;

    glm::vec3 lightPos1 = glm::vec3(x_mod, 2.f, z_mod);

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(80.0f), //FOV
        windowHeight / windowWidth, //Aspect Ratio
        0.1f, //Z near should never be <= 0
        500.f //Z far
    );

    glEnable(GL_DEPTH_TEST);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime ;

        static int lastPrintedSecond = -1;

        if (elapsedTime < 5.0) {
            int remainingTime = static_cast<int>(5.0 - elapsedTime) + 1; // Calculate remaining time and round up
            if (remainingTime != lastPrintedSecond) {
                std::cout << "Countdown: " << remainingTime << " seconds remaining" << std::endl;
                lastPrintedSecond = remainingTime;
            }
        }
        else {
            if (countdownActive) {
                std::cout << "Lights out and away we go!" << std::endl;
                countdownActive = false; // Stop the countdown after 5 seconds
            }
        }

        ProcessInput();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Orientation
        glm::vec3 worldUp = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)); // Pointing upwards

        glm::vec3 thirdPerson_front;
        thirdPerson_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        thirdPerson_front.y = sin(glm::radians(pitch));
        thirdPerson_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        thirdPerson_front = glm::normalize(thirdPerson_front);

        glm::vec3 firstPerson_front;
        firstPerson_front.x = cos(glm::radians(frontCamYaw)) * cos(glm::radians(frontCamPitch));
        firstPerson_front.y = sin(glm::radians(frontCamPitch));
        firstPerson_front.z = sin(glm::radians(frontCamYaw)) * cos(glm::radians(frontCamPitch));
        firstPerson_front = glm::normalize(firstPerson_front);

        glm::vec3 cameraPos;

        glm::mat4 viewMatrix;
        if (useFrontCamera) {
            // Position Matrix for the front camera
            cameraPos = glm::vec3(3 + x_mod, 2.8, z_mod); // Adjust the position to be in front of the car
            glm::vec3 cameraCenter = cameraPos + firstPerson_front;

            viewMatrix = glm::lookAt(cameraPos, cameraCenter, worldUp);
        }
        else {
            // Position Matrix for the main camera
            cameraPos = glm::vec3(-15 + x_mod, 10 + y_mod, 0 + z_mod); // eye
            glm::vec3 cameraCenter = cameraPos + thirdPerson_front;

            viewMatrix = glm::lookAt(cameraPos, cameraCenter, worldUp);
        }

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        /////////////////////////////////////////// SKYBOX SHADER ///////////////////////////////////////////
        skyboxShader.use();
        glm::mat4 sky_view = glm::mat4(1.0);
        sky_view = glm::mat4(
            glm::mat3(viewMatrix)
        );
        unsigned int sky_projectionLoc = glGetUniformLocation(skyboxShader.ID, "projection");
        glUniformMatrix4fv(sky_projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        unsigned int sky_viewLoc = glGetUniformLocation(skyboxShader.ID, "view");
        glUniformMatrix4fv(sky_viewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        /////////////////////////////////////////// SKYBOX SHADER ///////////////////////////////////////////
        //translation
        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x_mod, y_mod, z_mod));

        //scale
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scaleVal, scaleVal, scaleVal));

        //rotation
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaX), glm::vec3(1, 0, 0)); 
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaY), glm::vec3(0, 1, 0)); 
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaZ), glm::vec3(0, 0, 1)); 

        ////////////////////////////////////////// NO NORMAL SHADER /////////////////////////////////////////////
        noNormalShader.use();
        directionalLight.SetColor(lightColor);
        directionalLight.UpdateLightProperties(noNormalShader.ID);
        unsigned int noNormalviewLoc = glGetUniformLocation(noNormalShader.ID, "view");
        glUniformMatrix4fv(noNormalviewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int noNormalprojLoc = glGetUniformLocation(noNormalShader.ID, "projection");
        glUniformMatrix4fv(noNormalprojLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        GLuint noNormalambientStrAddress = glGetUniformLocation(noNormalShader.ID, "ambientStr");
        glUniform1f(noNormalambientStrAddress, ambientStr);

        GLuint noNormalambientColorAddress = glGetUniformLocation(noNormalShader.ID, "ambientColor");
        glUniform3fv(noNormalambientColorAddress, 1, glm::value_ptr(ambientColor));

        GLuint noNormalcameraPosAddress = glGetUniformLocation(noNormalShader.ID, "cameraPos");
        glUniform3fv(noNormalcameraPosAddress, 1, glm::value_ptr(cameraPos));

        GLuint noNormalspecStrAddress = glGetUniformLocation(noNormalShader.ID, "specStr");
        glUniform1f(noNormalspecStrAddress, specStr);

        GLuint noNormalspecPhongAddress = glGetUniformLocation(noNormalShader.ID, "specPhong");
        glUniform1f(noNormalspecPhongAddress, specPhong);

        // KART MODEL
        glActiveTexture(GL_TEXTURE0);
        GLuint tex0Address1 = glGetUniformLocation(mainShader.ID, "tex0");
        glBindTexture(GL_TEXTURE_2D, kartModel.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        GLuint tex1Address1 = glGetUniformLocation(mainShader.ID, "norm_tex");
        glBindTexture(GL_TEXTURE_2D, kartModel.normalMap);
        glUniform1i(tex1Address1, 1);

        kartModel.updateTransform(
            0, 180 + thetaY, 0,  // Rotation
            x_mod, 1.7, z_mod,     // Position
            1                 // Scale
        );

        kartModel.draw(noNormalShader.ID);

        ///////////////////////////////////

        ////////// GHOST MODEL 1 //////////
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ghostModel1.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ghostModel1.normalMap);
        glUniform1i(tex1Address1, 1);

        ghostModel1.updateTransform(
            0, 90, 0,  // Rotation
            ghost1x_mod, 0.7, ghost1z_mod,     // Position
            0.5                 // Scale
        );

        // Set the alpha value for transparency
        float alpha = 0.5f; // Adjust this value to control transparency (0.0f to 1.0f)
        GLuint alphaLoc = glGetUniformLocation(mainShader.ID, "alpha");
        glUniform1f(alphaLoc, alpha);

        ghostModel1.draw(noNormalShader.ID);  // Pass the shader program ID

        glDisable(GL_BLEND); // Disable blending after drawing

        ///////////////////////////////////

        ////////// GHOST MODEL 2 //////////
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ghostModel2.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ghostModel2.normalMap);
        glUniform1i(tex1Address1, 1);

        ghostModel2.updateTransform(
            0, 90, 0,  // Rotation
            ghost2x_mod, 0, ghost2z_mod,     // Position
            4                 // Scale
        );

        glUniform1f(alphaLoc, alpha);

        ghostModel2.draw(noNormalShader.ID);  // Pass the shader program ID

        glDisable(GL_BLEND); // Disable blending after drawing

        // ROAD MODEL 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roadModel1.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roadModel1.normalMap);
        glUniform1i(tex1Address1, 1);

        roadModel1.updateTransform(
            90, 0, 0,  // Rotation
            0, 0, 0,     // Position
            roadScaleVal
        );

        // ROAD MODEL 2
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roadModel2.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roadModel2.normalMap);
        glUniform1i(tex1Address1, 1);

        roadModel2.updateTransform(
            90, 0, 0,  // Rotation
            100, 0, 0,     // Position
            roadScaleVal
        );

        // ROAD MODEL 3 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roadModel3.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roadModel3.normalMap);
        glUniform1i(tex1Address1, 1);

        roadModel3.updateTransform(
            90, 0, 0,  // Rotation
            200, 0, 0,     // Position
            roadScaleVal
        );

        // ROAD MODEL 4 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roadModel4.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roadModel4.normalMap);
        glUniform1i(tex1Address1, 1);

        roadModel4.updateTransform(
            90, 0, 0,  // Rotation
            300, 0, 0,     // Position
            roadScaleVal
        );

        // ROAD MODEL 5
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roadModel5.texture);
        glUniform1i(tex0Address1, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, roadModel5.normalMap);
        glUniform1i(tex1Address1, 1);

        roadModel5.updateTransform(
            90, 0, 0,  // Rotation
            400, 0, 0,     // Position
            roadScaleVal
        );

        // Update position point lights
        lightManager.pointLights[0].SetPosition(glm::vec3(x_mod + 10, 1.7f, z_mod - 2));
        lightManager.pointLights[1].SetPosition(glm::vec3(x_mod + 10, 1.7f, z_mod + 2));
        lightManager.pointLights[2].SetPosition(glm::vec3(ghost1x_mod + 12, 1.7f, ghost1z_mod - 1));
        lightManager.pointLights[3].SetPosition(glm::vec3(ghost1x_mod + 12, 1.7f, ghost1z_mod + 1));
        lightManager.pointLights[4].SetPosition(glm::vec3(ghost2x_mod + 10, 1.7f, ghost2z_mod - 2));
        lightManager.pointLights[5].SetPosition(glm::vec3(ghost2x_mod + 10, 1.7f, ghost2z_mod + 2));

        // Update intensity of point lights depending on active skybox
        for (int i = 0; i < 6; i++)
        {
            lightManager.pointLights[i].SetIntensity(pointLightBrightness);
        }

        lightManager.UploadLights(noNormalShader.ID);
        roadModel1.draw(noNormalShader.ID);  // Pass the shader program ID
        roadModel2.draw(noNormalShader.ID);  // Pass the shader program ID
        roadModel3.draw(noNormalShader.ID);  // Pass the shader program ID
        roadModel4.draw(noNormalShader.ID);  // Pass the shader program ID
        roadModel5.draw(noNormalShader.ID);  // Pass the shader program ID

        ///////////////////////////////////// MAIN SHADER ////////////////////////////////////////////////
        mainShader.use();
        directionalLight.UpdateLightProperties(mainShader.ID);
        directionalLight.SetColor(lightColor);
        unsigned int viewLoc = glGetUniformLocation(mainShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int projLoc = glGetUniformLocation(mainShader.ID, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        unsigned int transformLoc = glGetUniformLocation(mainShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

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

        GLuint tex0Address2 = glGetUniformLocation(mainShader.ID, "tex0");
        GLuint tex1Address2 = glGetUniformLocation(mainShader.ID, "norm_tex");

        // LANDMARK MODEL 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, landmark1.texture);
        glUniform1i(tex0Address2, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, landmark1.normalMap);
        glUniform1i(tex1Address2, 1);

        landmark1.updateTransform(
            0, 0, 0,  // Rotation
            410, 25, -20,     // Position
            10                 // Scale
        );

        landmark1.draw(mainShader.ID);  // Pass the shader program ID

        // Ensure the shader program is used before drawing the second landmark
        mainShader.use();

        // LANDMARK MODEL 2
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, landmark2.texture);
        glUniform1i(tex0Address2, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, landmark2.normalMap);
        glUniform1i(tex1Address2, 1);

        landmark2.updateTransform(
            0, 270, 0,  // Rotation
            410, 0, 20,     // Position
            2.5                 // Scale
        );

        landmark2.draw(mainShader.ID);  // Pass the shader program ID

        if (x_mod >= 380.f && playerFinished == false) {
            playerFinished = true;
            // Record player's lap time.
            std::cout << "Player has crossed the finish line!" << std::endl;
            if (playerTimeRecorded == false) {

                playerLapTime = elapsedTime - 5;
                playerTimeRecorded = true;
            }
        }

        if (ghost1x_mod < 380.f && countdownActive == false && ghost1Finished == false) {
            // Ghost keeps moving while player hasn't press spacebar
            if (!ghostPaused) {
                ghost1x_mod += ghost1Speed;
            }
        }
        // Records ghost's lap time
        else if (ghost1x_mod >= 380.f && ghost1Finished == false) {
            ghost1Finished = true;
            std::cout << "Ghost 1 has crossed the finish line!" << std::endl;
            if (ghost1TimeRecorded == false) {

                ghost1LapTime = elapsedTime - 5;
                ghost1TimeRecorded = true;
            }
        }


        if (ghost2x_mod < 380.f && countdownActive == false && ghost2Finished == false) {
            // Ghost keeps moving while player hasn't press spacebar
            if (!ghostPaused) {
                ghost2x_mod += ghost2Speed;
            }
        }
        // Records ghost's lap time
        else if (ghost2x_mod >= 380.f && ghost2Finished == false) {
            ghost2Finished = true;
            std::cout << "Ghost 2 has crossed the finish line!" << std::endl;
            if (ghost2TimeRecorded == false) {

                ghost2LapTime = elapsedTime - 5;
                ghost2TimeRecorded = true;
            }
        }

        // The race is finished when all karts reach the finish line
        if (playerFinished && ghost1Finished && ghost2Finished) {
            if (raceFinished != true) {
                std::cout << "[][][][][][] GDGRAP1 GRAND PRIX HAS CONCLUDED [][][][][][]" << std::endl;
                std::cout << "Player Lap Time : " << playerLapTime << std::endl;
                std::cout << "Ghost 1 Lap Time: " << ghost1LapTime << std::endl;
                std::cout << "Ghost 2 Lap Time: " << ghost2LapTime << std::endl;
                std::cout << "[][][][][][][][][][][][][][][][][][][][][][][][][][][][][]" << std::endl;
                raceFinished = true;
            }
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
};