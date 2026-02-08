#ifndef VKUtil_H_
#define VKUtil_H_

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <assert.h>

#define VK_NO_PROTOTYPES

#include "volk.h"
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

/// @brief Helper macro to test the result of Vulkan calls which can return an error.
#define VK_CHECK(x)                                                                    \
	do                                                                                 \
	{                                                                                  \
		VkResult err = x;                                                              \
		if (err)                                                                       \
		{                                                                              \
			MGP_ERROR("Detected Vulkan error: %s\n", mrhi::to_string(err).c_str()); \
			abort();                    \
		}                                                                              \
	} while (0)

#define ASSERT_VK_HANDLE(handle)        \
	do                                  \
	{                                   \
		if ((handle) == VK_NULL_HANDLE) \
		{                               \
			MGP_ERROR("Handle is NULL\n");     \
			abort();                    \
		}                               \
	} while (0)


namespace mrhi {


	/**
	 * @brief Helper function to convert a VkResult enum to a string
	 * @param result Vulkan result to convert.
	 * @return The string to return.
	 */
	const std::string to_string(VkResult result);

}
#endif