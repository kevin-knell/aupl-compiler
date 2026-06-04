#include "vulkan.hpp"
#include "file.hpp"


namespace auplib {

VulkanInstance vulkan_instance;

void VulkanInstance::create_instance() {
	VkApplicationInfo app_info {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Application",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 0),
		.pEngineName = "Engine",
		.engineVersion = VK_MAKE_VERSION(0, 0, 0),
		.apiVersion = VK_API_VERSION_1_3,
	};

	glfwInit();

	const char* layers[] = {
		"VK_LAYER_KHRONOS_validation"
	};

	uint32_t glfw_extensions_amount = 0;
	const char** glfw_extension_names = glfwGetRequiredInstanceExtensions(&glfw_extensions_amount);

	assert(glfw_extensions_amount > 0);

	std::vector<const char*> extension_names;

	//std::cout << "extensions:" << std::endl;
	for (uint32_t i = 0; i < glfw_extensions_amount; ++i) {
		//std::cout << "\t" << glfw_extension_names[i] << std::endl;
		extension_names.push_back(glfw_extension_names[i]);
	}

	extension_names.push_back("VK_KHR_get_surface_capabilities2");
	//extension_names.push_back("VK_EXT_extended_dynamic_state");

	/*for (auto n : extension_names) {
		std::cout << n << std::endl;
	}*/


	VkInstanceCreateInfo vk_instance_create_info {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 1,
		.ppEnabledLayerNames = layers,
		.enabledExtensionCount = static_cast<uint32_t>(extension_names.size()),
		.ppEnabledExtensionNames = extension_names.data()
	};
	
	VkResult result = vkCreateInstance(&vk_instance_create_info, nullptr, &instance);
	std::cerr << result << std::endl;
	assert(result == VK_SUCCESS);
}

void VulkanInstance::create_device() {
	VkResult result;
	uint32_t amount_physical_devices;
	result = vkEnumeratePhysicalDevices(instance, &amount_physical_devices, nullptr);
	assert(result == VK_SUCCESS);
	
	VkPhysicalDevice* phys_devices = new VkPhysicalDevice[amount_physical_devices];
	result = vkEnumeratePhysicalDevices(instance, &amount_physical_devices, phys_devices);
	assert(result == VK_SUCCESS);

	//std::cout << "physical devices:" << std::endl;
	for (uint32_t i = 0; i < amount_physical_devices; ++i) {
		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(phys_devices[i], &device_properties);
		//std::cout << "\t" << device_properties.deviceName << std::endl;
	}

	phys_device = phys_devices[0];

	delete[] phys_devices;

	// queues
	uint32_t amount_queue_families;
	vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &amount_queue_families, nullptr);
	VkQueueFamilyProperties* vk_queue_family_properties = new VkQueueFamilyProperties[amount_queue_families];
	vkGetPhysicalDeviceQueueFamilyProperties(phys_device, &amount_queue_families, vk_queue_family_properties);

	for (uint32_t i = 0; i < amount_queue_families; ++i) {
		if (vk_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			selected_queue_family_idx = i;
			break;
		}
	}

	float queue_family_priorities = 1.0f;

	VkDeviceQueueCreateInfo device_queue_create_info {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.queueFamilyIndex = selected_queue_family_idx,
		.queueCount = 1,
		.pQueuePriorities = &queue_family_priorities
	};

	VkPhysicalDeviceVulkan12Features enabled_vk12_features{};
	enabled_vk12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	enabled_vk12_features.descriptorIndexing = true;
	enabled_vk12_features.shaderSampledImageArrayNonUniformIndexing = true;
	enabled_vk12_features.descriptorBindingVariableDescriptorCount = true;
	enabled_vk12_features.runtimeDescriptorArray = true;
	enabled_vk12_features.bufferDeviceAddress = true;

	VkPhysicalDeviceVulkan13Features enabled_vk13_features{};
	enabled_vk13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	enabled_vk13_features.pNext = &enabled_vk12_features;
	enabled_vk13_features.synchronization2 = VK_TRUE;
	enabled_vk13_features.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceFeatures enabled_vk10_features{};
	enabled_vk10_features.samplerAnisotropy = VK_TRUE;

	const std::vector<const char*> device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo vk_device_create_info {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &enabled_vk13_features,
		.flags = 0,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &device_queue_create_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size()),
		.ppEnabledExtensionNames = device_extensions.data(),
		.pEnabledFeatures = &enabled_vk10_features
	};

	result = vkCreateDevice(phys_device, &vk_device_create_info, nullptr, &device);
	assert(result == VK_SUCCESS);

	vkGetDeviceQueue(device, selected_queue_family_idx, 0, &queue);
	assert(result == VK_SUCCESS);
}

uint32_t VulkanInstance::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(phys_device, &memProps);

	for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProps.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("No suitable memory type");
}

VulkanInstance::VulkanInstance() {
	create_instance();
	create_device();
}

}