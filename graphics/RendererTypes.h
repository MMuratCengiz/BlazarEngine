#pragma once

#include "../core/Common.h"

NAMESPACES( SomeVulkan, Graphics )

class CommandExecutor;
class CommandList;

struct CustomTransfer {
    vk::DeviceSize transferSize;
    void * transferData;

    [[nodiscard]] vk::DeviceSize size() const {
        return transferSize;
    }

    [[nodiscard]] void * data() const {
        return transferData;
    }
};

enum class DeviceBufferType {
    Regular,
    Image
};

#define DBS_OP( OPERATOR ) \
bool operator OPERATOR( const DeviceBufferSize& other ) const { \
    if ( size != 0 || other.size != 0) { \
        return size OPERATOR other.size;\
    } \
    return extent.width * extent.height OPERATOR other.extent.width * other.extent.height; \
}

union DeviceBufferSize {
    vk::DeviceSize size ;
    vk::Extent2D extent;

    DBS_OP( < )
    DBS_OP( <= )
    DBS_OP( > )
    DBS_OP( >= )
    DBS_OP( == )


	DeviceBufferSize(const uint32_t& other) {
		size = other;
	}

	DeviceBufferSize(const vk::Extent2D& other) {
		extent = other;
	}

    bool operator ==( const uint32_t& other ) const {
        return size == other;
    }

    bool operator ==( const vk::Extent2D& other ) const {
        return extent.width == other.width && extent.height == other.height;
    }

    DeviceBufferSize* operator =( const uint32_t& other ) {
        size = other;
        return this;
    }

    DeviceBufferSize* operator =( const vk::Extent2D& other ) {
        extent = other;
        return this;
    }

    DeviceBufferSize* operator =( const DeviceBufferSize& other ) {
        this->size = other.size;
        this->extent = other.extent;
        return this;
    }
};

union DeviceBuffer {
    vk::Buffer regular;
    vk::Image image;
};


struct DeviceMemory {
    DeviceBuffer buffer{ };
    DeviceBufferType bufferType = DeviceBufferType::Regular;

    vk::DeviceMemory memory{ };

    DeviceBufferSize currentMemorySize{ 0 };
    vk::MemoryPropertyFlags properties;
    vk::BufferUsageFlags bufferUsage;
};

struct FrameContext {
    std::shared_ptr< CommandExecutor > commandExecutor;
    std::shared_ptr< CommandList > cachedBuffers;

	DeviceMemory vbo{ };
	DeviceMemory ibo{ };
    std::vector< DeviceMemory > ubo;
    std::vector< DeviceMemory > tbo;
    uint64_t vboOffset = 0;
};

END_NAMESPACES