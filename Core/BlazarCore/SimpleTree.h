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
    unsigned int totalNodes = 0;
    IdT largestIndex = 0;
public:
    SimpleTree()
    {
        root = new TNode { };
    }

    void freeNode( TNode * node )
    {
        if ( ! node )
        {
            return;
        }

        for ( auto child: node->children )
        {
            freeNode( child );
        }

        delete node;
    }

    inline TNode * getRoot() const
    {
        return root;
    }

    inline void setRootData( const IdT& id, const T& data )
    {
        root->id = id;
        root->data = data;
        // Todo maybe be more clear if root is considered a node or not
        totalNodes++;

        largestIndex = std::max( id, largestIndex );
    }

    inline void addNode( const IdT& id, const T& data )
    {
        auto child = new TNode { };
        child->id = id;
        child->data = data;
        child->parent = root;

        root->children.push_back( std::move( child ) );
        totalNodes++;
        largestIndex = std::max( id, largestIndex );
    }

    inline void addNode( TNode * parent, const IdT& id, const T& data )
    {
        auto child = new TNode { };
        child->id = id;
        child->data = data;
        child->parent = parent;

        parent->children.push_back( std::move( child ) );
        totalNodes++;
        largestIndex = std::max( id, largestIndex );
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

    inline std::vector< TNode * > flattenTree( bool skipRoot = false )
    {
        std::vector< TNode * > result;

        if ( !skipRoot )
        {
            flattenTree( result, root );
        }
        else
        {
            for ( TNode * child: root->children )
            {
                flattenTree( result, child );
            }
        }

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

    inline void moveNodeToParent( const IdT& id, const IdT& parentId )
    {
        auto parent = findNode( parentId );
        auto node = findNode( id );

        if ( node->parent != root && node->parent != nullptr )
        {
            int i = 0;

            for ( ; i < parent->children.size( ); ++i )
            {
                if ( parent->children[ i ]->id == id )
                {
                    break;
                }
            }

            parent->children.erase( parent->children.begin() + i );
        }

        parent->children.push_back( node );
        node->parent = parent;
    }

    [[nodiscard]] const unsigned int & size( ) const
    {
        return totalNodes;
    }

    [[nodiscard]] const IdT & getLargestIndex( ) const
    {
        return largestIndex;
    }
};

END_NAMESPACES