#ifndef SCENE_H
#define SCENE_H

// Définition de la classe Scene

#include <vector>
#include <gl-matrix.h>
#include <utils.h>
#include <TurnTableScene.h>

#include "Ground.h"
#include "Texture360.h"
#include "Billboard.h"


class Scene: public TurnTableScene
{
public:

    /** constructeur, crée les objets 3D à dessiner */
    Scene();

    /** destructeur, libère les ressources */
    ~Scene();

    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    void onSurfaceChanged(int width, int height);

    /**
     * dessin des objets de la scène sur l'écran
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    void onDraw(mat4& mat4Projection, mat4& mat4ModelView);


private:

    Ground* m_Ground;
    Texture360* m_TxTree;
    std::vector<Billboard*> m_Forest;

};

#endif
