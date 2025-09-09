#include <windows.h>
#include <GL/gl.h>
#include "glut.h"
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include "GL/glext.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <chrono>
#include <iomanip>

#pragma warning(disable : 4996) //to avoid sending some warnings
using namespace std;

int screenNumber = 0;  //variable to track the current screen

vector<glm::vec3> verticesCar, verticesTrack, verticesFinishLine;
vector<glm::vec3> normalsCar, normalsTrack, normalsFinishLine;
vector<glm::vec2> uvsCar, uvsTrack, uvsFinishLine;
vector<string> mtlNamesCar, mtlNamesTrack, mtlNamesFinishLine;
vector<glm::vec3> KdCar, KdTrack, KdFinishLine, rgbValuesCar, rgbValuesTrack, rgbValuesFinishLine;

float cameraRotationX;
float cameraPositionX, cameraPositionY, cameraPositionZ;
float carRotationX, carRotationY, carRotationZ;
float carPositionX, carPositionY, carPositionZ;
float scaleFactorCar;
float trackRotationX;
float trackPositionX, trackPositionZ;
float scaleFactorTrack;
float turnSpeed = 5.0f;
float carSpeed = 0.05f;
float carRotationAngle = 0.0f;
float acceleration;

bool wireframeMode = false;
bool isTiming = false;
bool wPressed = false;
bool sPressed = false;

glm::vec3 carDirection = glm::vec3(0.0f, 0.0f, 1.0f);

chrono::steady_clock::time_point lapStartTime;

string currentTime;

PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC	glUseProgram = NULL;
PFNGLCOMPILESHADERPROC	glCompileShader = NULL;
PFNGLGETUNIFORMLOCATIONPROC  glGetUniformLocation = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;

PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;

PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;

GLuint carVAO, carVBO, trackVAO, trackVBO, carColorVBO, trackColorVBO, trackNormalsVBO, carNormalsVBO;
GLuint finishLineVAO, finishLineVBO, finishLineColorVBO, finishLineNormalsVBO;
GLuint shaderProgram;
GLint modelLoc, viewLoc, projectionLoc;
GLuint vertexShader, fragmentShader;


//configuration structure
struct Config 
{

    string chosenCar;
    string bestTime;

};

//configuration variable and path to the car model
Config config;
string carPath;


void extensionSetup()
{

    const char* version = (char*)glGetString(GL_VERSION);
    printf("%s", version);
    if ((version[0] < '1') || ((version[0] == '1') && (version[2] < '5')) || (version[1] != '.')) 
    {

        printf("Incorrect version of OpenGL\n");
        exit(0);

    }

    else 
    {

        glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
        glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
        glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
        glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
        glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
        glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");

        glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");

        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");

        glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");

    }

}


char* readShader(const char* aShaderFile)
{

    FILE* filePointer = fopen(aShaderFile, "rb");
    char* content = NULL;
    long numVal = 0;

    fseek(filePointer, 0L, SEEK_END);
    numVal = ftell(filePointer);
    fseek(filePointer, 0L, SEEK_SET);
    content = (char*)malloc((numVal + 1) * sizeof(char));
    fread(content, 1, numVal, filePointer);
    content[numVal] = '\0';
    fclose(filePointer);

    return content;

}


void setShaders(const char* vertexShaderFile, const char* fragmentShaderFile)
{

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    //compile vertex shader
    const char* vertexShaderSource = readShader(vertexShaderFile);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    //compile fragment shader
    const char* fragmentShaderSource = readShader(fragmentShaderFile);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //create shaders program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    //set matrix uniform
    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");

}


void setupBuffers()
{

    glGenVertexArrays(1, &carVAO);
    glBindVertexArray(carVAO);

    glGenBuffers(1, &carVBO);
    glBindBuffer(GL_ARRAY_BUFFER, carVBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCar.size() * sizeof(glm::vec3), &verticesCar[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &carColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, carColorVBO);
    glBufferData(GL_ARRAY_BUFFER, rgbValuesCar.size() * sizeof(glm::vec3), &rgbValuesCar[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &carNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, carNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, normalsCar.size() * sizeof(glm::vec3), &normalsCar[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &trackVAO);
    glBindVertexArray(trackVAO);

    glGenBuffers(1, &trackVBO);
    glBindBuffer(GL_ARRAY_BUFFER, trackVBO);
    glBufferData(GL_ARRAY_BUFFER, verticesTrack.size() * sizeof(glm::vec3), &verticesTrack[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &trackColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, trackColorVBO);
    glBufferData(GL_ARRAY_BUFFER, rgbValuesTrack.size() * sizeof(glm::vec3), &rgbValuesTrack[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &trackNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, trackNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, normalsTrack.size() * sizeof(glm::vec3), &normalsTrack[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &finishLineVAO);
    glBindVertexArray(finishLineVAO);

    glGenBuffers(1, &finishLineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, finishLineVBO);
    glBufferData(GL_ARRAY_BUFFER, verticesFinishLine.size() * sizeof(glm::vec3), &verticesFinishLine[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &finishLineColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, finishLineColorVBO);
    glBufferData(GL_ARRAY_BUFFER, rgbValuesFinishLine.size() * sizeof(glm::vec3), &rgbValuesFinishLine[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &finishLineNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, finishLineNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, normalsFinishLine.size() * sizeof(glm::vec3), &normalsFinishLine[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void cleanupBuffers()
{

    glDeleteBuffers(1, &carVBO);
    glDeleteVertexArrays(1, &carVAO);
    glDeleteBuffers(1, &carColorVBO);
    glDeleteBuffers(1, &carNormalsVBO);

    glDeleteBuffers(1, &trackVBO);
    glDeleteVertexArrays(1, &trackVAO);
    glDeleteBuffers(1, &trackColorVBO);
    glDeleteBuffers(1, &trackNormalsVBO);

    glDeleteBuffers(1, &finishLineVBO);
    glDeleteVertexArrays(1, &finishLineVAO);
    glDeleteBuffers(1, &finishLineColorVBO);
    glDeleteBuffers(1, &finishLineNormalsVBO);

}

//loading materials from MTL file
bool loadMTL(const char* mtlFilePath, vector<string>& out_mtlNames, vector<glm::vec3>& out_rgb) 
{

    FILE* file = fopen(mtlFilePath, "r");
    if (file == NULL)
    {

        printf("%s\n", mtlFilePath);
        printf("Impossible to open the file !\n");
        return false;

    }

    while (1)
    {

        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        if (strcmp(lineHeader, "newmtl") == 0)
        {
        
            char mtlName[128];
            fscanf(file, "%s\n", &mtlName);
            out_mtlNames.push_back(string(mtlName));

        }

        else if (strcmp(lineHeader, "Kd") == 0)
        {
            
            glm::vec3 rgb_value;
            string strtest;
            fscanf(file, "%f %f %f\n", &rgb_value.x, &rgb_value.y, &rgb_value.z);
            out_rgb.push_back(rgb_value);

        }

        else
        {

            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        
        }

    }

    fclose(file);

    return true;

}


//read obj file
bool loadOBJ(const char* objFilePath, vector<glm::vec3>& out_vertices, vector<glm::vec2>& out_uvs, vector<glm::vec3>& out_normals, 
             vector<string> mtlNames, vector<glm::vec3> rgbMaterials, vector<glm::vec3>& out_rgb) 
{

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    glm::vec3 temp_rgb;

    FILE* file = fopen(objFilePath, "r");

    //error occured during opening a file
    if (file == NULL) 
    {

        printf(objFilePath);
        printf("Impossible to open the file !\n");
        return false;

    }

    while (1) 
    {

        char lineHeader[128];
        //read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; //EOF = End Of File. Quit the loop.

        if (strcmp(lineHeader, "v") == 0) 
        {

            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);

        }

        else if (strcmp(lineHeader, "vt") == 0) 
        {

            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y; //invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);

        }

        else if (strcmp(lineHeader, "vn") == 0) 
        {

            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);

        }

        else if (strcmp(lineHeader, "usemtl") == 0)
        {

            char mtlName[128];
            fscanf(file, "%s\n", &mtlName);
            string materialName(mtlName);

            int currentMtlIndex = -1;
            for (int i = 0; i < mtlNames.size(); i++)
                if (mtlNames[i] == materialName) 
                {
                    currentMtlIndex = i;
                    break;
                }

            if (currentMtlIndex >= 0)
                temp_rgb = rgbMaterials[currentMtlIndex];

        }

        else if (strcmp(lineHeader, "f") == 0) 
        {

            string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            
            if (matches != 9) 
            {

                printf("File can't be read by our simple parser :-Try exporting with other options\n");
                return false;

            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            out_rgb.push_back(temp_rgb);
            out_rgb.push_back(temp_rgb);
            out_rgb.push_back(temp_rgb);

        }

        else 
        {

            //Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);

        }

    }

    //For each vertex of each triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++) 
    {

        //Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        //Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];
        glm::vec2 uv = temp_uvs[uvIndex - 1];
        glm::vec3 normal = temp_normals[normalIndex - 1];

        //Put the attributes in buffers
        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);

    }

    return true;

}


//save configuration file
void saveConfig(const Config& config) 
{

    ofstream configFile("config.txt");

    if (configFile.is_open()) 
    {

        configFile << "ChosenCar: " << config.chosenCar << endl;
        configFile << "BestTime: " << config.bestTime << endl;
        configFile.close();

    }

}


//load configuration file
Config loadConfig() 
{

    Config config;
    ifstream configFile("config.txt");
    
    if (configFile.is_open()) 
    {
    
        string line;
        while (getline(configFile, line)) 
        {
        
            istringstream iss(line);
            string key, value;
            if (iss >> key >> value) 
            {

                if (key == "ChosenCar:")
                    config.chosenCar = value;

                else if (key == "BestTime:")
                    config.bestTime = value;
                
            }

        }

        configFile.close();
    }

    return config;

}


//draw a text on the screen
void drawText(float x, float y, const string& text)
{

    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);

}


void resetParameters() 
{

    isTiming = false;
    currentTime = "00:00:00.000";

    carDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    carRotationAngle = 0.0f;
    acceleration = 0.0f;

    cameraRotationX = -20;
    cameraPositionX = 0;
    cameraPositionY = 0.8;
    cameraPositionZ = -1;

    carRotationX = 0;
    carRotationY = 0;
    carRotationZ = 0;
    carPositionX = 0;
    carPositionY = 0;
    carPositionZ = 0;
    scaleFactorCar = 0.03;
    
    trackRotationX = -0.1;
    trackPositionX = -0.2;
    trackPositionZ = 5;
    scaleFactorTrack = 0.08;

    verticesCar.clear();
    verticesTrack.clear();
    verticesFinishLine.clear();

    normalsCar.clear();
    normalsTrack.clear();
    normalsFinishLine.clear();

    uvsCar.clear();
    uvsTrack.clear();
    uvsFinishLine.clear();

    KdCar.clear();
    KdTrack.clear();
    KdFinishLine.clear();
    rgbValuesCar.clear();
    rgbValuesTrack.clear();
    rgbValuesFinishLine.clear();

    mtlNamesCar.clear();
    mtlNamesTrack.clear();
    mtlNamesFinishLine.clear();

}


//display menu
void displayStartScreen(void)
{

    resetParameters();
    cleanupBuffers();
    glUseProgram(0);
    glClearColor(0, 156.0 / 255, 215.0 / 255, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //Start Game button
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.5, 0.2);
    glVertex2f(0.5, 0.2);
    glVertex2f(0.5, 0.4);
    glVertex2f(-0.5, 0.4);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, 0.28, "Start Game");

    //Configuration button
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-0.5, -0.1);
    glVertex2f(0.5, -0.1);
    glVertex2f(0.5, 0.1);
    glVertex2f(-0.5, 0.1);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, -0.02, "Configuration");
    
    //Exit button
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-0.5, -0.4);
    glVertex2f(0.5, -0.4);
    glVertex2f(0.5, -0.2);
    glVertex2f(-0.5, -0.2);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, -0.32, "Exit");

    glFinish();

}


void displayGameScreen(void)
{

    //split screen into 2 parts - grass and sky
    glScissor(0, int(0.6 * glutGet(GLUT_WINDOW_HEIGHT)), glutGet(GLUT_WINDOW_WIDTH), int(0.4 * glutGet(GLUT_WINDOW_HEIGHT)));
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.01f, 0.0f, 0.3, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glScissor(0, 0, glutGet(GLUT_WINDOW_WIDTH), int(0.6 * glutGet(GLUT_WINDOW_HEIGHT)));
    glEnable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.2f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    //display text
    glUseProgram(0);
    string speedText = to_string(static_cast<int>(abs(acceleration) * 50)) + " km/h";
    glColor3f(1.0, 1.0, 1.0);
    drawText(0.45, 0.88, speedText.c_str());

    glColor3f(1.0, 1.0, 1.0);
    drawText(0.45, 0.78, ("Best Lap Time:      " + config.bestTime).c_str());

    glColor3f(1.0, 1.0, 1.0);
    drawText(0.45, 0.68, ("Current Lap Time: " + currentTime).c_str());
    glUseProgram(shaderProgram);

    //check if wireframe mode is enabled
    if (wireframeMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);

    //set projection and view matrix
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)glutGet(GLUT_WINDOW_WIDTH) / float(glutGet(GLUT_WINDOW_HEIGHT)), 0.1f, 100.0f);

    //constant distance between car and camera
    float distance = 1.0f;

    glm::vec3 newCameraPosition = glm::vec3(carPositionX - distance * sin(glm::radians(carRotationY)), 
                                            carPositionY + distance / 2.0f, carPositionZ - distance * cos(glm::radians(carRotationY)));
    glm::vec3 lookAtPoint = glm::vec3(carPositionX, carPositionY, carPositionZ);
    glm::mat4 view = glm::lookAt(newCameraPosition, lookAtPoint, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 cameraRotation = glm::rotate(glm::mat4(1.0f), glm::radians(cameraRotationX), glm::vec3(1.0f, 0.0f, 0.0f))
        * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f))
        * glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    view = cameraRotation * view;

    //track rendering
    glBindVertexArray(trackVAO);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 trackModel = glm::mat4(1.0f);
    trackModel = glm::translate(trackModel, glm::vec3(trackPositionX, 0, trackPositionZ));
    trackModel = glm::scale(trackModel, glm::vec3(scaleFactorTrack));
    trackModel = glm::rotate(trackModel, glm::radians(trackRotationX), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(trackModel));
    glDrawArrays(GL_TRIANGLES, 0, verticesTrack.size());

    //finish line rendering
    glBindVertexArray(finishLineVAO);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 finishLineModel = glm::mat4(1.0f);
    finishLineModel = glm::translate(finishLineModel, glm::vec3(trackPositionX-0.9, 0.62, trackPositionZ));
    finishLineModel = glm::scale(finishLineModel, glm::vec3(scaleFactorTrack/2));
    finishLineModel = glm::rotate(finishLineModel, glm::radians(trackRotationX), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finishLineModel));
    glDrawArrays(GL_TRIANGLES, 0, verticesFinishLine.size());

    //car rendering
    glBindVertexArray(carVAO);

    glm::mat4 carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, glm::vec3(carPositionX, carPositionY, carPositionZ));
    carModel = glm::scale(carModel, glm::vec3(scaleFactorCar));
    carModel = glm::rotate(carModel, glm::radians(carRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    carModel = glm::rotate(carModel, glm::radians(carRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    carModel = glm::rotate(carModel, glm::radians(carRotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));
    glDrawArrays(GL_TRIANGLES, 0, verticesCar.size());

    glDisable(GL_DEPTH_TEST);
    glFlush();

}


void keyboard(unsigned char key, int x, int y)
{

    if (screenNumber != 1)
        return;

    switch (key) 
    {
        
        //move forward
        case 'w':
            
            wPressed = true;
            break;

        //rotate left
        case 'a':
            if (acceleration != 0.0f) carRotationAngle += turnSpeed;
            break;
    
        //move backward
        case 's':

            sPressed = true;
            break;
        
        //rotate right
        case 'd':
            if (acceleration != 0.0f) carRotationAngle -= turnSpeed;
            break;

        //quit the game
        case 'q':

            screenNumber = 0;
            wireframeMode = false;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(shaderProgram);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        //change display mode
        case '-':

            wireframeMode = !wireframeMode;
            break;

    }

    glutPostRedisplay();

}


void keyboardUp(unsigned char key, int x, int y) 
{

    switch (key) 
    {

        case 'w':
            wPressed = false;
            break;
    
        case 's':
            sPressed = false;
            break;

        case 'a':
        case 'd':
            carRotationAngle = carRotationY;
            break;
    
    }

}


//convert time to string
string convertToTimeString(chrono::steady_clock::duration duration)
{

    auto hours = chrono::duration_cast<chrono::hours>(duration);
    duration -= hours;
    auto minutes = chrono::duration_cast<chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = chrono::duration_cast<chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration);

    stringstream ss;
    ss << setw(2) << setfill('0') << hours.count() << ":"
        << setw(2) << setfill('0') << minutes.count() << ":"
        << setw(2) << setfill('0') << seconds.count() << "."
        << setw(3) << setfill('0') << milliseconds.count();

    return ss.str();

}


//convert string to time
chrono::steady_clock::duration convertToTime(const string& timeString)
{

    istringstream iss(timeString);
    char discard;
    int hours, minutes, seconds, milliseconds;
    iss >> hours >> discard >> minutes >> discard >> seconds >> discard >> milliseconds;

    return chrono::hours(hours) + chrono::minutes(minutes) + chrono::seconds(seconds) + chrono::milliseconds(milliseconds);

}


//start or end each lap
void startOrEndLap()
{

    if (isTiming)
    {

        auto lapEndTime = chrono::steady_clock::now();
        auto lapTime = lapEndTime - lapStartTime;

        auto bestTimeDuration = convertToTime(config.bestTime);

        if (lapTime < bestTimeDuration || config.bestTime.empty())
        {

            config.bestTime = convertToTimeString(lapTime);
            saveConfig(config);

        }

    }

    else
        isTiming = !isTiming;

    lapStartTime = chrono::steady_clock::now();

}


void updateMovement() 
{

    glm::vec3 previousCarPosition = glm::vec3(carPositionX, carPositionY, carPositionZ);

    if (wPressed) 
    {

        acceleration += 0.01f;
        if (acceleration > 2.0f) acceleration = 2.0f;
        else if (acceleration < 0.0f) acceleration = 0.0f;

    }

    else if (sPressed && acceleration > 0) 
    {

        acceleration -= 0.02f;
        if (acceleration < 0.0f) acceleration = 0.0f;

    }

    else if (sPressed && acceleration <= 0) 
    {

        acceleration -= 0.1f;
        if (acceleration < -0.3f) acceleration = -0.3f;

    }

    else 
    {

        if (acceleration > 0) 
        {

            acceleration -= 0.005f;
            if (acceleration < 0.0f) acceleration = 0.0f;
        
        }

        else if (acceleration < 0) 
        {

            acceleration += 0.005f;
            if (acceleration > 0.0f) acceleration = 0.0f;
        
        }

    }

    carRotationY = carRotationAngle;
    carDirection.x = cos(glm::radians(90 - carRotationAngle));
    carDirection.z = sin(glm::radians(90 - carRotationAngle));

    carPositionX += carSpeed * carDirection.x * acceleration;
    carPositionY += carSpeed * carDirection.y * acceleration;
    carPositionZ += carSpeed * carDirection.z * acceleration;

    if (previousCarPosition.z <= 5.30 && carPositionZ > 5.30)
        if (carPositionX >= -0.78 && carPositionX <= 1.058)
            startOrEndLap();
    
    glutPostRedisplay();

}


//display configuration screen
void displayConfigurationScreen(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(1.0, 0.6, 0.0);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5, 0.7);
    glVertex2f(-0.5, 0.7);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, 0.58, "Choose a car!");

    //Red Car button
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.5, 0.2);
    glVertex2f(0.5, 0.2);
    glVertex2f(0.5, 0.4);
    glVertex2f(-0.5, 0.4);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, 0.28, "Red");

    //Blue Car button
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(-0.5, -0.1);
    glVertex2f(0.5, -0.1);
    glVertex2f(0.5, 0.1);
    glVertex2f(-0.5, 0.1);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, -0.02, "Blue");

    //Gray Car button
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5);
    glVertex2f(-0.5, -0.4);
    glVertex2f(0.5, -0.4);
    glVertex2f(0.5, -0.2);
    glVertex2f(-0.5, -0.2);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, -0.32, "Gray");

    //back to the menu
    glBegin(GL_QUADS);
    glColor3f(0.0, 1.0, 0.1);
    glVertex2f(-0.5, -0.7);
    glVertex2f(0.5, -0.7);
    glVertex2f(0.5, -0.5);
    glVertex2f(-0.5, -0.5);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    drawText(-0.45, -0.62, "Back to the menu!");
    glFinish();

}


//choose appropriate screen
void display(void)
{

    switch (screenNumber)
    {

        case 0:
            displayStartScreen();
            break;

        case 1:
            displayGameScreen();
            break;

        case 2:
            displayConfigurationScreen();
            break;

    }

    glFlush();

}


//mouse click check
void mouse(int button, int state, int x, int y)
{

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {

        //convert mouse coordinates to OpenGL coordinates
        float mouseX = (2.0 * x) / glutGet(GLUT_WINDOW_WIDTH) - 1.0;
        float mouseY = 1.0 - (2.0 * y) / glutGet(GLUT_WINDOW_HEIGHT);

        //check which button is clicked (Menu screen)
        if (screenNumber == 0)
        {

            //Start Game button
            if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= 0.2 && mouseY <= 0.4) 
            {

                screenNumber = 1;
                carPath = "Modele_autek\\" + config.chosenCar + ".mtl";
                loadMTL(carPath.c_str(), mtlNamesCar, KdCar);
                carPath = "Modele_autek\\" + config.chosenCar + ".obj";
                loadOBJ(carPath.c_str(), verticesCar, uvsCar, normalsCar, mtlNamesCar, KdCar, rgbValuesCar);

                loadMTL("Tor\\Bez_barierek_plaski_triangulated.mtl", mtlNamesTrack, KdTrack);
                loadOBJ("Tor\\Bez_barierek_plaski_triangulated.obj", verticesTrack, uvsTrack, normalsTrack, mtlNamesTrack, KdTrack, rgbValuesTrack);

                for (int i = 0; i < rgbValuesTrack.size(); i++) 
                {
                 
                    rgbValuesTrack[i].x = 0.15f;
                    rgbValuesTrack[i].y = 0.15f;
                    rgbValuesTrack[i].z = 0.15f;

                }

                loadMTL("Tor\\finishLine.mtl", mtlNamesFinishLine, KdFinishLine);
                loadOBJ("Tor\\finishLine.obj", verticesFinishLine, uvsFinishLine, normalsFinishLine, mtlNamesFinishLine, KdFinishLine, rgbValuesFinishLine);

                setupBuffers();
                setShaders("vert_shader.vs", "fragm_shader.fs");

            }

            //Configuration button
            else if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= -0.1 && mouseY <= 0.1)
                screenNumber = 2;  

            //Exit button
            else if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= -0.4 && mouseY <= -0.2)
                exit(0);  

        }

        //check which button is clicked (Configuration screen)
        else if (screenNumber == 2) 
        {

            bool saveConfigFile = false;

            if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= 0.2 && mouseY <= 0.4) 
            {

                config.chosenCar = "czerwone";
                saveConfigFile = true;

            }

            else if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= -0.1 && mouseY <= 0.1) 
            {

                config.chosenCar = "niebieskie";
                saveConfigFile = true;

            }

            else if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= -0.4 && mouseY <= -0.2) 
            {

                config.chosenCar = "szare";
                saveConfigFile = true;

            }

            else if (mouseX >= -0.5 && mouseX <= 0.5 && mouseY >= -0.7 && mouseY <= -0.5)
                screenNumber = 0;


            //if there are any changes in configuration file, it's saved
            if (saveConfigFile) 
            {

                saveConfig(config);
                screenNumber = 0;

            }

        }

    }

    glutPostRedisplay();

}


void timer(int value)
{

    updateMovement();
    glutPostRedisplay();

    if (isTiming)
    {

        auto currentTimePoint = chrono::steady_clock::now();
        auto elapsedTime = currentTimePoint - lapStartTime;
        currentTime = convertToTimeString(elapsedTime);

    }

    // (16 ms is ~60 FPS)
    glutTimerFunc(16, timer, 0);

}


//main function
int main(int argc, CHAR* argv[])
{
    
    glutInitWindowSize(1200, 750); 
    glutInitWindowPosition(100, 50);
    glutInitDisplayMode(GLUT_RGB);
    glutCreateWindow("Car Racing");
    config = loadConfig();
    extensionSetup();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(16, timer, 0);  // set timer
    glutMainLoop();
    return 0;

}