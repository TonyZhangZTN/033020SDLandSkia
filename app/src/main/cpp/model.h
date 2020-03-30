#include "head.h"

extern AAssetManager *g_pAssetManager;
extern GLuint g_programObject;
extern GLint g_width, g_height;
extern SDL_Window *g_window;
extern GLuint g_blockBufferId;
extern GLint g_lightBufferSize;
struct Material {
    vec4 Ka;
    vec4 Kd;
    vec4 Ks;
};
struct Vertex {
    vec3 locationPos;
    vec3 normal;
    vec2 texturePos;
};
struct Texture {
    GLuint id;
    string type;
    string path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<Texture> textures;
    vector<unsigned int> indices;
    Material mater;
    GLuint VAO, VBO, EBO, bindingPoint, bindingBuffer;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures,
         Material mat);

    void setupMesh();

    void draw();
};

class Model {
public:
#ifdef ZTN_DEBUG
    int chance;
    int mes;
#endif
    string srcRootDir, srcName;
    int num;
    vector<Mesh> meshes;
    vector<Texture> loadedTextures;
    bool *flagMeshes;

    Model();

    void clear();

    void draw();

    void processNode(aiNode *node, const aiScene *scene);

    void loadModel(string srcRootDir, string srcName);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    vector<Texture> loadTexture(aiTextureType type, aiMaterial *material, string typeName);

    GLuint textureFromFile(const char *src);
};

class Cube {
public:
    GLuint VAO, VBO, EBO;

    void init();

    void draw();
};