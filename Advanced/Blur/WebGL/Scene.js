﻿// Définition de la classe Scene

// superclasses et classes nécessaires
Requires("libs/Misc/TurnTableScene.js");
Requires("libs/Misc/MeshObjectFromObj.js");
Requires("libs/Material/SoftSpotLight.js");
Requires("libs/Process/FrameBufferObject.js");
Requires("PoissonBlur");


class Scene extends TurnTableScene
{
    /**
     * Constructeur
     */
    constructor()
    {
        super(true);

        // créer les objets à dessiner
        this.m_Ground   = new MeshObjectFromObj("data/models/TerrainSimple", "Terrain.obj", "Terrain.mtl", 4.0);
        this.m_Lorry    = new MeshObjectFromObj("data/models/Camion", "camion.obj", "camion.mtl", 2.0);
        this.m_PalmTree = new MeshObjectFromObj("data/models/Palm_Tree", "Palm_Tree.obj", "Palm_Tree.mtl", 0.3);

        // définir une lampe spot qui projette de l'ombre
        this.m_Light0 = new SoftSpotLight(24.0, 20.0, 0.5, 512, 14, 26);
        this.m_Light0.setPosition(vec4.fromValues(5, 15, 15, 1));
        this.m_Light0.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light0.setColor(vec3.fromValues(400,400,400));
        this.addLight(this.m_Light0);

        // définir une lampe spot qui projette de l'ombre
        this.m_Light1 = new SoftSpotLight(40.0, 40.0, 0.2, 512, 12, 23);
        this.m_Light1.setPosition(vec4.fromValues(8, 15, -6, 1));
        this.m_Light1.setTarget(vec4.fromValues(0, 0, 0, 1));
        this.m_Light1.setColor(vec3.fromValues(300,300,300));
        this.addLight(this.m_Light1);

        // configurer les modes de dessin
        gl.enable(gl.DEPTH_TEST);
        gl.enable(gl.CULL_FACE);

        // couleur du fond
        gl.clearColor(0.4, 0.5, 0.6, 0.0);

        // paramètres de la vue
        this.m_CameraAngleAzimut = -40.0;
        this.m_CameraAngleElevation = 35.0;
        vec3.set(this.m_CameraPivot, 0.0, -0.5, 0.0);
        this.m_CameraDistance = 30;

        // traitement d'image nécessaire
        this.m_FBOimage = null;
        this.m_PoissonBlur = new PoissonBlur();
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    onSurfaceChanged(width, height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height, 1);

        // matrice de projection (champ de vision)
        mat4.perspective(this.m_Mat4Projection, Utils.radians(12.0), width / height, 1.0, 100.0);

        // créer un FBO de cette taille pour dessiner hors écran
        if (this.m_FBOimage != null) this.m_FBOimage.destroy();
        this.m_FBOimage = new FrameBufferObject(width, height, gl.TEXTURE_2D, gl.RENDERBUFFER);
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4View : matrice de vue
     */
    onDraw(mat4Projection, mat4View)
    {
        // matrice temporaire
        let mat4ModelView = mat4.create();

        // dessiner le terrain
        this.m_Ground.onDraw(mat4Projection, mat4View);

        // dessiner les palmiers
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(0.5,0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(3.0,0,1.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(2.0,0,-3.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(20.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,-2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(80.0));
        this.m_PalmTree.onDraw(mat4Projection, mat4ModelView);

        // dessiner les camions
        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-2.0,0.0,0.5));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(120.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(1.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(-20.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);

        mat4.translate(mat4ModelView, mat4View, vec3.fromValues(-1.0,0.0,2.0));
        mat4.rotateY(mat4ModelView, mat4ModelView, Utils.radians(200.0));
        this.m_Lorry.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Dessine l'image courante
     */
    onDrawFrame()
    {
        // construire la matrice caméra <- scène (fait passer de la scène à la caméra)
        let mat4CameraScene = this.getModelView();

        // préparer les lampes (transformation et shadow maps)
        this.prepareLights(mat4CameraScene);

        // dessiner les objets dans cette transformation
        this.drawDeferredShading(this.m_Mat4Projection, mat4CameraScene);

        // rediriger les dessins vers le FBO net
        this.m_FBOimage.enable();

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // dessiner l'éclairement de la première lampe
        this.addLighting(this.m_Light0, true);

        // rajouter les éclairements des lampes suivantes
        this.addLighting(this.m_Light1);

        // revenir au dessin sur l'écran
        this.m_FBOimage.disable();

        // (debug) dessiner le FBO sur l'écran
        //this.m_FBOnet.onDraw(gl.COLOR_ATTACHMENT0);return;

        // effacer l'écran
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        // à ce stade, on a une image nette dans this.m_FBOnet, on va la flouter
        let radius = /*Math.cos(Utils.Time) * */ 20.0;
        this.m_PoissonBlur.process(this.m_FBOimage, radius);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    destroy()
    {
        this.m_FBOimage.destroy();
        this.m_PoissonBlur.destroy();

        this.m_Light1.destroy();
        this.m_Light0.destroy();
        this.m_PalmTree.destroy();
        this.m_Lorry.destroy();
        this.m_Ground.destroy();
        super.destroy();
    }
}
