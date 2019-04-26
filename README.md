# Learning Vulkan through vulkan-tutorial.com and Intel Vulkan tutorial 

I'm currently learning the API by reeding tutorials and experimenting things.
My goal is to have a general comprehension of the API, and to learn commons graphics technics.

## TODO LIST

* Library loading
	* load extension functions only if activated at Instance/Device creation time

* Renderer
	* Rendering technics
		* Shadow maps
		* SSR
		* SSAO
		* PBR
		* SSSSS
		* SMAA

## How to build
### Dependencies

* Vulkan SDK >= 1.1.73 : [Download LunarG's SDK](https://vulkan.lunarg.com) or install vulkan packages for your Linux ditribution.
* GLFW >= 3.2.1 : [Download and compile GLFW](https://www.glfw.org) or install GLFW dev and regular packages.
* GLM >= 0.9.7 : [Download GLM](https://glm.g-truc.net) or install GLM packages.

### Generating workspace

I use premake 5 to generate build files used by tools like Microsoft Visual Studio or GNU Make.
--> [Premake Wiki](https://github.com/premake/premake-core/wiki/Using-Premake)

Simply run `premake5 [action]` in the root directory (premake5.exe for Windows, and premake5 for Linux).
It will create a "build" directory where you will find all the project files needed to build the lib and examples.

## Shipped Libraries

* STB_image
* Tiny_Obj_Loader