#ifndef VULKANRENDERER_H 
#define VULKANRENDERER_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_image.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <unordered_map>
#include <array>
#include <chrono>

#include "Vector.h"
#include "VMath.h"
#include "MMath.h"
#include "Hash.h"
#include "GlobalLight.h"

class Actor;
struct Sampler2D;
//#include "Actor.h"

using namespace MATH;


#include "Renderer.h"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG /// only use validation layers if in debug mode
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = false;
#endif

struct QueueFamilyIndices {
    /// optional means that it contains no value until it is assigned
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

    struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

    struct Vertex {
        Vec3 pos;
        Vec3 normal;
        Vec2 texCoord;

        //Static function for making a input binding for shaders - this sets up the point to bind in the shader
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{}; //Create struct
            bindingDescription.binding = 0; //binding point
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //Do it per vertex basis
            return bindingDescription;
        }

        //Static function for making a input attribute for shaders - this puts the info into the shader
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0; //Use the binding point from the other static function
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; //Vulkan format for floating point vec3 - 3 floating point values
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT; //Vulkan format for floating point vec2 
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
        bool operator == (const Vertex& other) const { //check if another vertex is the same - there is a lot of ways a vertex can be the same as another(mag,direction), so I did the overload here and not the vertex class
            return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
        }
        
    }; /// End of struct Vertex


    namespace std {
        template<> struct hash<Vertex> {
            size_t operator()(Vertex const& vertex) const noexcept { //overload the () 
                size_t hash1 = hash<Vec3>()(vertex.pos); //create a hash of the vertex pos
                size_t hash2 = hash<Vec3>()(vertex.normal);
                size_t hash3 = hash<Vec2>()(vertex.texCoord);
                size_t result = ((hash1 ^ (hash2 << 1)) >> 1) ^ (hash3 << 1); //teacher could not explain this code
                return result;
            }
        };
    }

struct Pipeline {
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipelineID;
};

struct BufferHandle {
    //Create a BufferHandle - a BufferHandle has the buffer struct and device memory
    //A VkBuffer is the handle to the buffer - its a ID
    //a VkDeviceMemory is the buffer memory with all the info of the buffer
    VkBuffer bufferID;
    VkDeviceMemory bufferMemoryID;
};

 
struct CameraUBO {
    Matrix4 view;
    Matrix4 proj;
    Vec4 normalColour;
    float normalLength;
};

//struct GlobalLightUBO {
//    Vec4 lightPos[4];
//    Vec4 lightColour[4];
//};

class VulkanRenderer : public Renderer {
public:
    /// C11 precautions - delete the auto copier and other things
    VulkanRenderer(const VulkanRenderer&) = delete;  /// Copy constructor
    VulkanRenderer(VulkanRenderer&&) = delete;       /// Move constructor
    VulkanRenderer& operator=(const VulkanRenderer&) = delete; /// Copy operator
    VulkanRenderer& operator=(VulkanRenderer&&) = delete;      /// Move operator

    VulkanRenderer();
    ~VulkanRenderer();
    SDL_Window* CreateWindow(std::string name_, int width, int height);
    bool OnCreate();
    void OnDestroy();
    void Render();
    void SetCameraUBO(const Matrix4& projection, const Matrix4& view);

    //void SetGlobalLightUBO(const Vec4 lightArray[4], const Vec4 colourArray[4]);
    void SetGlobalLightUBO(const GlobalLightUBO& gLights);

    void SetMeshPushConstant(const Matrix4& model);

    void createTextureImage(std::string filename_, Sampler2D& textureImageView);

    template<typename UniformBuffer> void updateUniformBuffers(UniformBuffer memoryObject, uint32_t currentImage, std::vector<BufferHandle> buffer, size_t bufferSize) {
        void* data; //set a void pointer to store the data location inside the gpu - void pointer is a pointer to anything
        //uniformBuffersMemory[currentImage] stores the current image(buffer) that we want to work on
        vkMapMemory(device, buffer[currentImage].bufferMemoryID, 0, bufferSize, 0, &data); //get the data location inside the gpu to put the ubo - its in a safe memory location inside the gpu
        //&data gets a pointer of a pointer (address of a pointer) - vkMapMemory could have returned a number, but it puts the address into the data variable
        memcpy(data, &memoryObject, bufferSize); //copys the memory of the ubo into the data location - memcpy(destination, address of structure, size of structure)
        vkUnmapMemory(device, buffer[currentImage].bufferMemoryID); //give the data location back - the gpu can now use the memory
    }

    SDL_Window* GetWindow() {
        return window;
    }

    std::unordered_map<std::string, Actor> actorGraph;

private:
    CameraUBO cameraUBO;
    GlobalLightUBO globalLightUBO;

    const size_t MAX_FRAMES_IN_FLIGHT = 2; //double buffering

    SDL_Event sdlEvent;
    uint32_t windowWidth;
    uint32_t windowHeight;
    SDL_Window* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;

    std::unordered_map<std::string, Pipeline> pipelineGraph;
    //VkPipeline graphicsPipelineID; //maybe make a unordered map of them

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkCommandPool commandPool;

    //make these std::unordered_maps
    //BufferHandle vertexBuffer;
    //BufferHandle indexBuffer;
    //VkDeviceSize indexBufferSize;
    //IndexedBufferHandle modelBuffer;

    //std::unordered_map<std::string, BufferHandle> UniformBuffers;

    std::vector<BufferHandle> cameraBuffers;
    std::vector<BufferHandle> globalLightBuffers;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    bool hasStencilComponent(VkFormat format);

    void initVulkan();
    void createInstance();
    void createSurface();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void recreateSwapChain();
    //void updateUniformBuffer(uint32_t currentImage);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline(Pipeline& pipelineRef, const char* vFilename, const char* fragFilename, const char* geoFilename);
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
    //void createTextureImage(std::string filename_, Sampler2D& textureImageView);
    void createTextureImageView(Sampler2D& textureImageView);
    void createTextureSampler(Sampler2D& sampler);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void loadModel(std::string filename_, BufferHandle& vertexBuffer, BufferHandle& indexBuffer, VkDeviceSize& indexBufferSize);
    void createVertexBuffer(BufferHandle& vertexBuffer, std::vector<Vertex> vertices);
        /// A helper function for createVertexBuffer()
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createIndexBuffer(BufferHandle& indexBuffer, std::vector<uint32_t> indices, VkDeviceSize& indexBufferSize);
    void createUniformBuffers(VkDeviceSize bufferSize, std::vector<BufferHandle>& uniformBuffer);
    void createDescriptorPool();

    void createDescriptorSets();
    void updateDescriptorSets(Sampler2D& sampler);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, BufferHandle& buffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createCommandBuffers();
    void updateCommandBuffers();

    void createSyncObjects();
    void cleanup();
    void cleanupSwapChain();
    void cleanupUniformBuffer(std::vector<BufferHandle>& uniformBuffer);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void  populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);


    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    static std::vector<char> readFile(const std::string& filename);

};
#endif 

