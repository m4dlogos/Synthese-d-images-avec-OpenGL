package com.dbooker.livreopengl;

import static android.opengl.GLES20.*;

import com.dbooker.livreopengl.misc.TurnTableScene;
import com.dbooker.livreopengl.glmatrix.mat4;
import com.dbooker.livreopengl.glmatrix.vec3;
import com.dbooker.livreopengl.glmatrix.vec4;
import com.dbooker.livreopengl.misc.MeshStructureDraw;
import com.dbooker.livreopengl.material.OmniLight;
import com.dbooker.livreopengl.material.DeferredShadingMaterial;
import com.dbooker.livreopengl.material.Material;
import com.dbooker.livreopengl.utils.Utils;


public class Scene extends TurnTableScene
{
    // objets de la scène
    private Disk m_Object;
    private Material m_Material;
    private OmniLight m_Light0;
    private OmniLight m_Light1;
    private MeshStructureDraw m_MeshStructureDraw;


    /** créer les objets et initialise OpenGL */
    public Scene() throws Exception
    {
        super(true);

        // créer le matériau de l'objet
        vec4 Kd = vec4.fromValues(0.7, 0.3, 0.3, 1.0);
        vec3 Ks = vec3.fromValues(0.1, 0.1, 0.1);
        float Ns = 64;
        m_Material = new DeferredShadingMaterial(Kd, Ks, Ns);

        // définir une lampe directionnelle
        m_Light0 = new OmniLight();
        m_Light0.setPosition(vec4.fromValues(1, 2, 1, 0));
        m_Light0.setColor(vec3.fromValues(0.7,0.7,0.7));
        addLight(m_Light0);

        // définir une lampe positionnelle
        m_Light1 = new OmniLight();
        m_Light1.setPosition(vec4.fromValues(-5, 10, -5, 1));
        m_Light1.setColor(vec3.fromValues(150,150,150));
        addLight(m_Light1);

        // créer l'objet à dessiner
        m_Object = new Disk();

        // configurer les modes de dessin
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // couleur du fond
        glClearColor(0.4f, 0.4f, 0.4f, 0.0f);

        // paramètres de la vue
        m_CameraAngleAzimut = 50.0f;
        m_CameraAngleElevation = 20.0f;
        m_CameraDistance = 7.0f;

        // modes de dessin : dessiner les arêtes par défaut
        m_MeshStructureDraw = new MeshStructureDraw(m_Object.getMesh(), m_Material, null, null, null, 0.5f);
        m_MeshStructureDraw.nextEdgesMode();
    }


    /**
     * appelée quand la taille de la vue OpenGL change
     * @param width : largeur en nombre de pixels de la fenêtre
     * @param height : hauteur en nombre de pixels de la fenêtre
     */
    @Override
    public void onSurfaceChanged(int width, int height)
    {
        // appeler la méthode de la superclasse
        super.onSurfaceChanged(width, height);

        // matrice de projection (champ de vision)
        mat4.perspective(m_Mat4Projection, Utils.radians(25.0), (float)width / height, 0.1f, 20.0f);
    }


    /** appelée quand on appuie sur une touche du clavier **/
    @Override
    public void onKeyDown(char code)
    {
        switch (code) {
        case 'T':
            m_MeshStructureDraw.nextTrianglesMode();
            break;
        case 'Y':
            m_MeshStructureDraw.nextEdgesMode();
            break;
        case 'U':
            m_MeshStructureDraw.nextNormalsMode();
            break;
        default:
            // appeler la méthode de la superclasse
            super.onKeyDown(code);
        }
    }


    /**
     * dessine les objets de la scène
     * @param mat4Projection : matrice de projection
     * @param mat4ModelView : matrice de vue
     */
    @Override
    public void onDraw(mat4 mat4Projection, mat4 mat4ModelView)
    {
        // dessiner l'objet selon les modes demandés
        m_MeshStructureDraw.onDraw(mat4Projection, mat4ModelView);
    }


    /**
     * Cette méthode supprime les ressources allouées
     */
    @Override
    public void destroy()
    {
        m_MeshStructureDraw.destroy();
        m_Light1.destroy();
        m_Light0.destroy();
        m_Material.destroy();
        m_Object.destroy();
        super.destroy();
    }
}
