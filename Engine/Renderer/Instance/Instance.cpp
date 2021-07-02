
#include "Instance.h"

Instance::Instance() {
    vkInstance = nullptr;
    requiredExtensions = std::set<const char*>();
    applicationInfo = {};
    createInfo = {};
}

Instance* Instance::create(Allocator allocator) {

	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "App";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.pEngineName = "Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.apiVersion = VK_API_VERSION_1_1;

    #ifdef DEBUG

        //validation layers
        requireExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        const std::vector<const char*> validationLayers = {
             "VK_LAYER_KHRONOS_validation"
        };

    #else
        const std::vector<const char*> validationLayers;
    #endif

    //push all the glfw extensions to the set
    getRequiredExtensions();

    if(!checkRequiredExtensions(requiredExtensions))
        allocator.throwError(VK_ERROR_EXTENSION_NOT_PRESENT, "Could not find required extensions");

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.enabledLayerCount = validationLayers.size();

    // i'm fairly certain theres a better way to do this, not sure why i can't just get a pointer from the iterator
    // TODO: find a better way to do this
    std::vector<const char*> extens(requiredExtensions.cbegin(), requiredExtensions.cend());

    createInfo.ppEnabledExtensionNames = extens.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());

    createInfo.flags = 0;
    createInfo.pNext = nullptr;

    VkResult res = vkCreateInstance(&createInfo, allocator.allocationCallbacksPtr, &vkInstance);

    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create instance");
    }

    return this;
}

Instance *Instance::destroy(Allocator allocator) {
    vkDestroyInstance(vkInstance, allocator.allocationCallbacksPtr);
    return this;
}

Instance* Instance::requireExtension(const char *extensionName) {
    requiredExtensions.insert(extensionName);
    return this;
}

/////////////////////////
/// Private functions ///
/////////////////////////

Instance* Instance::getRequiredExtensions() {

    uint32_t extenCount = 0;
    const char** extens = glfwGetRequiredInstanceExtensions(&extenCount);

    for(int i = 0; i<extenCount; i++){
        this->requireExtension(extens[i]);
    }

    return this;
}

std::vector<VkExtensionProperties> Instance::getAvailableExtensions() {

    uint32_t extenCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extenCount, nullptr);
    std::vector<VkExtensionProperties> extens(extenCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extenCount, extens.data());

    return extens;
}

bool Instance::checkRequiredExtensions(const std::set<const char*>& required) {

    bool allSupported = false;

    std::vector<VkExtensionProperties> availableExtens = getAvailableExtensions();

    std::vector<const char*> extens = std::vector<const char*>(required.begin(), required.end());

    #ifdef DEBUG

        printf("Available Instance Extensions:\n");
        for(const auto exten : availableExtens)
            printf("----%s\n", exten.extensionName);

        printf("Required Instance Extensions:\n");
        for(const auto exten : requiredExtensions)
        printf("----%s\n", exten);

    #endif

    for (auto exten = extens.end()-1; exten != extens.begin()-1; exten--) {
        for (const auto availableExten : availableExtens) {
            if (strncmp(*exten, availableExten.extensionName, strlen(availableExten.extensionName)) == 0)
                extens.erase(exten);
        }
    }

    if(!extens.empty()){

        allSupported = false;
        for(const auto exten : extens) {
            printf("Could not find instance Extension: %s\n", exten);
        }

    }else
        allSupported = true;

    return allSupported;
}