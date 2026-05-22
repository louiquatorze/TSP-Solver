
#include "VulkanContext.h"

#include <iostream>
#include <cstring>

VulkanContext::VulkanContext() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();

    std::cout << "[C++] Vulkan Context successfully created.\n";  
}

VulkanContext::~VulkanContext() {
    if (m_device != VK_NULL_HANDLE)
        vkDestroyDevice(m_device, nullptr);

    if (m_instance != VK_NULL_HANDLE)
        vkDestroyInstance(m_instance, nullptr);

    std::cout << "[C++] Vulkan Context successfully destroyed.\n";    
}

void VulkanContext::createInstance() {
    if (m_enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested, but not available!");

    // 1. Describe your application parameters to the driver
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Headless TSP Compute Context";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3; // Modern Vulkan baseline

    // 2. Configure the instance creation structure
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Headless setup: No window extensions required
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;

    // Attach development diagnostics if compiling in debug mode
    if (m_enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<u32>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan instance!");

    std::cout << "[C++] VkInstance created successfully.\n";
}

void VulkanContext::pickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    // Iterate and find a physical device that matches our compute criteria
    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    i32 highestRating = -1;

    for (const auto& device : devices) {
        i32 rating = ratePhysicalDevice(device);

        if (rating >= 0 && rating > highestRating) {
            bestDevice = device;
            highestRating = rating;
        }
    }

    if (bestDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to find a suitable compute GPU!");

    m_physicalDevice = bestDevice;

    // Optional: Print out the name of your active GPU
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &deviceProperties);

    std::cout << "[C++] Selected GPU: " << deviceProperties.deviceName << "\n";
}

i32 VulkanContext::ratePhysicalDevice(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceSubgroupProperties subgroupProperties{};
    subgroupProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;

    VkPhysicalDeviceProperties2 deviceProperties2{};
    deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    deviceProperties2.pNext = &subgroupProperties;
    vkGetPhysicalDeviceProperties2(device, &deviceProperties2);

    auto computeFamily = findComputeQueueFamily(device);
    auto transferFamily = findTransferQueueFamily(device);

    if (!computeFamily.has_value() || !transferFamily.has_value())
        return -1;

    // Add max compute queue family score
    i32 rating = computeFamily->second * 5;

    std::cout << "[C++] Physical device type: " << deviceProperties.deviceType << std::endl;

    switch (deviceProperties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            rating += 100000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            rating += 5000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            rating += 2500;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            rating += 500;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            rating += 0;
            break;
        default:
            return -1;
    }

    i32 maxComputeShmSize = deviceProperties.limits.maxComputeSharedMemorySize;
    rating += (maxComputeShmSize / 1024) * 10;

    i32 maxComputeWGI = deviceProperties.limits.maxComputeWorkGroupInvocations;
    rating += maxComputeWGI / 2;

    if (subgroupProperties.supportedOperations & VK_SUBGROUP_FEATURE_SHUFFLE_BIT)
        rating += 2000;
    
    return rating;
}

std::optional<std::pair<u32, u32>> VulkanContext::findComputeQueueFamily(VkPhysicalDevice device) {
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    i32 idx = -1;
    i32 highestRating = -1;

    for (u32 i = 0; i < queueFamilyCount; i++) {
        i32 rating = rateComputeQueueFamily(queueFamilies[i]);

        if (rating >= 0 && rating > highestRating) {
            idx = i;
            highestRating = rating;
        }
    }

    if (idx < 0 || highestRating < 0)
        return std::nullopt;

    return std::pair(static_cast<u32>(idx), static_cast<u32>(highestRating));
} 

std::optional<u32> VulkanContext::findTransferQueueFamily(VkPhysicalDevice device) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            return i;
    }

    return std::nullopt;
} 

i32 VulkanContext::rateComputeQueueFamily(VkQueueFamilyProperties& properties) {
    const VkQueueFlagBits otherQueueFlagBits[] ={
        VK_QUEUE_SPARSE_BINDING_BIT,
        VK_QUEUE_PROTECTED_BIT,
        VK_QUEUE_VIDEO_DECODE_BIT_KHR,
        VK_QUEUE_VIDEO_ENCODE_BIT_KHR,
        VK_QUEUE_OPTICAL_FLOW_BIT_NV,
    };
    
    if ((properties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0) 
        return -1; 

    i32 rating = 0;

    if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
        rating += 500;

    for (auto qfBit : otherQueueFlagBits) {
        if ((properties.queueFlags & qfBit) == 0)
            rating += 15;
    }

    rating += static_cast<int32_t>(properties.queueCount) * 25;

    return rating;
}

void VulkanContext::createLogicalDevice() {
    auto computeFamilyOpt = findComputeQueueFamily(m_physicalDevice);
    m_computeFamilyIndex = computeFamilyOpt->first;

    auto transferFamilyOpt = findTransferQueueFamily(m_physicalDevice);
    m_transferFamilyIndex = transferFamilyOpt.value();

    auto queueCreateInfos = getQueueCreateInfos();

    // 2. Request physical core features (empty = standard defaults)
    VkPhysicalDeviceFeatures deviceFeatures{};

    // 3. Configure the logical device structural details
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    // Headless: Zero external extensions needed (No VK_KHR_swapchain!)
    createInfo.enabledExtensionCount = 0;

    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical compute device!");

    vkGetDeviceQueue(m_device, m_computeFamilyIndex, 0, &m_computeQueue);

    if (m_transferFamilyIndex == m_computeFamilyIndex)
        m_transferQueue == m_computeQueue;
    else
        vkGetDeviceQueue(m_device, m_transferFamilyIndex, 0, &m_transferQueue);

    std::cout << "[C++] Logical interface initialized. " << std::endl;
}

std::vector<VkDeviceQueueCreateInfo> VulkanContext::getQueueCreateInfos() {
    static const f32 PRIORITY_HIGH = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;    

    VkDeviceQueueCreateInfo computeQueueCreateInfo{};
    computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    computeQueueCreateInfo.queueFamilyIndex = m_computeFamilyIndex;
    computeQueueCreateInfo.queueCount = 1;
    computeQueueCreateInfo.pQueuePriorities = &PRIORITY_HIGH;

    queueCreateInfos.push_back(computeQueueCreateInfo);
    
    if (m_computeFamilyIndex != m_transferFamilyIndex) {
        VkDeviceQueueCreateInfo transferQueueCreateInfo{};
        transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        transferQueueCreateInfo.queueFamilyIndex = m_transferFamilyIndex;
        transferQueueCreateInfo.queueCount = 1;
        transferQueueCreateInfo.pQueuePriorities = &PRIORITY_HIGH;
        
        queueCreateInfos.push_back(transferQueueCreateInfo);
    }

    return queueCreateInfos;
}

bool VulkanContext::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (std::strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) return false;
    }
    return true;
}