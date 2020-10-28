#pragma once

#include "../core/Common.h"
#include "IComponent.h"

NAMESPACES(SomeVulkan, ECS)

class IGameEntity {
private:
    std::unordered_map< uint32_t, std::shared_ptr< IComponent > > componentMap;
    bool componentMapInitialized = false;

    void initializeComponents() {
        if ( !componentMapInitialized ) {
            for ( auto component: components() ) {
                componentMap[ component->getId() ] = component;
            }

            componentMapInitialized = true;
        }
    }

protected:
    virtual std::vector< std::shared_ptr< IComponent > > components() = 0;
public:
    IGameEntity() = default;

    bool hasComponent( uint32_t index ) {
        initializeComponents();
        return componentMap.find( index ) != componentMap.end();
    }

    template< class CastAs >
    std::shared_ptr< CastAs > getComponent( ) {
        initializeComponents();
        return std::dynamic_pointer_cast< CastAs >( componentMap[ CastAs::UID ] );
    }

    template< class CType >
    std::shared_ptr< CType > createComponent( ) {
        CType cType = std::make_shared< CType >( );
        componentMap[ cType->getId() ] = cType;
    }
};

#define START_COMPONENTS private: \
    std::vector< std::shared_ptr< SomeVulkan::ECS::IComponent > > vComponents; \
    public:                                                                    \
    std::vector< std::shared_ptr< SomeVulkan::ECS::IComponent > > components() override {

#define END_COMPONENTS return vComponents; }

END_NAMESPACES