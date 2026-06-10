#include "canvas_item.hpp"

#include "vulkan.hpp"

namespace auplib {

void CanvasItem::init(VkCommandPool cmd_pool) {
    VkDevice device = vulkan_instance.device;

    // -----------------------------
    // 1. Create uniform buffer
    // -----------------------------
    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = sizeof(ObjectUniformData),
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr
    };

    vkCreateBuffer(device, &bufferCreateInfo, nullptr, &object_uniform_buffer);

    // -----------------------------
    // 2. Allocate memory
    // -----------------------------
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, object_uniform_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = vulkan_instance.findMemoryType(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        )
    };

    vkAllocateMemory(device, &allocInfo, nullptr, &object_uniform_memory);

    // -----------------------------
    // 3. Bind buffer memory
    // -----------------------------
    vkBindBufferMemory(device, object_uniform_buffer, object_uniform_memory, 0);

    // -----------------------------
    // 4. (Optional) map memory for CPU updates
    // -----------------------------
    vkMapMemory(
        device,
        object_uniform_memory,
        0,
        sizeof(ObjectUniformData),
        0,
        &object_uniform_mapped
    );

    // -----------------------------
    // 5. Allocate descriptor set
    // -----------------------------
	std::vector<VkDescriptorSetLayout> desc_set_layouts = {
		vulkan_instance.desc_set_layout_object,
		vulkan_instance.desc_set_layout_sampler
	};

    VkDescriptorSetAllocateInfo descAlloc{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = vulkan_instance.desc_pool,
        .descriptorSetCount = static_cast<uint32_t>(desc_set_layouts.size()),
        .pSetLayouts = desc_set_layouts.data()
    };

    vkAllocateDescriptorSets(device, &descAlloc, &object_descriptor_set);

    // -----------------------------
    // 6. Write descriptor
    // -----------------------------
    VkDescriptorBufferInfo object_buffer_info{
        .buffer = object_uniform_buffer,
        .offset = 0,
        .range = sizeof(ObjectUniformData),
    };

    VkWriteDescriptorSet object_write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = object_descriptor_set,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &object_buffer_info,
        .pTexelBufferView = nullptr
    };

	image->upload(vulkan_instance.device, cmd_pool, vulkan_instance.queue);

    VkDescriptorImageInfo sampler_buffer_info{
        .sampler = image->get_sampler(),
		.imageView = image->get_image_view(),
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkWriteDescriptorSet sampler_write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = sampler_descriptor_set,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &sampler_buffer_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr
    };

	std::vector<VkWriteDescriptorSet> writes = {
		object_write,
		sampler_write
	};

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}
}