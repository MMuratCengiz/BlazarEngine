#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

class CommandExecutor;
class CommandList;

typedef struct CustomTransfer {
    VkDeviceSize transferSize;
    void * transferData;

    [[nodiscard]] VkDeviceSize size() const {
        return transferSize;
    }

    [[nodiscard]] void * data() const {
        return transferData;
    }
} CustomTransfer;

typedef enum class DeviceBufferType {
    Regular,
    Image
} DeviceBufferType;

typedef union DeviceBufferSize {
    VkDeviceSize size ;
    VkExtent2D extent;

    bool operator <( const DeviceBufferSize& other ) const {
        if ( size != 0 || other.size != 0) {
            return size < other.size;
        }

        return extent.width * extent.height < other.extent.width * other.extent.height;
    }
    bool operator <=( const DeviceBufferSize& other ) const {
        if ( size != 0 || other.size != 0 ) {
            return size <= other.size;
        }

        return extent.width * extent.height <= other.extent.width * other.extent.height;
    }

    bool operator >( const DeviceBufferSize& other ) const {
        if ( size != 0 || other.size != 0 ) {
            return size > other.size;
        }

        return extent.width * extent.height > other.extent.width * other.extent.height;
    }

    bool operator >=( const DeviceBufferSize& other ) const {
        if ( size != 0 || other.size != 0 ) {
            return size >= other.size;
        }

        return extent.width * extent.height >= other.extent.width * other.extent.height;
    }

    bool operator ==( const DeviceBufferSize& other ) const {
        if ( size != 0 || other.size != 0 ) {
            return size == other.size;
        }

        return extent.width == other.extent.width && extent.height == other.extent.height;
    }

    bool operator ==( const uint32_t& other ) const {
        return size == other;
    }

    bool operator ==( const VkExtent2D& other ) const {
        return extent.width == other.width && extent.height == other.height;
    }
} DeviceBufferSize;

typedef union DeviceBuffer {
    VkBuffer regular;
    VkImage image;
} DeviceBuffer;

typedef struct DeviceMemory {
    DeviceBuffer buffer;
    DeviceBufferType bufferType = DeviceBufferType::Regular;

    VkDeviceMemory memory;

    DeviceBufferSize currentMemorySize;
    VkMemoryPropertyFlags properties;
    VkBufferUsageFlags bufferUsage;
} DeviceMemory;

typedef struct FrameContext {
    std::shared_ptr< CommandExecutor > commandExecutor;
    std::shared_ptr< CommandList > cachedBuffers;

    DeviceMemory vbo;
    DeviceMemory ibo;
    std::vector< DeviceMemory > ubo;
    std::vector< DeviceMemory > tbo;
} FrameContext;

END_NAMESPACES