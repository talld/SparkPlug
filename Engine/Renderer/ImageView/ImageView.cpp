#include "ImageView.h"

ImageView::ImageView() {
    imageCount = 0;
    images = std::vector<VkImage>();
    imageViews = std::vector<VkImageView>();
}

ImageView *ImageView::create(const Allocator &allocator, const LogicalDevice &device, const Swapchain &swapchain) {

    vkGetSwapchainImagesKHR(device.vkDevice, swapchain.vkSwapchain, &imageCount, nullptr);

    images.resize(imageCount);
    imageViews.resize(imageCount);

    vkGetSwapchainImagesKHR(device.vkDevice, swapchain.vkSwapchain, &imageCount, images.data());

    for(int i = 0; i < imageCount; i++){
        imageViews[i] = createImageView(allocator, device, images[i], swapchain.format.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    return this;
}

ImageView *ImageView::destroy(const Allocator &allocator, const LogicalDevice &device) {
    for(const auto imageView : imageViews){
        vkDestroyImageView(device.vkDevice, imageView, allocator.allocationCallbacksPtr);
    }
    return nullptr;
}

/////////////////////////
/// Private Functions ///
/////////////////////////

VkImageView ImageView::createImageView(const Allocator &allocator, const LogicalDevice &device, const VkImage &image,
                                       const VkFormat &format, const VkImageAspectFlagBits &aspectFlags) {

    VkImageView imageView;

    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_A;

    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    VkResult res = vkCreateImageView(device.vkDevice, &createInfo, allocator.allocationCallbacksPtr, &imageView);

    if(res != VK_SUCCESS){
        allocator.throwError(res, "Failed to create ImageView");
    }

    return imageView;
}