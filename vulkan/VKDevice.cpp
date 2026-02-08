#include "VKDevice.h"
#include "VKCommandEncoder.h"


using namespace mrhi;

extern GraphicsDevice* g_graphicsDevice;

/// @brief A debug callback used to report messages from the validation layers. See instance creation for details on how this is set up
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data)
{
	(void)user_data;

	if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		MGP_ERROR("%d Validation Layer: Error: %s: %s\n", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
	}
	else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		MGP_ERROR("%d Validation Layer: Warning: %s: %s\n", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
	}
	else if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
	{
		MGP_ERROR("%d Validation Layer: Performance warning: %s: %s\n", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
	}
	else
	{
		MGP_ERROR("%d Validation Layer: Information: %s: %s\n", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
	}
	return VK_FALSE;
}

/**
 * @brief Validates a list of required extensions, comparing it with the available ones.
 *
 * @param required A vector containing required extension names.
 * @param available A VkExtensionProperties object containing available extensions.
 * @return true if all required extensions are available
 * @return false otherwise
 */
bool validate_extensions(const std::vector<const char*>& required,
	const std::vector<VkExtensionProperties>& available)
{
	for (auto extension : required)
	{
		bool found = false;
		for (auto& available_extension : available)
		{
			if (strcmp(available_extension.extensionName, extension) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	return true;
}

void getRequiredExtensions(std::vector<const char*>& requiredExtensions, std::vector<const char*>& requested_instance_layers, bool isDebug) {
	uint32_t instance_extension_count;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr));

	std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data()));

	std::vector<const char*>& required_instance_extensions = requiredExtensions;

	// Validation layers help finding wrong api usage, we enable them when explicitly requested or in debug builds
	// For this we use the debug utils extension if it is supported
	bool has_debug_utils = false;
	if (isDebug) {
		for (const auto& ext : available_instance_extensions)
		{
			if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
			{
				has_debug_utils = true;
				required_instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				break;
			}
		}
		if (!has_debug_utils)
		{
			printf("%s not supported or available\n", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			printf("Make sure to compile the sample in debug mode and/or enable the validation layers\n");
		}
	}

#if (defined(VKB_ENABLE_PORTABILITY))
	required_instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	bool portability_enumeration_available = false;
	if (std::ranges::any_of(available_instance_extensions,
		[](VkExtensionProperties const& extension) { return strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0; }))
	{
		required_instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		portability_enumeration_available = true;
	}
#endif

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	required_instance_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	required_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
	required_instance_extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	required_instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	required_instance_extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	required_instance_extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
	required_instance_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#else
#	pragma error Platform not supported
#endif

	if (!validate_extensions(required_instance_extensions, available_instance_extensions))
	{
		MGP_ERROR("Required instance extensions are missing.\n");
	}

	if (isDebug) {
		char const* validationLayer = "VK_LAYER_KHRONOS_validation";

		uint32_t instance_layer_count;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

		std::vector<VkLayerProperties> supported_instance_layers(instance_layer_count);
		VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_instance_layers.data()));

		bool foundValidationLayer = false;
		for (auto& lp : supported_instance_layers) {
			if (strcmp(lp.layerName, validationLayer) == 0) {
				requested_instance_layers.push_back(validationLayer);
				printf("Enabled Validation Layer %s\n", validationLayer);
				foundValidationLayer = true;
				break;
			}
		}
		if (!foundValidationLayer) {
			printf("Validation Layer %s is not available\n", validationLayer);
		}

	}
}

bool VKDevice::init_instance(std::vector<const char*>& requiredExtensions, bool isDebug)
{
	//LOGI("Initializing vulkan instance.");

	if (volkInitialize())
	{
		MGP_ERROR("Failed to initialize volk.\n");
	}

	std::vector<const char*> requested_instance_layers{};
	getRequiredExtensions(requiredExtensions, requested_instance_layers, isDebug);

	VkApplicationInfo app{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Hello Triangle",
		.pEngineName = "Vulkan Samples",
		#if 1
		.apiVersion = VK_API_VERSION_1_3,
		#else
		.apiVersion = VK_API_VERSION_1_1,
		#endif
	};

	VkInstanceCreateInfo instance_info{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
		.enabledLayerCount = static_cast<uint32_t>(requested_instance_layers.size()),
		.ppEnabledLayerNames = requested_instance_layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data() };


	// Validation layers help finding wrong api usage, we enable them when explicitly requested or in debug builds
	// For this we use the debug utils extension if it is supported
	VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	if (isDebug)
	{
		debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debug_utils_create_info.pfnUserCallback = debug_callback;

		instance_info.pNext = &debug_utils_create_info;
	}

#if (defined(VKB_ENABLE_PORTABILITY))
	if (portability_enumeration_available)
	{
		instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	}
#endif

	// Create the Vulkan instance
	VK_CHECK(vkCreateInstance(&instance_info, nullptr, &this->instance));

	volkLoadInstance(this->instance);

	if (isDebug)
	{
		VK_CHECK(vkCreateDebugUtilsMessengerEXT(this->instance, &debug_utils_create_info, nullptr, &debug_callback_obj));
	}
	return true;
}

bool VKDevice::init_device(VkSurfaceKHR surface)
{
	//LOGI("Initializing vulkan device.");

	uint32_t gpu_count = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &gpu_count, nullptr));

	if (gpu_count < 1)
	{
		MGP_ERROR("No physical device found.\n");
		return false;
	}

	// For simplicity, the sample selects the first gpu that has a graphics and present queue
	std::vector<VkPhysicalDevice> gpus(gpu_count);
	VK_CHECK(vkEnumeratePhysicalDevices(this->instance, &gpu_count, gpus.data()));

	for (size_t i = 0; i < gpu_count && (graphics_queue_index < 0); i++)
	{
		this->gpu = gpus[i];

		uint32_t queue_family_count;
		vkGetPhysicalDeviceQueueFamilyProperties(this->gpu, &queue_family_count, nullptr);

		if (queue_family_count < 1)
		{
			MGP_ERROR("No queue family found.\n");
			return false;
		}

		std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(this->gpu, &queue_family_count, queue_family_properties.data());

		for (uint32_t i = 0; i < queue_family_count; i++)
		{
			VkBool32 supports_present;
			vkGetPhysicalDeviceSurfaceSupportKHR(this->gpu, i, surface, &supports_present);

			// Find a queue family which supports graphics and presentation.
			if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && supports_present)
			{
				this->graphics_queue_index = i;
				break;
			}
		}
	}

	if (this->graphics_queue_index < 0)
	{
		MGP_ERROR("Did not find suitable device with a queue that supports graphics and presentation.");
		return false;
	}

	uint32_t device_extension_count;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(this->gpu, nullptr, &device_extension_count, nullptr));
	std::vector<VkExtensionProperties> device_extensions(device_extension_count);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(this->gpu, nullptr, &device_extension_count, device_extensions.data()));

	// Since this sample has visual output, the device needs to support the swapchain extension
	std::vector<const char*> required_device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	if (!validate_extensions(required_device_extensions, device_extensions))
	{
		MGP_ERROR("Required device extensions are missing.");
		return false;
	}

#if (defined(VKB_ENABLE_PORTABILITY))
	// VK_KHR_portability_subset must be enabled if present in the implementation (e.g on macOS/iOS using MoltenVK with beta extensions enabled)
	if (std::ranges::any_of(device_extensions,
		[](VkExtensionProperties const& extension) { return strcmp(extension.extensionName, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0; }))
	{
		required_device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
	}
#endif

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	// The sample uses a single graphics queue
	const float queue_priority = 0.5f;

	VkDeviceQueueCreateInfo queue_info{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = static_cast<uint32_t>(this->graphics_queue_index),
		.queueCount = 1,
		.pQueuePriorities = &queue_priority };

	VkDeviceCreateInfo device_info{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_info,
		.enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size()),
		.ppEnabledExtensionNames = required_device_extensions.data(),
		.pEnabledFeatures = &deviceFeatures,
	};

	VK_CHECK(vkCreateDevice(this->gpu, &device_info, nullptr, &this->device));
	volkLoadDevice(this->device);

	vkGetDeviceQueue(this->device, this->graphics_queue_index, 0, &this->queue);

	// This sample uses the Vulkan Memory Alloctor (VMA), which needs to be set up
	VmaVulkanFunctions vma_vulkan_func{
		.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
		.vkGetDeviceProcAddr = vkGetDeviceProcAddr };

	VmaAllocatorCreateInfo allocator_info{
		.physicalDevice = this->gpu,
		.device = this->device,
		.pVulkanFunctions = &vma_vulkan_func,
		.instance = this->instance };

	VkResult result = vmaCreateAllocator(&allocator_info, &this->vma_allocator);
	if (result != VK_SUCCESS)
	{
		MGP_ERROR("Could not create allocator for VMA allocator");
		return false;
	}
	return true;
}

void VKDevice::createDescriptorPool() {
	uint32_t size = 16;
	std::vector<VkDescriptorPoolSize> poolSizes = {
		{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = size },
		{.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, .descriptorCount = size },
		{.type = VK_DESCRIPTOR_TYPE_SAMPLER, .descriptorCount = size },
		{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = size },
		{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = size },
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(size);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		MGP_ERROR("failed to create descriptor pool!");
	}
}

VkInstance VKDevice::initInstance(std::vector<const char*>& requiredExtensions, bool isDebug) {
	init_instance(requiredExtensions, isDebug);
	return instance;
}
void VKDevice::initDevice(VkSurfaceKHR surface) {
	if (!surface)
	{
		MGP_ERROR("Failed to create window surface.\n");
	}

	init_device(surface);

	//VkFenceCreateInfo info{
	//.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	//.flags = VK_FENCE_CREATE_SIGNALED_BIT };
	//VK_CHECK(vkCreateFence(device, &info, nullptr, &queue_submit_fence));

	VkCommandPoolCreateInfo cmd_pool_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		//.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = static_cast<uint32_t>(this->graphics_queue_index) };
	VK_CHECK(vkCreateCommandPool(device, &cmd_pool_info, nullptr, &primary_command_pool));

	createDescriptorPool();

	//this->window = window;
	g_graphicsDevice = this;
}

VKDevice::~VKDevice() {

    if (g_graphicsDevice == this) {
        g_graphicsDevice = nullptr;
    }
	// When destroying the application, we need to make sure the GPU is no longer accessing any resources
	// This is done by doing a device wait idle, which blocks until the GPU signals
	if (device != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(device);
	}

	if (descriptorPool) {
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}

	/*if (queue_submit_fence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device, queue_submit_fence, nullptr);

		queue_submit_fence = VK_NULL_HANDLE;
	}*/

	if (primary_command_pool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device, primary_command_pool, nullptr);

		primary_command_pool = VK_NULL_HANDLE;
	}

	if (vma_allocator != VK_NULL_HANDLE)
	{
		vmaDestroyAllocator(vma_allocator);
	}

	if (device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(device, nullptr);
	}

	if (debug_callback_obj != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(instance, debug_callback_obj, nullptr);
	}

	vkDestroyInstance(instance, nullptr);
}

void VKDevice::waitIdle() {
	if (device != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(device);
	}
}

Surface* VKDevice::createSurface(const SurfaceDesc& d) {
    VKSurface* fbo = new VKSurface();
    fbo->init(this, &d);
    return fbo;
}

Pipeline* VKDevice::createPipeline(const PipelineDesc& d) {
	VKPipeline* t = new VKPipeline();
	t->init(this, &d);
	return t;
}

Texture* VKDevice::createTexture(const TextureDesc& d) {
    VKTexture* t = new VKTexture();
    t->init(this, &d);
    return t;
}

Buffer* VKDevice::createBuffer(const BufferDesc& d) {
    VKBuffer* t = new VKBuffer();
    t->init(this, d);
    return t;
}

CommandEncoder* VKDevice::createCommandEncoder(const CommandEncoderDesc& d) {
    VKCommandEncoder* t = new VKCommandEncoder();
    t->init(this, &d);
    return t;
}

Shader* VKDevice::createShader(const ShaderDesc& d) {
    return VKShader::create(this, &d);
}

Sampler* VKDevice::createSampler(const SamplerDesc& d) {
    VKSampler* t = new VKSampler();
    t->init(this, &d);
    return t;
}

BindingGroup* VKDevice::createBindingGroup(const BindingGroupDesc& d) {
    VKBindingGroup* t = new VKBindingGroup();
    t->init(this, &d);
    return t;
}

FrameBuffer* mrhi::VKDevice::createFrameBuffer(const RenderPassDesc& desc)
{
	VKFrameBuffer* t = new VKFrameBuffer();
	t->init(this, desc);
	return t;
}
