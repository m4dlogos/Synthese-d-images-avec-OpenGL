// Définition de la classe Rectangle2


#include <GL/glew.h>
#include <GL/gl.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include <utils.h>
#include <MeshModuleDrawing.h>

#include <Rectangle2.h>



/**
 * Constructeur
 */
Rectangle2::Rectangle2()
{
    // créer le maillage
    m_Mesh = new Mesh("Rectangle2");

    // sommets
    MeshVertex* B = m_Mesh->addVertex("B");
    B->setCoord(vec3::fromValues(-1.0, -1.0, 0.0));
    B->setTexCoord(vec2::fromValues(0.25, 0.0));

    MeshVertex* C = m_Mesh->addVertex("C");
    C->setCoord(vec3::fromValues(+1.0, -1.0, 0.0));
    C->setTexCoord(vec2::fromValues(0.75, 0.0));

    MeshVertex* F = m_Mesh->addVertex("F");
    F->setCoord(vec3::fromValues(+1.0, +1.0, 0.0));
    F->setTexCoord(vec2::fromValues(0.75, 1.0));

    MeshVertex* G = m_Mesh->addVertex("G");
    G->setCoord(vec3::fromValues(-1.0, +1.0, 0.0));
    G->setTexCoord(vec2::fromValues(0.25, 1.0));

    // quadrilatère
    m_Mesh->addQuad(B, C, F, G);

    // créer le matériau du rectangle
    m_Texture = new Texture2D("data/textures/1024px-LandscapeArchPano.jpg", GL_LINEAR, GL_REPEAT);
    m_Material = new TextureMaterial(m_Texture);

    // créer les VBO pour afficher ce maillage
    MeshModuleDrawing renderer(m_Mesh);
    m_VBOset = renderer.createStripVBOset(m_Material, true);
}


/**
 * dessine l'objet sur l'écran
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Rectangle2::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    m_VBOset->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Rectangle2::~Rectangle2()
{
    delete m_Material;
    delete m_VBOset;
    delete m_Mesh;
}
