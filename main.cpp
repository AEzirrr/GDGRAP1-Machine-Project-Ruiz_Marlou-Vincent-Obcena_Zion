#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <cmath>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

glm::mat4 identity_matrix(1.0);

bool isDaySkybox = true;


bool isPressedD, isPressedA, isPressedW, isPressedS, 
isPressedQ, isPressedE, isPressedZ, isPressedX,
isPressedF, isPressedH, isPressedT, isPressedG, isPressedC, isPressedV,
isPressedLEFT, isPressedRIGHT, isPressedUP, isPressedDOWN;






// Camera class
class MyCamera {
protected:
    glm::vec3 front; // Front dir
    glm::vec3 up; // Up dir
    float yaw; // Yaw angle
    float pitch; // Pitch angle
    float distance; // Distance from the target

public:
    glm::vec3 position;

    // Constructor
    MyCamera(glm::vec3 pos, glm::vec3 frontDir, glm::vec3 upDir, float y, float p, float dist)
        : position(pos), front(frontDir), up(upDir), yaw(y), pitch(p), distance(dist) {
        UpdateCameraVectors();
    }

    virtual glm::mat4 GetProjectionMatrix(float aspect) const = 0;

    // Function to get the view matrix
    glm::mat4 GetViewMatrix(const glm::vec3& target) const {
        return glm::lookAt(position, target, up);
    }

    // Function to process mouse movement
    void ProcessMouseMovement(float xoffset, float yoffset, float sensitivity = 0.1f) {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        UpdateCameraVectors();
    }

    // Function to update the camera position
    void UpdateCameraPosition(const glm::vec3& target) {
        position.x = target.x + distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        position.y = target.y + distance * sin(glm::radians(pitch));
        position.z = target.z + distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    }

private:
    // Function to update the camera vectors
    void UpdateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
    }
};

// Perspective camera class
class PerspectiveCamera : public MyCamera {
private:
    float fov;

public:
    // Constructor
    PerspectiveCamera(glm::vec3 pos, glm::vec3 frontDir, glm::vec3 upDir, float y, float p, float dist, float fovAngle)
        : MyCamera(pos, frontDir, upDir, y, p, dist), fov(fovAngle) {
    }

    // Function to get the projection matrix
    glm::mat4 GetProjectionMatrix(float aspect) const override {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
    }
};

// Orthographic camera class
class OrthographicCamera : public MyCamera {
private:
    float orthoSize;

public:
    // Constructor
    OrthographicCamera(glm::vec3 pos, glm::vec3 frontDir, glm::vec3 upDir, float y, float p, float dist, float size)
        : MyCamera(pos, frontDir, upDir, y, p, dist), orthoSize(size) {
    }

    // Function to get the projection matrix
    glm::mat4 GetProjectionMatrix(float aspect) const override {
        return glm::ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, -50.0f, 50.0f);
    }
};


















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


    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    stbi_set_flip_vertically_on_load(true);
    int img_width, img_height, color_channels;
    //3 == RGB: JPGS no transparency
    //4 == RGBA PNGS with transparency 

    unsigned char* tex_bytes = stbi_load(
        "3D/brickwall.jpg",
        &img_width,
        &img_height,
        &color_channels,
        0
    );


    //          MinX MinY Width Height
    //glViewport(320, 0, 640/2, 480);

    glfwSetKeyCallback(window, Key_Callback);


    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    //Add the file stream to the string stream
    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();
    ///////////////////////////////////////////
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    //Add the file stream to the string stream
    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();


    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);


    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);


    GLuint shaderProg = glCreateProgram();

    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);


    glLinkProgram(shaderProg);

    /////////////////////////////SKYBOX SHADER///////////////////////////////////////
    std::fstream sky_vertSrc("Shaders/skybox.vert");
    std::stringstream sky_vertBuff;
    sky_vertBuff << sky_vertSrc.rdbuf();

    std::string sky_vertS = sky_vertBuff.str();
    const char* sky_v = sky_vertS.c_str();

    std::fstream sky_fragSrc("Shaders/skybox.frag");
    std::stringstream sky_fragBuff;
    sky_fragBuff << sky_fragSrc.rdbuf();
    std::string sky_fragS = sky_fragBuff.str();
    const char* sky_f = sky_fragS.c_str();

    GLuint sky_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sky_vertexShader, 1, &sky_v, NULL);
    glCompileShader(sky_vertexShader);

    GLuint sky_fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sky_fragShader, 1, &sky_f, NULL);
    glCompileShader(sky_fragShader);

    GLuint skyboxShaderProg = glCreateProgram();
    glAttachShader(skyboxShaderProg, sky_vertexShader);
    glAttachShader(skyboxShaderProg, sky_fragShader);

    glLinkProgram(skyboxShaderProg);
    ////////////////////////////////////////////////////////////////////////////////

    /*
      7--------6
     /|       /|
    4--------5 |
    | |      | |
    | 3------|-2
    |/       |/
    0--------1
    */
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

    std::string path = "3D/plane.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(shapes[0].mesh.indices[i].vertex_index);
    }


	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {

		tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
        tinyobj::index_t vData2 = shapes[0].mesh.indices[i+1];
        tinyobj::index_t vData3 = shapes[0].mesh.indices[i+2];

		glm::vec3 v1 = glm::vec3(
			attributes.vertices[vData1.vertex_index * 3],
			attributes.vertices[vData1.vertex_index * 3 + 1],
			attributes.vertices[vData1.vertex_index * 3 + 2]
		);

        glm::vec3 v2 = glm::vec3(
            attributes.vertices[vData2.vertex_index * 3],
            attributes.vertices[vData2.vertex_index * 3 + 1],
            attributes.vertices[vData2.vertex_index * 3 + 2]
        );

        glm::vec3 v3 = glm::vec3(
            attributes.vertices[vData3.vertex_index * 3],
            attributes.vertices[vData3.vertex_index * 3 + 1],
            attributes.vertices[vData3.vertex_index * 3 + 2]
        );

		glm::vec2 uv1 = glm::vec2(
			attributes.texcoords[vData1.texcoord_index * 2],
			attributes.texcoords[vData1.texcoord_index * 2 + 1]
		);

        glm::vec2 uv2 = glm::vec2(
            attributes.texcoords[vData2.texcoord_index * 2],
            attributes.texcoords[vData2.texcoord_index * 2 + 1]
        );

        glm::vec2 uv3 = glm::vec2(
            attributes.texcoords[vData3.texcoord_index * 2],
            attributes.texcoords[vData3.texcoord_index * 2 + 1]
        );


		glm::vec3 deltaPos1 = v2 - v1;
        glm::vec3 deltaPos2 = v3 - v1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

		float r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
    }

    std::vector<GLfloat> fullVertexData;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]   
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        //UV
        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );

		fullVertexData.push_back(
			tangents[i].x
		);

        fullVertexData.push_back(
			tangents[i].y
        );

        fullVertexData.push_back(
            tangents[i].z
        );

		fullVertexData.push_back(
			bitangents[i].x
		);

        fullVertexData.push_back(
            bitangents[i].y
        );

        fullVertexData.push_back(
            bitangents[i].z
        );
    }


    GLfloat vertices[]{
        //x    y    z
        0.f, 0.5f, 0.f,
        -0.5f, 0.f, 0.f,
        0.5f, 0.f, 0.f
    };

    GLuint indices[]{
        0,1,2
    };


    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    //currTex = texture = tex0

    //tile x
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //tile y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
                 //GL_RGB == 3 channels
        GL_RGB, //4 channels, transparency
        img_width,
        img_height,
        0, //border
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    tex_bytes = stbi_load(
        "3D/brickwall_normal.jpg",
        &img_width,
        &img_height,
        &color_channels,
        0
    );

	GLuint norm_tex;
	glGenTextures(1, &norm_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, norm_tex);
    //tile x
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //tile y
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		//GL_RGB == 3 channels
		GL_RGB, //4 channels, transparency
		img_width,
		img_height,
		0, //border
		GL_RGB,
		GL_UNSIGNED_BYTE,
		tex_bytes
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(tex_bytes);



    // ID of VAO, VBO
    GLuint VAO, VBO;
    //generate VAO and assign the ID to variable
    glGenVertexArrays(1, &VAO);
    //generate VBO and assign the ID to variable
    glGenBuffers(1, &VBO);

    //current VAO = null

    glBindVertexArray(VAO); // current VAO = VAO

    //current VBO = null
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //current VBO = VBO
    // currVAO.VBO.append(VBO)

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * fullVertexData.size(), fullVertexData.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(
        0, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        14 * sizeof(float), 
        (void*)0
    );

    //normal
    GLintptr normalPtr = 3 * sizeof(float);
    glVertexAttribPointer(
        1,                       
        3,                         
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(float),         
        (void*)normalPtr
    );

    GLintptr uvPtr = 6 * sizeof(float);
    glVertexAttribPointer(
        2,                         
        2,                         
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(float),         
        (void*)uvPtr
    );

	GLintptr tangentPtr = 8 * sizeof(float);
	GLintptr bitangentPtr = 11 * sizeof(float);

	glVertexAttribPointer(
		3,
		3,
		GL_FLOAT,
		GL_FALSE,
		14 * sizeof(float),
		(void*)tangentPtr
	);

    glVertexAttribPointer(
        4,
        3,
        GL_FLOAT,
        GL_FALSE,
        14 * sizeof(float),
        (void*)bitangentPtr
    );


    //0 Position
    //1 ??
    //2 UV / Texture data

    glEnableVertexAttribArray(0); //enable attrib index 0
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2); //enable attrib index 2
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    //current VBO = VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0); //current VBO = null

    //current VAO = VAO
    glBindVertexArray(0); //current VAO = null

    float radius = 0.5f;
    float prevAngle = 0;
    float yOffset = 0.54;

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


        glUseProgram(skyboxShaderProg);
        glm::mat4 sky_view = glm::mat4(1.0);
        sky_view = glm::mat4(
            glm::mat3(viewMatrix)
        );

        unsigned int sky_projectionLoc = glGetUniformLocation(skyboxShaderProg, "projection");
        glUniformMatrix4fv(sky_projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        unsigned int sky_viewLoc = glGetUniformLocation(skyboxShaderProg, "view");
        glUniformMatrix4fv(sky_viewLoc, 1, GL_FALSE, glm::value_ptr(sky_view));


        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(shaderProg);

        //translation
        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x_mod, y_mod, z_mod));

        //scale
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scaleVal, scaleVal, scaleVal));

        //rotation
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaX), glm::vec3(1, 0, 0)); 
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaY), glm::vec3(0, 1, 0)); 
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaZ), glm::vec3(0, 0, 1)); 

        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        unsigned int projLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));


        glBindTexture(GL_TEXTURE_2D, texture);

        GLuint lightAddress = glGetUniformLocation(shaderProg, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));

        GLuint lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));

        GLuint brightnessAddress = glGetUniformLocation(shaderProg, "brightness");
        glUniform1f(brightnessAddress, brightness);


        GLuint ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, ambientStr);

        GLuint ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));



        GLuint cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));

        GLuint specStrAddress = glGetUniformLocation(shaderProg, "specStr");
        glUniform1f(specStrAddress, specStr);

        GLuint specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
        glUniform1f(specPhongAddress, specPhong);

		glActiveTexture(GL_TEXTURE0);
        GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(tex0Address, 0);

        glActiveTexture(GL_TEXTURE1);
        GLuint tex1Address = glGetUniformLocation(shaderProg, "norm_tex");
        glBindTexture(GL_TEXTURE_2D, norm_tex);
        glUniform1i(tex1Address, 1);

        glUseProgram(shaderProg);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 14);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
};