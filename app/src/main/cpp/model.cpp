#include "model.h"
AAssetManager *g_pAssetManager;
GLuint g_programObject;
GLint g_width, g_height;
SDL_Window *g_window;
GLuint g_blockBufferId;
GLint g_lightBufferSize;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures,
           Material mat) {
    this->indices = indices;
    this->vertices = vertices;
    this->textures = textures;
    this->mater = mat;
    setupMesh();
}

void Mesh::setupMesh() {
    LOGI("a:%f %f %f", mater.Ka.x, mater.Ka.y, mater.Ka.z);
    LOGI("d:%f %f %f", mater.Kd.x, mater.Kd.y, mater.Kd.z);
    LOGI("s:%f %f %f", mater.Ks.x, mater.Ks.y, mater.Ks.z);
    GLint locationPos, normal, texturePos;
    locationPos = glGetAttribLocation(g_programObject, "vPosition");
    texturePos = glGetAttribLocation(g_programObject, "texPosition");
    normal = glGetAttribLocation(g_programObject, "normal");
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0],
                 GL_STATIC_DRAW
    );
    glEnableVertexAttribArray((GLuint) locationPos);
    glVertexAttribPointer((GLuint) locationPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray((GLuint) texturePos);
    glVertexAttribPointer((GLuint) texturePos, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex,
                                            texturePos));//there texturePos is a member of structure 'Vertex'
    glEnableVertexAttribArray((GLuint) normal);
    glVertexAttribPointer((GLuint) normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *) offsetof(Vertex,
                                            normal));//there normal is a member of structure 'Vertex'
    glBindVertexArray(0);
}

void Mesh::draw() {
    for (unsigned int i = 0; i < textures.size(); i++) {
        //TODO What are you fucking coding?
        if (textures[i].type == "diffuse") {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
            glUniform1i(glGetUniformLocation(g_programObject, "texId"), 1);
            break;
        }
    }
    GLfloat lightData[] = {
            mater.Ka.x, mater.Ka.y, mater.Ka.z, 1.0f,
            mater.Kd.x, mater.Kd.y, mater.Kd.z, 1.0f,
            mater.Ks.x, mater.Ks.y, mater.Ks.z, 1.0f,
    };

    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_UNIFORM_BUFFER, g_blockBufferId);
    glBufferData(GL_UNIFORM_BUFFER, g_lightBufferSize, lightData, GL_DYNAMIC_DRAW);
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Model::clear() {
    //TODO[Error][Date 02.29.20] Need to Destroy VAO etc.
    meshes.clear();
    loadedTextures.clear();
    delete[] flagMeshes;
}

void Model::draw() {
#ifdef ZTN_DEBUG
    chance++;
    if(chance == 50)
    {
        chance = 0;
        mes +=1;
        if(mes == meshes.size())
        {
            mes = 0;
        }
        LOGD("mesh[%d]",mes);
    }
    meshes[mes].draw();
#else
    for (int i = 0; i < meshes.size(); i++) {
        if (flagMeshes[i])
            meshes[i].draw();
    }

#endif
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for (int i = 0; i < node->mNumMeshes; i++) {
        num++;
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));

    }
    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::loadModel(string srcRootDir, string srcName) {
    this->srcName = srcName;
    this->srcRootDir = srcRootDir;
    Importer importer;
    const aiScene *scene = importer.ReadFile((srcRootDir + "/" + srcName).c_str(),
                                             aiProcess_Triangulate | aiProcess_FlipUVs
    );
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOGE("error%s", importer.GetErrorString());
        return;
    }
    processNode(scene->mRootNode, scene);
    LOGI("num of meshes:%ld", meshes.size());
    flagMeshes = new bool[meshes.size()];
    memset(flagMeshes, true, sizeof(bool) * meshes.size());
    flagMeshes[meshes.size()] = false;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    Material mate;
    vec3 bufVertex;
    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.locationPos.x = mesh->mVertices[i].x;
        vertex.locationPos.y = mesh->mVertices[i].y;
        vertex.locationPos.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) {
            vertex.texturePos.x = mesh->mTextureCoords[0][i].x;
            vertex.texturePos.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texturePos.x = 0.0f;
            vertex.texturePos.y = 0.0f;
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color3D;
    material->Get(AI_MATKEY_COLOR_AMBIENT, color3D);
    mate.Ka = vec4(color3D.r, color3D.g, color3D.b, 1.0f);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color3D);
    mate.Kd = vec4(color3D.r, color3D.g, color3D.b, 1.0f);
    material->Get(AI_MATKEY_COLOR_SPECULAR, color3D);
    mate.Ks = vec4(color3D.r, color3D.g, color3D.b, 1.0f);

    vector<Texture> diffuse = loadTexture(aiTextureType_DIFFUSE, material, "diffuse");
    vector<Texture> ambient = loadTexture(aiTextureType_AMBIENT, material, "ambient");
    vector<Texture> specular = loadTexture(aiTextureType_SPECULAR, material, "specular");
    textures.insert(textures.end(), diffuse.begin(), diffuse.end());
    textures.insert(textures.end(), ambient.begin(), ambient.end());
    textures.insert(textures.end(), specular.begin(), specular.end());
    LOGI("Amount of Diffuse texture of mesh[%d]:%ld", num, diffuse.size());
    LOGI("Amount of Ambient texture of mesh[%d]:%ld", num, ambient.size());
    LOGI("Amount of Specular texture of mesh[%d]:%ld", num, specular.size());
    return Mesh(vertices, indices, textures, mate);
}

vector<Texture> Model::loadTexture(aiTextureType type, aiMaterial *material, string typeName) {
    vector<Texture> textures;
    for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
        aiString pBuffer;
        material->GetTexture(type, i, &pBuffer);

        bool skip = false;
        for (int j = 0; j < loadedTextures.size(); j++) {
            if (!strcmp(loadedTextures[j].path.c_str(), pBuffer.C_Str())) {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture tex;
            tex.id = textureFromFile(pBuffer.C_Str());
            tex.path = string(pBuffer.C_Str());
            tex.type = typeName;
            loadedTextures.push_back(tex);
            textures.push_back(tex);
        }
    }
    return textures;

}

GLuint Model::textureFromFile(const char *src) {
    string filename = string(src);
    filename = srcRootDir + "/" + filename;
    GLuint textureId = 0;
    glGenTextures(1, &textureId);

    int width, height, n;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &n, 0);
    if (data) {
        GLenum format;
        switch (n) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                LOGE("unknown format!!");
                format = GL_RGB;
                break;
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        LOGE("can't load file:%s", filename.c_str());
    }
    LOGI("%u", textureId);
    glBindTexture(GL_TEXTURE_2D, 0);
    vector<int> array1;
    vector<int> array2;
    array2.insert(array2.end(), array1.begin(), array1.end());
    return textureId;
}

Model::Model() {
#ifdef ZTN_DEBUG
    this->chance = 0;
    this->mes = 0;
#endif
}

void Cube::init() {
    GLfloat vertices[] = {
            2.0f, -2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -2.0f, 2.0f, -2.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -2.0f, -2.0f, -2.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -4.0f, -4.0f, 10.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            4.0f, -4.0f, 10.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 4.0f, 10.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -2.0f, -2.0f, 5.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            2.0f, -2.0f, 5.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 2.0f, 5.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    GLuint indices[] = {
            0, 1, 3, 1, 2, 3,
            0, 4, 5, 0, 5, 1,
            1, 5, 6, 1, 6, 2,
            6, 7, 3, 6, 3, 2,
            5, 4, 7, 5, 7, 6,
            7, 4, 0, 7, 0, 3,
            8, 9, 10, 11, 12, 13
    };
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 14 * 7, vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 7 * 6, indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray((GLuint) glGetAttribLocation(g_programObject, "vPosition"));
    glEnableVertexAttribArray((GLuint) glGetAttribLocation(g_programObject, "aColor"));
    glVertexAttribPointer((GLuint) glGetAttribLocation(g_programObject, "vPosition"), 3, GL_FLOAT,
                          GL_FALSE, 7 * 4, 0
    );
    glVertexAttribPointer((GLuint) glGetAttribLocation(g_programObject, "aColor"), 4, GL_FLOAT,
                          GL_FALSE, 7 * 4, (void *) 12
    );
    glBindVertexArray(0);
    LOGI("VAO%d", VAO);
    LOGI("VBO%d", VBO);
    LOGI("EBO%d", EBO);
}

void Cube::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);
}
