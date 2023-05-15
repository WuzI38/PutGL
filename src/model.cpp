#include "headers/model.h"

void Model::loadModel(std::string filepath)
{
	// Load the object file
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

	// Check if errors occured
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error loading mesh: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Take the first mesh
	aiMesh* mesh = scene->mMeshes[0];

	// Load vertices, normals and texCoords
	for (int i = 0; i < mesh->mNumVertices; i++) {
		aiVector3D vertex = mesh->mVertices[i];
		this->vertices.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

		aiVector3D normal = mesh->mNormals[i];
		this->normals.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));

		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		this->texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
	}

	// Loop through the indices of the face and add them to a vector
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace& face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			this->indices.push_back(face.mIndices[j]);
		}
	}
}

void Model::loadTexture(std::string filepath) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	// Load to memory
	std::vector<unsigned char> image;
	unsigned width, height;
	// Load image
	unsigned error = lodepng::decode(image, width, height, filepath);
	/*if (!error) {
		std::cerr << "Error: the texture cannot be loaded properly" << std::endl;
		return;
	}*/

	// Import to GPU memory
	glGenTextures(1, &tex); // Init handle
	glBindTexture(GL_TEXTURE_2D, tex); // Activate handle
	// Load image + handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->texture = tex;
}

void Model::draw() {

	glEnableVertexAttribArray(spLambertTextured->a("vertex"));
	glVertexAttribPointer(spLambertTextured->a("vertex"), 4, GL_FLOAT, false, 0, this->vertices.data()); //Wspó³rzêdne wierzcho³ków bierz z tablicy myCubeVertices

	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
	glVertexAttribPointer(spLambertTextured->a("texCoord"), 2, GL_FLOAT, false, 0, this->texCoords.data()); //Wspó³rzêdne teksturowania bierz z tablicy myCubeTexCoords

	glEnableVertexAttribArray(spLambertTextured->a("normal"));
	glVertexAttribPointer(spLambertTextured->a("normal"), 4, GL_FLOAT, false, 0, this->normals.data()); //Wspó³rzêdne teksturowania bierz z tablicy myCubeTexCoords

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glUniform1i(spLambertTextured->u("tex"), 0);

	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, this->indices.data());

	glDisableVertexAttribArray(spLambertTextured->a("vertex"));
	glDisableVertexAttribArray(spLambertTextured->a("texCoord"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));
}

Model::Model(const char* modelPath, const char* texturePath) {
	this->loadModel(modelPath);
	this->loadTexture(texturePath);
}

Model::~Model() {
	glDeleteTextures(1, &this->texture);
}
