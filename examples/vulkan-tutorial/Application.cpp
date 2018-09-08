#include "Application.hpp"

Application::Application() : window(800, 600, "Vulkan"){

}

Application::~Application() {

}

void Application::init() {
	auto extensions = window.getRequiredExtensions();
	for (auto extensionName : extensions)
		render.addExtension(extensionName);
	#if defined(DEBUG)
	render.enableValidationLayer();
	#endif

	render.init();
}

void Application::mainLoop() {
	while(!window.isClosed()) {
		window.pollEvents();
	}
}
