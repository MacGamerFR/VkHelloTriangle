#ifndef RENDER_HPP
#define RENDER_HPP

#include <vector>
#include <iostream>
#include <optional>
#include <string>
#include <functional>
#include <cassert>

#include "Vulkan.hpp"

#include "RenderPipeline.hpp"
#include "RenderTarget.hpp"
#include "RenderUtils.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

/* used as callback function to delegate surface creation */
using createSurfaceFoncter = std::function<vk::SurfaceKHR(vk::Instance, vk::DispatchLoaderDynamic)>;


class Render {
public:
	Render(std::string appName = "", uint32_t appVersion = 0, RenderType type = RenderTypeBits::eGraphics);
	~Render();

	void init();
	void cleanup();

	void drawFrame();

	void addLayer(const char*);
	void addInstanceExtension(const char*);
	void addDeviceExtension(const char*);

	void enableValidationLayer();

	void setRenderType(RenderType);

	void setSurfaceCreationFunction(createSurfaceFoncter);

	void setExtent(const vk::Extent2D&);

	void setAppName(std::string name) { appName = name; }
	void setAppVersion(uint32_t version) { appVersion = version; }

	uint32_t addPipeline(RenderPipeline& pipeline) {
		uint32_t index = pipelines.size();
		pipeline.device = device;
		pipeline.deviceLoader = deviceLoader;
		pipelines.push_back(&pipeline);
		return index;
	}

	/**
	 * Create Vulkan resources with infos provided by the pipeline
	 */
	void finishSetup();

	/**
	 * Create a one-time-submit CommandBuffer
	 */
	vk::CommandBuffer beginDrawCommands();

	/**
	 * End the CommandBuffer and submit it
	 */
	void endDrawCommands(vk::CommandBuffer);

	void submitDrawCommands(vk::CommandBuffer);

	/**
	 * Create a buffer of size bytes and return its index
	 * If writeFromHost is true, you can call fillBuffer on it later
	 * If readFromHost if true, you can call getDataFomBuffer on it later
	 */
	unsigned int addBuffer(uint64_t size, vk::BufferUsageFlags usage, bool writeFromHost = true, bool readFromHost = false);

	/**
	 * Fill a buffer
	 */
	void fillBuffer(unsigned int bufferIndex, const void* data, uint64_t dataSize);

	template<typename T> void fillBuffer(unsigned int bufferIndex, const T& data) {
		fillBuffer(bufferIndex, &data, sizeof(data));
	}

	template<typename T> void fillBuffer(unsigned int bufferIndex, std::vector<T> data) {
		fillBuffer(bufferIndex, data.data(), data.size() * sizeof(data[0]));
	}

	/**
	 * Add a uniform buffer before init time
	 */
/*	unsigned int addUniform(uint64_t uniformSize, vk::ShaderStageFlags stageFlags) {
		unsigned int uniformIndex = uniformSizes.size();
		uniformSizes.push_back(uniformSize);
		uniformStages.push_back(stageFlags);
		return uniformIndex;
	}
*/
	/**
	 * Update uniform data while rendering
	 */
/*	void updateUniform(unsigned int uniformIndex, const void* data, uint64_t dataSize) {
		assert(uniformIndex < uniformSizes.size());
		assert(dataSize <= uniformSizes[uniformIndex]);

		// TODO update only the next uniform used for rendering if possible (cf. vkAcquireNextImageKHR)
		// check wether the buffer is ready to be updated (no longer used)
		for (unsigned int i = 0; i < swapchain.getImageCount(); i++)
			if (device.getEventStatus(uniformEvent[i], deviceLoader) == vk::Result::eEventSet) {
				::fillBuffer(device, uniformBuffersMemory[uniformIndex * swapchain.getImageCount() + i], data, dataSize, deviceLoader);
				device.resetEvent(uniformEvent[i], deviceLoader);
			}
	}
*/
	/**
	 * Get data from a buffer
	 */
	template<typename T>
	std::vector<T> getDataFromBuffer(unsigned int index, uint64_t elementCount){
		assert(index < buffers.size());
		uint64_t dataSize = elementCount * sizeof(T);

		// Create staging buffer
		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		createBuffer(dataSize, vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
		// Copy buffer data from device to host accessbile buffer
		copyBuffer(buffers[index], stagingBuffer, dataSize);

		// Get data back
		auto data = static_cast<T*>(device.mapMemory(stagingBufferMemory, /*offset*/ 0, dataSize, vk::MemoryMapFlags(), deviceLoader));
		std::vector<T> res(data, data + elementCount);
		device.unmapMemory(stagingBufferMemory, deviceLoader);

		// Destroy staging buffer
		device.freeMemory(stagingBufferMemory, nullptr, deviceLoader);
		device.destroyBuffer(stagingBuffer, nullptr, deviceLoader);
		return res;
	}

	const vk::DispatchLoaderDynamic& getDeviceLoader() { return deviceLoader; }

	vk::Buffer getBuffer(uint32_t bufferIndex) {
		return buffers[bufferIndex];
	}

	vk::RenderPassBeginInfo getRenderPassInfo(const RenderPipeline& pipeline) {
		vk::RenderPassBeginInfo renderPassInfo;
		renderPassInfo.renderPass = pipeline.renderPass;
		renderPassInfo.framebuffer = swapchain.getFramebuffer(currentFrame);
		renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
		renderPassInfo.renderArea.extent = swapchain.getExtent();
		std::array<vk::ClearValue, 2> clearColors;
		clearColors[0].color = vk::ClearColorValue(std::array<float, 4>({0.2f, 0.2f, 0.2f, 1.f}));
		clearColors[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);
		renderPassInfo.clearValueCount = clearColors.size();
		renderPassInfo.pClearValues = clearColors.data();
		return renderPassInfo;
	}

	void bindPipeline(vk::CommandBuffer& commandBuffer, const RenderPipeline& pipeline) {
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline, deviceLoader);
	}

	void bindDescritpor(vk::CommandBuffer& commandBuffer, const RenderPipeline& pipeline) {
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.pipelineLayout, 0u, pipeline.descriptorSets[currentFrame], nullptr, deviceLoader);
	}

	uint32_t addTexture(const std::string& filename) { imageFilenames.push_back(filename); return imageFilenames.size()-1; }

	void waitForFences() { device.waitForFences(inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max(), deviceLoader); }

	void waitDeviceIdle() { device.waitIdle(deviceLoader); }

protected:

	VulkanLoader loader;

	std::string appName;

	uint32_t appVersion;

	RenderType renderType;

	bool validationLayerEnabled;
	std::vector<const char*> layers;
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> deviceExtensions;

	createSurfaceFoncter surfaceCreation;

	vk::Extent2D windowExtent;

	vk::Instance instance;
	vk::DispatchLoaderDynamic instanceLoader;

	vk::DebugUtilsMessengerEXT callback;

	vk::PhysicalDevice physicalDevice;

	vk::Device device;
	vk::DispatchLoaderDynamic deviceLoader;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	RenderTarget<vk::DispatchLoaderDynamic> swapchain;

	vk::Queue graphicsQueue;
	vk::Queue computeQueue;

	vk::SurfaceKHR surface;

	vk::Queue presentQueue;

	std::vector<RenderPipeline*> pipelines;

//	Texture depthBuffer;

//	vk::RenderPass renderPass;
//	vk::DescriptorSetLayout descriptorSetLayout;
//	vk::PipelineLayout pipelineLayout;

//	vk::Pipeline graphicsPipeline;

//	vk::DescriptorPool descriptorPool;
//	std::vector<vk::DescriptorSet> descriptorSets;

	vk::CommandPool commandPool;

	std::vector<std::string> imageFilenames;
	std::vector<Texture>     textures;

	vk::Sampler textureSampler;

	std::vector<vk::Buffer>           buffers;
	std::vector<vk::DeviceMemory>     buffersMemory;
	std::vector<uint64_t>             bufferSizes;
	std::vector<vk::BufferUsageFlags> bufferUsages;
/*
	std::vector<uint64_t>             uniformSizes;
	std::vector<vk::ShaderStageFlags> uniformStages;
*/	std::vector<vk::Buffer>           uniformBuffers;
	std::vector<vk::DeviceMemory>     uniformBuffersMemory;
/*	std::vector<vk::Event>            uniformEvent;
*/
	std::vector<vk::CommandBuffer> commandBuffers;

	/* Semaphores sync GPU's operations */
	std::vector<vk::Semaphore> imageAvailableSemaphores;
	std::vector<vk::Semaphore> renderFinishedSemaphores;
	/* Fences sync GPU with CPU */
	std::vector<vk::Fence> inFlightFences;
	size_t currentFrame = 0;

	// initialising functions

	void createInstance();

	void setupDebugCallback();

	void pickPhysicalDevice();

	void createLogicalDevice();

	void createSurface();

	void createSwapchain(vk::SwapchainKHR = nullptr);

//	void createRenderPass();

//	void createDescriptorSetLayout();

//	void createDescriptorPool();

//	void createDescriptorSets();

//	void createGraphicsPipeline();

	void createFramebuffers();

	void createCommandPool();

	void createTextureImages();

	void createTextureImageViews();

	void createTextureSampler(float maxAnisotropy, float maxMipLod);

	Texture createDepthResources(uint32_t width, uint32_t height);

	void createBuffer(vk::DeviceSize, vk::BufferUsageFlags, vk::MemoryPropertyFlags, vk::Buffer&, vk::DeviceMemory&);

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, vk::Format, vk::ImageTiling, vk::ImageUsageFlags, vk::MemoryPropertyFlags, vk::Image&, vk::DeviceMemory&);

	void transitionImageLayout(vk::Image, vk::Format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t mipLevels);

	void createBuffers();

//	void createUniformBuffers();

	void createPipelineUniformBuffers(const RenderPipeline&);

	void createUniformBuffer(vk::DeviceSize bufferSize, vk::Buffer& buffer, vk::DeviceMemory& memory);

	void updateUniformBuffer(uint32_t uniformIndex, uint32_t imageIndex);

	void copyBuffer(vk::Buffer, vk::Buffer, vk::DeviceSize);

	void copyBufferToImage(vk::Buffer, vk::Image, uint32_t w, uint32_t h);

	void generateMipmaps(vk::Image, vk::Format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	vk::CommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(vk::CommandBuffer);

	void createCommandBuffers();

	void cleanupSwapchain();

	void recreateSwapChain();

	void createSemaphores();

	void createFences();
};

#endif
