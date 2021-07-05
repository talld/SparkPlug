#ifndef SPARKPLUG_IMAGEVIEW_H
#define SPARKPLUG_IMAGEVIEW_H

#include "../LogicalDevice/LogicalDevice.h"
#include "../Swapchain/Swapchain.h"

#include <vulkan/vulkan.h>
#include <vector>

class ImageView {

public:

    uint32_t imageCount;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;


    ImageView();

    ImageView *create(const Allocator &allocator, const LogicalDevice &device, const Swapchain &swapchain);

    ImageView *destroy(const Allocator &allocator, const LogicalDevice &device);

private:

    static VkImageView createImageView(const Allocator &allocator, const LogicalDevice &device, const VkImage &image,
                                       const VkFormat &format, const VkImageAspectFlagBits &aspectFlags);
};


#endif //SPARKPLUG_IMAGEVIEW_H
