#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>
#include <sstream>

#include <utils.h>
#include <ProceduralMaterial.h>


/**
 * Constructeur
 */
ProceduralMaterial::ProceduralMaterial() : Material("ProceduralMaterial")
{
    // compiler le shader
    compileShader();
}


/**
 * retourne le source du Vertex Shader
 */
std::string ProceduralMaterial::getVertexShader()
{
    const char* srcVertexShader =
        "#version 300 es\n"
        "in vec3 glVertex;\n"
        "in vec2 glTexCoord;\n"
        "out vec2 frgTexCoord;\n"
        "uniform mat4 mat4ModelView;\n"
        "uniform mat4 mat4Projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mat4Projection * mat4ModelView * vec4(glVertex, 1.0);\n"
        "    frgTexCoord = glTexCoord;\n"
        "}";
    return srcVertexShader;
}


/**
 * retourne le source du Fragment Shader
 */
std::string ProceduralMaterial::getFragmentShader()
{
    const char* srcFragmentShader =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 frgTexCoord;\n"
        "out vec4 glFragColor;\n"
        "void main()\n"
        "{\n"
        "    // la distance du fragment au point (0.5, 0.5)\n"
        "    float d = min(distance(frgTexCoord.st, vec2(0.5, 0.5)), 1.0);\n"
        "    // couleurs fonction de la distance\n"
        "    float red   = 1.0 - d;\n"
        "    float green = 0.5 - d;\n"
        "    float blue  = d;\n"
        "    glFragColor = vec4(red, green, blue, 1.0);\n"
        "}";
    return srcFragmentShader;
}


/**
 * crée et retourne un VBOset pour ce matériau, afin qu'il soit rempli par un maillage
 * @return le VBOset du matériau
 */
VBOset* ProceduralMaterial::createVBOset()
{
    // créer le VBOset et spécifier les noms des attribute nécessaires à ce matériau
    VBOset* vboset = Material::createVBOset();
    vboset->addAttribute(MeshVertex::ID_ATTR_TEXCOORD, Utils::VEC2, "glTexCoord");
    return vboset;
}


/**
 * Cette méthode supprime les ressources allouées
 */
ProceduralMaterial::~ProceduralMaterial()
{
}
