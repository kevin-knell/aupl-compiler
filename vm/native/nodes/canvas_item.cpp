#include "canvas_item.hpp"

#include "vulkan.hpp"

auplib::CanvasItem::CanvasItem()
{
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
    VkDescriptorSetAllocateInfo descAlloc{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = vulkan_instance.desc_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vulkan_instance.desc_set_layout_object
    };

    vkAllocateDescriptorSets(device, &descAlloc, &object_descriptor_set);

    // -----------------------------
    // 6. Write descriptor
    // -----------------------------
    VkDescriptorBufferInfo bufferInfo{
        .buffer = object_uniform_buffer,
        .offset = 0,
        .range = sizeof(ObjectUniformData),
    };

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = object_descriptor_set,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = nullptr
    };

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}