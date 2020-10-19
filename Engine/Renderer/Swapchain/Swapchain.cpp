#include "Swapchain.h"

VkExtent2D Swapchain::selectSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, Window window) {
	if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
		return surfaceCapabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { window.WIDTH, window.HEIGHT };

		actualExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkPresentModeKHR Swapchain::selectSwapchainPresentMode(std::vector<VkPresentModeKHR> presentModes) {
	for (const auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}
	//fifo mode always has to be available for vulkan compatibillity anyway so good backup
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkFormat Swapchain::selectSwapchainFormat(VkSurfaceFormatKHR format, std::vector<VkSurfaceFormatKHR> availableFormats) {

	VkFormat chosenFormat = availableFormats[0].format;

	switch (format.colorSpace)
	{
	case(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR):
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB) {
				chosenFormat = VK_FORMAT_B8G8R8A8_SRGB;
			}
		}
		break;
	default:
		break;
	}

	return chosenFormat;
}

VkColorSpaceKHR Swapchain::selectSwapchainColorSpace(VkSurfaceFormatKHR format, std::vector<VkSurfaceFormatKHR> availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		}
	}
	return availableFormats[0].colorSpace;
}

VkSurfaceFormatKHR Swapchain::selectSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats) {

	VkSurfaceFormatKHR format{};

	format.colorSpace = selectSwapchainColorSpace(format, availableFormats);
	format.format = selectSwapchainFormat(format, availableFormats);

	return format;
}

VkImageView Swapchain::createImageView(LogicalDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(logicalDevice.getLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Image view");
	}
	return imageView;
}

void Swapchain::createFrameBuffers(VkDevice vkLogicalDevice, RenderPass renderPass) {
	swapchainDetails.frameBuffers.resize(swapchainDetails.swapchainImages.size());

	for (size_t i = 0; i < swapchainDetails.frameBuffers.size(); i++) {

		std::array<VkImageView, 1> attachments = {
			swapchainDetails.swapchainImages[i].imageView
		};

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass.getRenderPasses().main;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = swapchainDetails.extent2D.width;
		framebufferCreateInfo.height = swapchainDetails.extent2D.height;
		framebufferCreateInfo.layers = 1;


		if (vkCreateFramebuffer(vkLogicalDevice, &framebufferCreateInfo, nullptr, &swapchainDetails.frameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
}

void Swapchain::create(PhysicalDevice physicalDevice, LogicalDevice logicalDevice, VkSurfaceKHR surface, Window window) {

    auto swapChainSupport = physicalDevice.getSurfaceSwapchainSupport(physicalDevice.getPhysicalDevice(), surface);

	VkSurfaceFormatKHR format = selectSwapchainSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = selectSwapchainPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = selectSwapExtent(swapChainSupport.capabilities, window);
	uint32_t minImageCount;
	if (swapChainSupport.capabilities.maxImageCount != 0) {
		minImageCount = std::min(swapChainSupport.capabilities.minImageCount + 1, swapChainSupport.capabilities.maxImageCount);
	}
	else {
		minImageCount = swapChainSupport.capabilities.minImageCount + 1;
	}
	VkSwapchainCreateInfoKHR swapChainCreateInfo{};

	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface;
	swapChainCreateInfo.minImageCount = minImageCount;
	swapChainCreateInfo.imageFormat = format.format;
	swapChainCreateInfo.imageColorSpace = format.colorSpace;
	swapChainCreateInfo.imageExtent = extent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.clipped = VK_TRUE;

	if (physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex != physicalDevice.getQueueFamilyIndices().presentationFamilyIndex) {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(physicalDevice.getQueueFamilyIndices().graphicsFamilyIndex), static_cast<uint32_t>(physicalDevice.getQueueFamilyIndices().presentationFamilyIndex) };
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
		swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}


	if (vkCreateSwapchainKHR(logicalDevice.getLogicalDevice(), &swapChainCreateInfo, nullptr, &vkSwapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swapchain");
	}

	swapchainDetails.extent2D = extent;
	swapchainDetails.format = format;
	swapchainDetails.presentMode = presentMode;

	swapchainImageCount = 0;
	vkGetSwapchainImagesKHR(logicalDevice.getLogicalDevice(), vkSwapchain, &swapchainImageCount, nullptr);
	std::vector<VkImage> images(swapchainImageCount);
	vkGetSwapchainImagesKHR(logicalDevice.getLogicalDevice(), vkSwapchain, &swapchainImageCount, images.data());

	for (VkImage image : images) {
		SwapchainImage swapchainImage{};
		swapchainImage.image = image;
		swapchainImage.imageView = createImageView(logicalDevice, image, format.format, VK_IMAGE_ASPECT_COLOR_BIT);
		swapchainDetails.swapchainImages.push_back(swapchainImage);
	}

}

VkSwapchainKHR Swapchain::getSwapchain() {
	return vkSwapchain;
}

SwapchainDetails Swapchain::getSwapchainDetails() {
	return swapchainDetails;
}

void Swapchain::destroy(VkDevice vkLogicalDevice) {

	for (auto framebuffer : swapchainDetails.frameBuffers) {
		vkDestroyFramebuffer(vkLogicalDevice, framebuffer, nullptr);
	}

	for (auto& image : swapchainDetails.swapchainImages) {
		vkDestroyImageView(vkLogicalDevice, image.imageView, nullptr);
	}

	vkDestroySwapchainKHR(vkLogicalDevice, vkSwapchain, nullptr);
}
