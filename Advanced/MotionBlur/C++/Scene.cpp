#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(true)
{
    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);

    // définir une lampe spot qui projette de l'ombre
    m_Light0 = new SoftSpotLight(24.0, 20.0, 0.5, 512, 14, 26);
    m_Light0->setPosition(vec4::fromValues(5, 15, 15, 1));
    m_Light0->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light0->setColor(vec3::fromValues(400,400,400));
    addLight(m_Light0);

    // définir une lampe spot qui projette de l'ombre
    m_Light1 = new SoftSpotLight(40.0, 40.0, 0.2, 512, 12, 23);
    m_Light1->setPosition(vec4::fromValues(8, 15, -6, 1));
    m_Light1->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light1->setColor(vec3::fromValues(300,300,300));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // couleur du fond
    glClearColor(0.4, 0.5, 0.6, 0.0);
    m_SkyBackground = new SkyBackground();

    // paramètres de la vue
    m_CameraAngleAzimut = -40.0;
    m_CameraAngleElevation = 35.0;
    vec3::set(m_CameraPivot, 0.0, -0.5, 0.0);
    m_CameraDistance = 30;

    // FBO nécessaires
    m_CurrentFBOnumber = 0;
}


/**
 * appelée quand la taille de la vue OpenGL change
 * @param width : largeur en nombre de pixels de la fenêtre
 * @param height : hauteur en nombre de pixels de la fenêtre
 */
void Scene::onSurfaceChanged(int width, int height)
{
    // facteur d'agrandissement du FBO pour simuler un antialiasing global
    const int scale = 2;

    // appeler la méthode de la superclasse
    TurnTableScene::onSurfaceChanged(width, height, scale);

    // matrice de projection (champ de vision)
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 1.0, 100.0);

    // créer plusieurs FBO de cette taille pour mémoriser les images successives
    for (int i=0; i<m_FBOimages.size(); i++) delete m_FBOimages[i];
    m_FBOimages.clear();
    for (int i=0; i<IMAGES_COUNT; i++) {
        m_FBOimages.push_back(new FrameBufferObject(width * scale, height * scale, GL_TEXTURE_2D, GL_RENDERBUFFER));
    }
}


/**
 * dessine les objets de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDraw(mat4& mat4Projection, mat4& mat4View)
{
    // matrice temporaire
    mat4 mat4ModelView = mat4::create();

    // dessiner le terrain
    m_Ground->onDraw(mat4Projection, mat4View);

    // dessiner les palmiers
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.5,0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(3.0,0,1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.0,0,-3.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,-2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les camions
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2.0,0.0,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(200.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);
}


/**
 * Dessine l'image courante
 */
void Scene::onDrawFrame()
{
    // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
    mat4 mat4CameraScene = getModelView();

    // préparer les lampes (transformation et shadow maps)
    prepareLights(mat4CameraScene);

    // dessiner les objets dans cette transformation
    drawDeferredShading(m_Mat4Projection, mat4CameraScene);

    // rediriger les dessins vers le FBO courant
    m_FBOimages[m_CurrentFBOnumber]->enable();

    // effacer l'écran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dessiner le fond de ciel
    m_SkyBackground->process(m_GBuffer);

    // dessiner l'éclairement de la première lampe
    addLighting(m_Light0, true);

    // rajouter les éclairements des lampes suivantes
    addLighting(m_Light1);

    // revenir au dessin sur l'écran
    m_FBOimages[m_CurrentFBOnumber]->disable();

    // (debug) dessiner le FBO sur l'écran
    //m_FBOimages[m_CurrentFBOnumber]->onDraw(GL_COLOR_ATTACHMENT0);/*

    // effacer l'écran en noir pour additionner les images
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // superposer les images des FBO, avec un facteur de 1/m_NbImages
    m_FBOimages[0]->onDraw(GL_COLOR_ATTACHMENT0);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    glBlendColor(0.0, 0.0, 0.0, 1.0/(IMAGES_COUNT-1));
    glEnable(GL_BLEND);
    for (int i=1; i<IMAGES_COUNT; i++) {
        m_FBOimages[i]->onDraw(GL_COLOR_ATTACHMENT0);
    }
    glDisable(GL_BLEND);

    // passer à l'image suivante
    m_CurrentFBOnumber = (m_CurrentFBOnumber + 1) % IMAGES_COUNT;
    //*/
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_SkyBackground;
    delete m_Light1;
    delete m_Light0;
    delete m_PalmTree;
    delete m_Lorry;
    delete m_Ground;

    for (int i=0; i<IMAGES_COUNT; i++) delete m_FBOimages[i];
}
