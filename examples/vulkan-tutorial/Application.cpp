#include "Application.hpp"

Application::Application() : window(800, 600, "Vulkan"){

}

Application::~Application() {

}

vk::SurfaceKHR Application::createRenderSurface(vk::Instance instance) {
	return window.createSurface(instance);
}

void Application::init() {
	auto extensions = window.getRequiredExtensions();
	for (auto extensionName : extensions)
		render.addInstanceExtension(extensionName);
	render.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	#if defined(DEBUG)
	render.enableValidationLayer();
	#endif

	render.setParentApplication(this);
	render.setSurfaceCreationFunction(createSurface);

	render.setExtent(windowExtent());

	render.init();
}

vk::Extent2D Application::windowExtent() {
	return {window.getWidth(), window.getHeight()};
}

void Application::mainLoop() {
	while(!window.isClosed()) {
		window.pollEvents();
		render.drawFrame();
	}
	render.waitDeviceIdle();
}

vk::SurfaceKHR createSurface(Application* app, vk::Instance instance) {
	return app->createRenderSurface(instance);
}
