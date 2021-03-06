#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <gl-matrix.h>
#include <utils.h>
#include <SceneBase.h>
#include <Texture2D.h>

#include "BWThreshold.h"


class Scene: public SceneBase
{
private:

    Texture2D* m_Texture;
    BWThreshold* m_BWThreshold;

public:

    /** constructeur, crée les objets 3D à dessiner */
    Scene();

    /** destructeur, libère les ressources */
    ~Scene();

    /** Dessine l'image courante */
    void onDrawFrame();

};

#endif
