#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <DeferredShadingMaterial.h>
#include "Scene.h"



/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer le matériau de l'objet
    vec3 Ks = vec3::fromValues(0.4, 0.4, 0.4);
    float Ns = 200;
    m_Material = new DeferredShadingMaterial(Ks, Ns);

    // définir une lampe ambiante
    m_Light = new Light();
    m_Light->setColor(vec3::fromValues(1.0,1.0,1.0));
    addLight(m_Light);

    // créer l'objet à dessiner
    m_Object = new SquaredPyramid(m_Material);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.4, 0.4, 0.0);

    // paramètres de la vue
    m_CameraAngleAzimut = 210.0;
    m_CameraAngleElevation = 10.0;
    vec3::set(m_CameraPivot, 0.0, +0.2, 0.0);
    m_CameraDistance = 7;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(width, height);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(25.0), (float)width / height, 0.1, 20.0);
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4ModelView : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4ModelView)
{
    // dessiner l'objet selon les modes demandés
    m_Object->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_Object;
    delete m_Light;
    delete m_Material;
}
