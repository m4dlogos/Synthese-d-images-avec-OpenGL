﻿// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("Ground");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(false);

        // créer l'objet
        this.m_Object = new Ground();

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        //gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.4, 0.4, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = 15.0;
        this.m_CameraAngleElevation = 50.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.1, 0.0);
        this.m_CameraDistance = 9.5;
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(10.0), width / height, 0.1, 20.0);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    onDraw(mat4Projection, mat4ModelView)
    {
        // dessiner l'objet
        this.m_Object.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_Object.destroy();
        super.destroy();
    }
}
