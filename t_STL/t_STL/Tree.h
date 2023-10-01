#pragma once

#include "Memory.h"

namespace t
{
    template< class T >
    struct Tree;

    template< class T >
    class TreeIterator;

    template< class T >
    class TreeConstIterator;

    namespace details
    {
        template< class T >
        struct TreeNode
        {
        public:
            using ValueType = T;
        public:
            constexpr TreeNode() = default;

            constexpr TreeNode( T&& data, TreeNode* parent ):
                m_data( std::move( data ) ),
                m_parent( parent ) {}

            TreeNode( TreeNode const& ) = delete;
            TreeNode( TreeNode&& ) = delete;
            TreeNode& operator=( TreeNode const& ) = delete;
            TreeNode& operator=( TreeNode&& ) = delete;

            constexpr ~TreeNode() = default;
            
            template< class U >
            constexpr bool insert( U&& data )
            {
                if ( data < m_data )
                {
                    if ( m_left )
                        return m_left->insert( T( std::move( data ) ) );
                    
                    m_left = new TreeNode( T( std::move( data ) ), this );
                    return true;
                }
                if ( data > m_data )
                {
                    if ( m_right )
                        return m_right->insert( T( std::move( data ) ) );
                    
                    m_right = new TreeNode( T( std::move( data ) ), this );
                    return true;
                }
                return false;
            }

            template< class U >
            constexpr TreeNode* find( U const& data )
            {
                if ( data < m_data )
                {
                    if ( m_left )
                        return m_left->find( data );
                    return nullptr;
                }
                if ( data > m_data )
                {
                    if ( m_right )
                        return m_right->find( data );
                    return nullptr;
                }
                if ( data == m_data )
                    return this;
                return nullptr;
            }

            constexpr TreeNode* getSmallestNodeOnTheRight() const
            {
                if ( m_right == nullptr )
                    return nullptr;
                auto node = m_right;

                while ( node->m_left != nullptr )
                    node = node->m_left;

                return node;
            }

        private:
            T m_data;
            TreeNode* m_right = nullptr;
            TreeNode* m_left = nullptr;
            TreeNode* m_parent = nullptr;
            friend Tree< T >;
            friend TreeIterator< Tree< T > >;
            friend TreeConstIterator< Tree< T > >;
        };
    }

    template< class Tree >
    class TreeIterator
    {
    private:
#ifdef _MSC_VER
        using NodeType = typename Tree::NodeType;
        using ValueType = typename NodeType::ValueType;
#else
        using NodeType = Tree::NodeType;
        using ValueType = NodeType::ValueType;
#endif
    public:
        TreeIterator() = delete;

        constexpr explicit TreeIterator( NodeType* node )
        {
            if ( node == nullptr )
                return;

            while ( node->m_left != nullptr )
                node = node->m_left;

            m_node = node;
        }

        constexpr TreeIterator& operator++()
        {
            if ( m_node->m_right )
            {
                m_node = m_node->m_right;

                while ( m_node->m_left )
                    m_node = m_node->m_left;
            }
            else
            {
                auto parent = m_node->m_parent;

                while ( parent && parent->m_data < m_node->m_data )
                    parent = parent->m_parent;

                m_node = parent;
            }

            return *this;
        }

        constexpr ValueType& operator*()
        {
            return m_node->m_data;
        }

        constexpr ValueType* operator->()
        {
            return &m_node->m_data;
        }

        constexpr bool operator==( TreeIterator const& rhs ) const
        {
            return m_node == rhs.m_node;
        }

        constexpr bool operator!=( TreeIterator const& rhs ) const
        {
            return m_node != rhs.m_node;
        }
    private:
        NodeType* m_node = nullptr;
        friend TreeConstIterator< Tree >;
    };

    template< class Tree >
    class TreeConstIterator
    {
    private:
#ifdef _MSC_VER
        using NodeType = typename const Tree::NodeType;
        using ValueType = typename const NodeType::ValueType;
#else
        using NodeType = const Tree::NodeType;
        using ValueType = const NodeType::ValueType;
#endif
    public:
        TreeConstIterator() = delete;

        constexpr TreeConstIterator( TreeIterator< Tree > it ):
            m_node( it.m_node ) {}

        constexpr explicit TreeConstIterator( NodeType* node )
        {
            if ( node == nullptr )
                return;

            while ( node->m_left != nullptr )
                node = node->m_left;

            m_node = node;
        }

        constexpr TreeConstIterator& operator++()
        {
            if ( m_node->m_right )
            {
                m_node = m_node->m_right;

                while ( m_node->m_left )
                    m_node = m_node->m_left;
            }
            else
            {
                auto parent = m_node->m_parent;

                while ( parent && parent->m_data < m_node->m_data )
                    parent = parent->m_parent;

                m_node = parent;
            }

            return *this;
        }

        constexpr ValueType& operator*()
        {
            return m_node->m_data;
        }

        constexpr ValueType* operator->()
        {
            return &m_node->m_data;
        }

        constexpr bool operator==( TreeConstIterator const& rhs ) const
        {
            return m_node == rhs.m_node;
        }

        constexpr bool operator!=( TreeConstIterator const& rhs ) const
        {
            return m_node != rhs.m_node;
        }
    private:
        NodeType* m_node = nullptr;
    };

    template< class T >
    struct Tree
    {
    private:
        using NodeType = details::TreeNode< T >;
    public:
        using Iterator = TreeIterator< Tree >;
        using ConstIterator = TreeConstIterator< Tree >;
    public:
        constexpr Tree() = default;

        constexpr ~Tree()
        {
            DestroyTree( m_data );
        }

        constexpr auto begin() { return Iterator( m_data ); }
        constexpr auto end() { return Iterator( nullptr ); }

        constexpr auto cbegin() { return ConstIterator( m_data ); }
        constexpr auto cend() { return ConstIterator( nullptr ); }

        constexpr auto begin() const { return cbegin(); }
        constexpr auto end() const { return cend(); }

        template< class U >
        constexpr void insert( U&& data )
        {
            if ( m_size == 0 )
            {
                m_data = new NodeType( T( std::move( data ) ), nullptr );
                ++m_size;
                return;
            }
            if ( m_data->insert( std::move( data ) ) )
                ++m_size;
        }

        template< class U >
        constexpr void insert( U const& data )
        {
            auto actual = data;
            insert( std::move( actual ) );
        }

        template< class U >
        constexpr T* find( U const& data )
        {
            if ( m_data == nullptr )
                return nullptr;

            auto ptr = m_data->find( data );

            if ( ptr )
                return &ptr->m_data;

            return nullptr;
        }

        template< class U >
        constexpr void remove( U const& data )
        {
            if ( m_data == nullptr )
                return;
            NodeType* node = m_data->find( data );

            if ( node == nullptr )
                return;

            // leaf node
            if ( node->m_left == nullptr && node->m_right == nullptr )
            {
                NodeType* parent = node->m_parent;
                if ( parent->m_left == node )
                    parent->m_left = nullptr;
                else
                    parent->m_right = nullptr;
                delete node;
                --m_size;
                return;
            }

            if ( node->m_right == nullptr && node->m_left )
            {
                NodeType* parent = node->m_parent;
                if ( parent )
                {
                    if ( parent->m_left == node )
                    {
                        parent->m_left = node->m_left;
                        parent->m_left->m_parent = parent;
                    }
                    else
                    {
                        parent->m_right = node->m_left;
                        parent->m_right->m_parent = parent;
                    }
                    delete node;
                    --m_size;
                }
                else
                {
                    m_data = node->m_left;
                    m_data->m_parent = nullptr;
                    delete node;
                    --m_size;
                }
                return;
            }

            if ( node->m_left == nullptr && node->m_right )
            {
                NodeType* parent = node->m_parent;
                if ( parent )
                {
                    if ( parent->m_left == node )
                    {
                        parent->m_left = node->m_right;
                        parent->m_left->m_parent = parent;
                    }
                    else
                    {
                        parent->m_right = node->m_right;
                        parent->m_right->m_parent = parent;
                    }
                    delete node;
                    --m_size;
                }
                else
                {
                    m_data = node->m_right;
                    m_data->m_parent = nullptr;
                    delete node;
                    --m_size;
                }
                return;
            }

            NodeType* successor = node->getSmallestNodeOnTheRight();

            assert( successor != nullptr );

            if ( successor != node->m_right )
            {
                successor->m_parent->m_left = successor->m_right;
                if ( successor->m_right )
                    successor->m_right->m_parent = successor->m_parent;
            }
            else
            {
                successor->m_parent = successor->m_parent->m_parent;
                if ( successor->m_parent )
                {
                    if ( successor->m_parent->m_right == node )
                        successor->m_parent->m_right = successor;
                    else
                        successor->m_parent->m_left = successor;
                }
            }

            if ( successor->m_parent == nullptr )
            {
                m_data = successor;
            }
            else
            {
            }

            successor->m_left = node->m_left;
            if ( successor->m_left )
                successor->m_left->m_parent = successor;

            delete node;
            --m_size;
        }

        constexpr uint64 size() const { return m_size; }

    private:
        constexpr void DestroyTree( NodeType* node )
        {
            if ( node->m_right )
            {
                DestroyTree( node->m_right );
                node->m_right = nullptr;
            }
                
            if ( node->m_left )
            {
                DestroyTree( node->m_left );
                node->m_left = nullptr;
            }

            delete node;
        }
    private:
        NodeType* m_data = nullptr;
        uint64 m_size = 0;
        friend TreeIterator< Tree< T > >;
    };
}