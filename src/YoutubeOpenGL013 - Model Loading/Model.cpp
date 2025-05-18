#include "Model.h"


Model::Model(const char* file)
{
  std::string text = get_file_contents(file);
  JSON = nlohmann::json::parse(text);
  
  Model::file = file;
  data = getData();
  
  traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera)
{
  for (unsigned int i = 0; i < meshes.size(); i++)
  {
    meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i]);
  }
}


void Model::loadMesh(unsigned int indMesh)
{
  unsigned int posAccId = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
  unsigned int normalAccId = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
  unsigned int texAccId = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
  unsigned int indAccId = JSON["meshes"][indMesh]["primitives"][0]["indices"];
  
  std::vector<float> posVec = getFloats(JSON["accessors"][posAccId]);
  std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
  std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccId]);
  std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
  std::vector<float> texVec = getFloats(JSON["accessors"][texAccId]);
  std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);
  
  std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
  std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccId]);
  std::vector<Texture> textures = getTextures();
  
  // for (auto item: vertices)
  // {
  //   std::cout << "Index: " << item.position.x << std::endl;
  //   std::cout << "Index: " << item.position.y << std::endl;
  //   std::cout << "Index: " << item.position.z << std::endl;
  //   break;
  // }
  meshes.push_back(Mesh(vertices, indices, textures));
}


void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
  nlohmann::json node = JSON["nodes"][nextNode];
  
  glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
  if (node.find("translation") != node.end())
  {
    float transValues[3];
    for (unsigned int i = 0; i < node["translation"].size(); i++)
    {
      transValues[i] = (node["translation"][i]);
    }
    translation = glm::make_vec3(transValues);
  }
  
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  if (node.find("rotation") != node.end())
  {
    float rotValues[4] = 
    {
      node["rotation"][3],
      node["rotation"][0],
      node["rotation"][1],
      node["rotation"][2],
    };
    rotation = glm::make_quat(rotValues);
  }
  
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
  if (node.find("scale") != node.end())
  {
    float scaleValues[3];
    for (unsigned int i = 0; i < node["scale"].size(); i++)
    {
      scaleValues[i] = (node["scale"][i]);
    }
    scale = glm::make_vec3(scaleValues);
  }
  
  glm::mat4 matNode = glm::mat4(1.0f);
  if (node.find("matrix") != node.end())
  {
    float matValues[16];
    for (unsigned int i = 0; i < node["matrix"].size(); i++)
    {
      matValues[i] = (node["matrix"][i]);
    }
    matNode = glm::make_mat4(matValues);
  }
  
  glm::mat4 trans = glm::mat4(1.0f);
  glm::mat4 rot = glm::mat4(1.0f);
  glm::mat4 sca = glm::mat4(1.0f);
  
  trans = glm::translate(trans, translation);
  rot = glm::mat4_cast(rotation);
  sca = glm::scale(sca, scale);
  
  glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;
  
  if (node.find("mesh") != node.end())
  {
    translationsMeshes.push_back(translation);
    rotationsMeshes.push_back(rotation);
    scalesMeshes.push_back(scale);
    matricesMeshes.push_back(matNextNode);
    
    loadMesh(node["mesh"]);
  }
  
  if (node.find("children") != node.end())
  {
    for (unsigned int i = 0; i < node["children"].size(); i++)
    {
      traverseNode(node["children"][i], matNextNode);
    }
  }
}


std::vector<unsigned char> Model::getData()
{
  std::string bytesText;
  std::string uri = JSON["buffers"][0]["uri"];
  
  std::string fileStr = std::string(file);
  std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
  bytesText = get_file_contents((fileDirectory + uri).c_str());
  
  std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
  return data;
}


std::vector<float> Model::getFloats(nlohmann::json accessor)
{
  std::vector<float> floatVec;
  
  unsigned int buffViewInd = accessor.value("bufferView", 1);
  unsigned int count = accessor["count"];
  unsigned int accByteOffset = accessor.value("byteOffset", 0);
  std::string type = accessor["type"];
  
  nlohmann::json bufferView = JSON["bufferViews"][buffViewInd];
  unsigned int byteOffset = bufferView["byteOffset"];
  
  unsigned int numPerVert;
  if (type == "SCALAR") numPerVert = 1;
  else if (type == "VEC2") numPerVert = 2; 
  else if (type == "VEC3") numPerVert = 3;
  else if (type == "VEC4") numPerVert = 4;
  else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");
  
  unsigned int beginningOfData = byteOffset + accByteOffset;
  unsigned int lengthOfData = count * 4 * numPerVert; // 4 is the size of float
  for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i)
  {
    unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
    float value;
    std::memcpy(&value, bytes, sizeof(float));
    floatVec.push_back(value);
  }
  
  return floatVec;
}


std::vector<GLuint> Model::getIndices(nlohmann::json accessor)
{
  std::vector<GLuint> indices;
  
  unsigned int buffViewInd = accessor.value("bufferView", 1);
  unsigned int count = accessor["count"];
  unsigned int accByteOffset = accessor.value("byteOffset", 0);
  unsigned int componentType = accessor["componentType"];
  
  nlohmann::json bufferView = JSON["bufferViews"][buffViewInd];
  unsigned int byteOffset = bufferView["byteOffset"];
  
  unsigned int beginningOfData = byteOffset + accByteOffset;
  // unsigned int is 4 bytes size
  if (componentType == 5125)
  {
    for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i) 
    {
      unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
      unsigned int value;
      std::memcpy(&value, bytes, sizeof(unsigned int));
      indices.push_back(value);
    }
  }
  else if (componentType == 5123)
  {
    for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
    {
      unsigned char bytes[] = { data[i++], data[i++] };
      unsigned short value;
      std::memcpy(&value, bytes, sizeof(unsigned short));
      indices.push_back((GLuint)value);
    }
  }
  else if (componentType == 5122)
  {
    for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
    {
      unsigned char bytes[] = { data[i++], data[i++] };
      short value;
      std::memcpy(&value, bytes, sizeof(unsigned short));
      indices.push_back((GLuint)value);
    }
  }
  
  return indices;
}


std::vector<Texture> Model::getTextures()
{
  std::vector<Texture> textures;
  
  std::string fileStr = std::string(file);
  std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
  
  for (unsigned int i = 0; i < JSON["images"].size(); i++)
  {
    std::string texPath = JSON["images"][i]["uri"];
    
    bool skip = false;
    for (unsigned int j = 0; j < loadedTexName.size(); j++)
    {
      if (loadedTexName[j] == texPath)
      {
        skip = true;
        break;
      }
    }
    
    if (skip) continue;
    
    if (texPath.find("baseColor") != std::string::npos)
    {
      Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size());
      textures.push_back(diffuse);
      loadedTex.push_back(diffuse);
      loadedTexName.push_back(texPath);
    }
    else if (texPath.find("metallicRoughness") != std::string::npos)
    {
      Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTex.size());
      textures.push_back(specular);
      loadedTex.push_back(specular);
      loadedTexName.push_back(texPath);
    }
  }
  
  return textures;
}


std::vector<Vertex> Model::assembleVertices
(
    std::vector<glm::vec3> postiions,
    std::vector<glm::vec3> normals,  
    std::vector<glm::vec2> texUVs
)
{
  std::vector<Vertex> vertices;
  for (int i = 0; i < postiions.size(); i++)
  {
    vertices.push_back(
      Vertex
      {
        postiions[i],
        normals[i],
        glm::vec3(1.0f, 1.0f, 1.0f),
        texUVs[i]
      }
    );
  }
  return vertices;
}


std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
  std::vector<glm::vec2> vectors;
  for (int i = 0; i < floatVec.size(); i)
  {
    vectors.push_back( glm::vec2(floatVec[i++], floatVec[i++]) );
  }
  return vectors;
}


std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
  std::vector<glm::vec3> vectors;
  for (int i = 0; i < floatVec.size(); i)
  {
    vectors.push_back( glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]) );
  }
  return vectors;
}


std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
  std::vector<glm::vec4> vectors;
  for (int i = 0; i < floatVec.size(); i)
  {
    vectors.push_back( glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]) );
  }
  return vectors;
}