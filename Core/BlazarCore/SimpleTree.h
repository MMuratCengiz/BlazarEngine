#pragma once

#include "Common.h"

NAMESPACES(ENGINE_NAMESPACE, Core)

template< typename T1, typename T2 = T1 >
struct TreeDataTuple
{
    T1 first;
    T2 second;
};

template< typename T1, typename T2 = T1, typename T3 = T1 >
struct TreeDataTriplet
{
    T1 first;
    T2 second;
    T3 third;
};

template< typename T, typename IdT = int >
struct TreeNode;

template< typename T, typename IdT >
struct TreeNode
{
    IdT id;
    T data;
    TreeNode< T, IdT > * parent;
    std::vector< TreeNode< T, IdT > * > children;
};

template< typename T, typename IdT = int >
class SimpleTree
{
private:
    typedef TreeNode< T, IdT > TNode;

    TNode * root;
public:
    SimpleTree()
    {
        root = new TNode { };
    }

    ~SimpleTree()
    {
        freeNode( root );
    };

    void freeNode( TNode * node )
    {
        for ( auto child: node->children )
        {
            freeNode( child );
        }

        delete node;
    }

    inline const TNode * getRoot() const
    {
        return root;
    }

    inline void setRootData( const IdT& id, const T& data )
    {
        root->id = id;
        root->data = data;
    }

    inline void addNode( const IdT& id, const T& data )
    {
        auto child = new TNode { };
        child->id = id;
        child->data = data;
        child->parent = root;

        root->children.push_back( std::move( child ) );
    }

    inline void addNode( TNode * parent, const IdT& id, const T& data )
    {
        auto child = new TNode { };
        child->id = id;
        child->data = data;
        child->parent = parent;

        parent->children.push_back( std::move( child ) );
    }

    inline TNode * findNode( const IdT& id )
    {
        return findNode( root, id );
    }

    inline TNode * findNode( TNode * iter, const IdT& id )
    {
        if ( iter->id == id )
        {
            return iter;
        }

        if ( iter->children.empty() )
        {
            return nullptr;
        }

        for ( const auto& child: iter->children )
        {
            auto * result = findNode( child, id );

            if ( result != nullptr )
            {
                return result;
            }
        }

        return nullptr;
    }

    inline std::vector< TNode * > flattenTree( )
    {
        std::vector< TNode * > result;

        flattenTree( result, root );

        return result;
    }

    inline void flattenTree( std::vector< TNode * >& data, TNode * parent )
    {
        data.push_back( parent );

        for ( TNode * child: parent->children )
        {
            flattenTree( data, child );
        }
    }
};

END_NAMESPACES