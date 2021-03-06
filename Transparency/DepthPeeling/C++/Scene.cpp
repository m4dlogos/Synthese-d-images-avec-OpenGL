#include <GL/glew.h>
#include <GL/gl.h>

#include <iostream>

#include <utils.h>
#include <MeshObjectFromObj.h>

#include "Scene.h"
#include <TransparentMaterial.h>


/** NOMBRE DE COUCHES À DESSINER, mettre entre 0 et 10, mais plus que 5 est peu utile **/
const int NumLayers = 5;


/**
 * Constructeur
 */
Scene::Scene() : TurnTableScene(false)
{
    // créer le matériau de dessin de la pomme (couleurs forcées sinon trop sombres)
    vec4 Kd = vec4::fromValues(2.0, 0.4, 0.2, 0.5);
    vec3 Ks = vec3::fromValues(2.0, 2.0, 2.0);
    float Ns = 128.0;
    m_TransparentMaterial = new TransparentMaterial(Kd, Ks, Ns);

    // créer les objets à dessiner
    m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "TerrainHerbe.mtl", 4.0);
    m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.4);
    m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
    m_Apple    = new MeshObjectFromObj("data/models/Apple","apple.obj", m_TransparentMaterial, 0.02);

    // définir une lampe
    m_Light0 = new OmniLight();
    //m_Light0 = new SoftSpotLight(24.0, 20.0, 0.5, 512, 14, 26);
    //m_Light0->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light0->setPosition(vec4::fromValues(5, 15, 15, 1));
    m_Light0->setColor(vec3::fromValues(500,500,500));
    addLight(m_Light0);

    // définir une lampe
    m_Light1 = new OmniLight();
    //m_Light1 = new SoftSpotLight(40.0, 40.0, 0.2, 512, 12, 23);
    //m_Light1->setTarget(vec4::fromValues(0, 0, 0, 1));
    m_Light1->setPosition(vec4::fromValues(8, 15, -6, 1));
    m_Light1->setColor(vec3::fromValues(300,300,300));
    addLight(m_Light1);

    // configurer les modes de dessin
    glEnable(GL_DEPTH_TEST);

    // couleur du fond
    glClearColor(0.5, 0.6, 0.7, 0.0);
    m_SkyBackground = new SkyBackground();

    // paramètres de la vue
    m_CameraAngleAzimut = -30.0;
    m_CameraAngleElevation = 20.0;
    vec3::set(m_CameraPivot, 0.0, -0.5, 0.0);
    m_CameraDistance = 40;

    // ressources
    m_GBuffer = nullptr;
    m_PreviousGBuffer = nullptr;
    m_CurrentGBuffer = nullptr;
    m_FBOlights  = nullptr;
    m_FBOlayers = nullptr;
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
    mat4::perspective(m_Mat4Projection, Utils::radians(12.0), (float)width / height, 20.0, 60.0);

    // supprimer les anciens FBO
    if (m_GBuffer != nullptr) delete m_GBuffer;
    if (m_PreviousGBuffer != nullptr) delete m_PreviousGBuffer;
    if (m_CurrentGBuffer != nullptr) delete m_CurrentGBuffer;
    if (m_FBOlights != nullptr) delete m_FBOlights;
    if (m_FBOlayers != nullptr) delete m_FBOlayers;

    // agrandissement des FBO pour améliorer la qualité d'image
    const int K = 2;

    // FBO pour la première passe du dessin des objets opaques
    m_GBuffer = new FrameBufferObject(width*K, height*K, GL_TEXTURE_2D, GL_TEXTURE_2D, 3);

    // FBO pour la première passe du dessin des objets transparents (alternances entre les deux)
    m_PreviousGBuffer = new FrameBufferObject(width*K, height*K, GL_TEXTURE_2D, GL_TEXTURE_2D, 3);
    m_CurrentGBuffer = new FrameBufferObject(width*K, height*K, GL_TEXTURE_2D, GL_TEXTURE_2D, 3);

    // FBO pour additionner les éclairements des lampes
    m_FBOlights = new FrameBufferObject(width*K, height*K, GL_TEXTURE_2D, GL_NONE);

    // FBO pour accumuler la deuxième passe du dessin des objets transparents
    m_FBOlayers = new FrameBufferObject(width*K, height*K, GL_TEXTURE_2D, GL_NONE);

    // fournir la taille de la fenêtre au matériau transparent
    m_TransparentMaterial->setWindowDimensions(width*K, height*K);
}


/**
 * dessine les objets opaques de la scène
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
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.0,0.0,0.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(30.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0,0.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(210.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-0.5,0.0,-2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(140.0));
    m_PalmTree->onDraw(mat4Projection, mat4ModelView);

    // dessiner les camions
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-2.0,0.0,-0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.0,0.0,1.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(-20.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,0.0,2.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(200.0));
    m_Lorry->onDraw(mat4Projection, mat4ModelView);
}


/**
 * dessine les objets transparents de la scène
 * @param mat4Projection : matrice de projection
 * @param mat4View : matrice de vue
 */
void Scene::onDrawTranspa(mat4& mat4Projection, mat4& mat4View)
{
    // matrice temporaire
    mat4 mat4ModelView = mat4::create();

    // dessiner les pommes
    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(0.0,-3.0,0.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(120.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(5.0, 5.0, 5.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.5,-0.5,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    mat4::scale(mat4ModelView, mat4ModelView, vec3::fromValues(2.0, 2.0, 2.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(2.0,0,-1.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(20.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-3.0,0.0,-3.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-1.0,1.0,2.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(1.0,1.5,0.5));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);

    mat4::translate(mat4ModelView, mat4View, vec3::fromValues(-3.0,0.5,0.0));
    mat4::rotateY(mat4ModelView, mat4ModelView, Utils::radians(80.0));
    m_Apple->onDraw(mat4Projection, mat4ModelView);
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

    // effacer le depth buffer du 2e g-buffer transparents
    glClearDepth(0.0);
    m_PreviousGBuffer->enable();
    glClear(GL_DEPTH_BUFFER_BIT);
    m_PreviousGBuffer->disable();
    glClearDepth(1.0);

    // effacer le FBO qui contiendra les couches transparentes
    m_FBOlayers->enable();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_FBOlayers->disable();


    /// dessiner les objets opaques sur l'écran

    // ne dessiner que les faces avant
    glEnable(GL_CULL_FACE);

    // dessiner les objets opaques dans le g-buffer
    m_GBuffer->enable();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    onDraw(m_Mat4Projection, mat4CameraScene);
    m_GBuffer->disable();
    //m_GBuffer->onDraw(GL_DEPTH_ATTACHMENT);/*

    // dessiner les éclairements des objets opaques directement à l'écran
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_SkyBackground->process(m_GBuffer);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
    m_Light0->process(m_GBuffer);
    m_Light1->process(m_GBuffer);
    glDisable(GL_BLEND);

    /// Cycles d'épluchage des transparences

    // dessiner toutes les faces des objets
    glDisable(GL_CULL_FACE);

    // NumLayers est définie au début de ce source
    for (int i=0; i<NumLayers; i++) {

        // fournir le depth buffer du g-buffer opaques au matériau transparent
        m_TransparentMaterial->setDepthMaps(m_GBuffer->getDepthBuffer(), m_PreviousGBuffer->getDepthBuffer());

        // rediriger les dessins vers le g-buffer première passe des transparents
        m_CurrentGBuffer->enable();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        onDrawTranspa(m_Mat4Projection, mat4CameraScene);
        m_CurrentGBuffer->disable();
        //m_GBuffer_courant->onDraw(GL_COLOR_ATTACHMENT0);return;}/*
        //m_GBuffer_courant->onDraw(GL_DEPTH_ATTACHMENT);return;}/*

        // ajouter les éclairements des lampes sur les objets transparents
        m_FBOlights->enable();
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        m_Light0->process(m_CurrentGBuffer);
        m_Light1->process(m_CurrentGBuffer);
        glDisable(GL_BLEND);
        m_FBOlights->disable();
        //m_FBOlights->onDraw(GL_COLOR_ATTACHMENT0);return;}/*
        //m_FBOlights->onDrawAlpha(GL_COLOR_ATTACHMENT0);return;}/*

        // mélanger cette couche à l'arrière du FBO des couches
        m_FBOlayers->enable();
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
        m_FBOlights->onDraw(GL_COLOR_ATTACHMENT0);
        glDisable(GL_BLEND);
        m_FBOlayers->disable();
        //m_FBOlayers->onDraw(GL_COLOR_ATTACHMENT0);return;}/*
        //m_FBOlayers->onDrawAlpha(GL_COLOR_ATTACHMENT0);return;}/*

        // échanger les g-buffers
        FrameBufferObject* tmp = m_CurrentGBuffer;
        m_CurrentGBuffer = m_PreviousGBuffer;
        m_PreviousGBuffer = tmp;
    }
    //m_FBOlayers->onDraw(GL_COLOR_ATTACHMENT0);/*
    //m_FBOlayers->onDrawAlpha(GL_COLOR_ATTACHMENT0);/*

    // superposer le FBO des couches transparentes sur l'écran
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    m_FBOlayers->onDraw(GL_COLOR_ATTACHMENT0);
    glDisable(GL_BLEND);

    //si on décommente l'une des lignes de mise au point*/
}


/**
 * Cette méthode supprime les ressources allouées
 */
Scene::~Scene()
{
    delete m_SkyBackground;
    delete m_Light1;
    delete m_Light0;
    delete m_Apple;
    delete m_Lorry;
    delete m_PalmTree;
    delete m_Ground;
    delete m_TransparentMaterial;
    delete m_PreviousGBuffer;
    delete m_CurrentGBuffer;
    delete m_FBOlights;
    delete m_FBOlayers;
}
