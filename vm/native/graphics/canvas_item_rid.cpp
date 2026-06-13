#include "canvas_item_rid.hpp"

#include "vulkan.hpp"
#include "canvas_item.hpp"

#include "ubo.hpp"

namespace auplib {

void CanvasItemRID_T::init() {
    VkDevice device = VulkanInstance::singleton()->device;

    // -----------------------------
    // 1. Create uniform buffer
    // -----------------------------
    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = sizeof(ObjectUBO),
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
        .memoryTypeIndex = VulkanInstance::singleton()->findMemoryType(
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
        sizeof(ObjectUBO),
        0,
        &object_uniform_mapped
    );

    // -----------------------------
    // 5. Allocate descriptor set
    // -----------------------------
    VkDescriptorSetAllocateInfo descAlloc{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = VulkanInstance::singleton()->desc_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &VulkanInstance::singleton()->desc_set_layout_object
    };

    vkAllocateDescriptorSets(device, &descAlloc, &object_descriptor_set);

    // -----------------------------
    // 6. Write descriptor
    // -----------------------------
    VkDescriptorBufferInfo object_buffer_info{
        .buffer = object_uniform_buffer,
        .offset = 0,
        .range = sizeof(ObjectUBO),
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

    vkUpdateDescriptorSets(device, 1, &object_write, 0, nullptr);
}

}