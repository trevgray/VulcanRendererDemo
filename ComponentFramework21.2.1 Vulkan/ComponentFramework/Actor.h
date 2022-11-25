#pragma once
#include "Matrix.h"
#include "VulkanRenderer.h"

struct MeshPushConstants {
    MATH::Matrix4 model;
    MATH::Matrix4 normal; //it could be a matrix3 because it just rotation - but we should keep it as mat4 because its a multiple of 4
};

struct Sampler2D {
    //Make a BufferHandle
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    //This is what is passed into the gpu
    VkImageView textureImageView;
    VkSampler textureSampler;
};

class Actor {
public:
    Actor();
    ~Actor();
    MeshPushConstants mesh;
    //
    BufferHandle vertexBuffer;
    BufferHandle indexBuffer;
    VkDeviceSize indexBufferSize;
    //
    //std::vector<VkDescriptorSet> descriptorSets;
    Sampler2D image;
};

