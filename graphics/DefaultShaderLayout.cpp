//
// Created by Murat on 10/22/2020.
//

#include "DefaultShaderLayout.h"

const uint32_t SomeVulkan::Graphics::DefaultShaderLayout::ALL_ELEMENTS_COUNT = 6;

SomeVulkan::Graphics::DefaultShaderLayout::DefaultShaderLayout( glm::vec3 pos, glm::vec3 color ) :
        pos( pos ), color( color ) {

}

const VkVertexInputBindingDescription& SomeVulkan::Graphics::DefaultShaderLayout::getBindingDescription( ) {
    return description;
}

const std::array< VkVertexInputAttributeDescription, 2 >& SomeVulkan::Graphics::DefaultShaderLayout::getAttributeDescription( ) {
    return descriptions;
}
