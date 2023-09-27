#pragma once

#include "Memory.h"

namespace t
{
	namespace details
	{
		template< class T >
		struct LinkedListNode
		{
		public:
			constexpr LinkedListNode( T&& data ):
				data( std::move( data ) ) {}
			constexpr LinkedListNode( T const& data ):
				data( data ) {}

			LinkedListNode() = delete;

			constexpr T& addNext( T&& data )
			{
				auto oldNext = next;
				next = new LinkedListNode( std::move( data ) );
				next->prev = this;
				next->next = oldNext;
				return next->data;
			}

			constexpr T& addNext( T const& data )
			{
				auto cpy = data;
				return addNext( std::move( cpy ) );
			}

			constexpr T& addPrev( T&& data )
			{
				auto oldPrev = prev;
				prev = new LinkedListNode( std::move( data ) );
				prev->next = this;
				prev->prev = oldPrev;
				return prev->data;
			}

			constexpr T& addPrev( T const& data )
			{
				auto cpy = data;
				return addPrev( std::move( cpy ) );
			}

		public:
			T data{};
			LinkedListNode* next = nullptr;
			LinkedListNode* prev = nullptr;
		};
	}

	template< class Buffer >
	class LinkedListConstIterator;

	template< class Buffer >
	class LinkedListIterator
	{
	public:
#ifdef _MSC_VER
		using ValueType = typename Buffer::ValueType;
#else
		using ValueType = Buffer::ValueType;
#endif
	private:
		using NodeType = details::LinkedListNode< ValueType >;
	public:
		constexpr LinkedListIterator( NodeType* node ):
			m_node( node ) {}

		LinkedListIterator() = delete;

		constexpr LinkedListIterator& operator++()
		{
			m_node = m_node->next;
			return *this;
		}

		constexpr bool operator==( LinkedListIterator rhs )
		{
			return m_node == rhs.m_node;
		}

		constexpr bool operator!=( LinkedListIterator rhs )
		{
			return m_node != rhs.m_node;
		}

		constexpr ValueType& operator*() const
		{
			return m_node->data;
		}

		constexpr ValueType* operator->() const
		{
			return &m_node->data;
		}

	private:
		NodeType* m_node;
		friend LinkedListConstIterator< Buffer >;
	};

	template< class Buffer >
	class LinkedListConstIterator
	{
	public:
#ifdef _MSC_VER
		using ValueType = typename const Buffer::ValueType;
	private:
		using InnerValueType = typename Buffer::ValueType;
#else
		using ValueType = const Buffer::ValueType;
	private:
		using InnerValueType = Buffer::ValueType;
#endif
	private:
		using NodeType = details::LinkedListNode< InnerValueType >;
	public:
		constexpr LinkedListConstIterator( NodeType* node ):
			m_node( node ) {}

		constexpr LinkedListConstIterator( LinkedListIterator< Buffer > it ):
			m_node( it.m_node ) {}

		LinkedListConstIterator() = delete;

		constexpr LinkedListConstIterator& operator++()
		{
			m_node = m_node->next;
			return *this;
		}

		constexpr bool operator==( LinkedListConstIterator rhs )
		{
			return m_node == rhs.m_node;
		}

		constexpr bool operator!=( LinkedListConstIterator rhs )
		{
			return m_node != rhs.m_node;
		}

		constexpr ValueType& operator*() const
		{
			return m_node->data;
		}

		constexpr ValueType* operator->() const
		{
			return &m_node->data;
		}

	private:
		NodeType* m_node;
	};

	template< class T >
	class LinkedList
	{
	public:
		using ValueType = T;
		using Iterator = LinkedListIterator< LinkedList >;
		using ConstIterator = LinkedListConstIterator< LinkedList >;
	private:
		using NodeType = details::LinkedListNode< T >;
	public:
		constexpr LinkedList() = default;

		constexpr ~LinkedList()
		{
			auto head = m_head;

			while ( head )
			{
				auto cpy = head;
				head = head->next;
				delete cpy;
				if ( head )
					head->prev = nullptr;
			}

			m_head = nullptr;
			m_tail = nullptr;
		}

		constexpr auto begin()
		{
			return Iterator( m_head );
		}

		constexpr auto end()
		{
			return Iterator( nullptr );
		}

		constexpr auto cbegin()
		{
			return ConstIterator( m_head );
		}

		constexpr auto cend()
		{
			return ConstIterator( nullptr );
		}

		constexpr uint64 size() const { return m_size; }

		constexpr T& pushBack( T&& data )
		{
			if ( m_head == nullptr )
			{
				InitHeadAndTail( std::move( data ) );
				return m_head->data;
			}

			if ( m_size == 1 )
			{
				m_tail = new NodeType( std::move( data ) );
				m_head->next = m_tail;
				++m_size;
				return m_tail->data;
			}

			m_tail->addNext( std::move( data ) );
			m_tail = m_tail->next;
			++m_size;
			return m_tail->data;
		}

		constexpr T& pushBack( T const& data )
		{
			auto cpy = data;

			return pushBack( std::move( cpy ) );
		}

		constexpr T& pushFront( T&& data )
		{
			if ( m_head == nullptr )
			{
				InitHeadAndTail( std::move( data ) );
				return m_head->data;
			}

			if ( m_size == 1 )
			{
				m_head = new NodeType( std::move( data ) );
				m_head->next = m_tail;
				m_tail->prev = m_head;
				++m_size;
				return m_head->data;
			}

			m_head->addPrev( std::move( data ) );
			m_head = m_head->prev;
			++m_size;
			return m_head->data;
		}

		constexpr T& pushFront( T const& data )
		{
			auto cpy = data;
			return pushFront( std::move( cpy ) );
		}

		constexpr Iterator find( T const& data )
		{
			if ( m_head == nullptr )
				return end();

			auto it = m_head;

			while ( it )
			{
				if ( it->data == data )
					return Iterator( it );
				it = it->next;
			}

			return end();
		}

		constexpr ConstIterator find( T const& data ) const
		{
			if ( m_head == nullptr )
				return cend();

			auto it = m_head;

			while ( it )
			{
				if ( it->data == data )
					return ConstIterator( it );
				it = it->next;
			}

			return cend();
		}

		constexpr void remove( T const& data )
		{
			if ( m_head == nullptr )
				return;

			if ( m_size == 1 )
			{
				delete m_head;
				m_head = m_tail = nullptr;
				--m_size;
				return;
			}

			auto it = m_head;

			while ( it )
			{
				if ( it->data == data )
				{
					if ( it->next )
					{
						it->next->prev = it->prev;
					}
					else
					{
						m_tail = m_tail->prev;
						m_tail->next = nullptr;
					}
						
					if ( it->prev )
					{
						it->prev->next = it->next;
					}
					else
					{
						m_head = m_head->next;
						m_head->prev = nullptr;
					}

					auto cpy = it;
					it = it->next;
					delete cpy;
					--m_size;
				}
				else
				{
					it = it->next;
				}
			}
		}

		constexpr void remove( ConstIterator where )
		{
			if ( m_head == nullptr || where == cend() )
				return;

			auto it = m_head;

			while ( it )
			{
				if ( &it->data == where.operator->() )
				{
					if ( it->next )
					{
						it->next->prev = it->prev;
					}
					else
					{
						m_tail = m_tail->prev;
						m_tail->next = nullptr;
					}
					if ( it->prev )
					{
						it->prev->next = it->next;
					}
					else
					{
						m_head = m_head->next;
						m_head->prev = nullptr;
					}
					delete it;
					it = nullptr;
					return;
				}

				it = it->next;
			}
		}

		constexpr ValueType& back()
		{
			if ( m_head == nullptr )
				throw std::runtime_error("Empty list!");

			return m_tail->data;
		}

		constexpr ValueType const& back() const
		{
			if ( m_head == nullptr )
				throw std::runtime_error("Empty list!");

			return m_tail->data;
		}

		constexpr ValueType& front()
		{
			if ( m_head == nullptr )
				throw std::runtime_error("Empty list!");

			return m_head->data;
		}

		constexpr ValueType const& front() const
		{
			if ( m_head == nullptr )
				throw std::runtime_error( "Empty list!" );

			return m_head->data;
		}

	private:
		constexpr void InitHeadAndTail( T&& data )
		{
			m_head = new NodeType( std::move( data ) );
			m_tail = m_head;
			++m_size;
		}

	private:
		NodeType* m_head = nullptr;
		NodeType* m_tail = nullptr;
		uint64    m_size = 0;
	};
}

template< class Buffer >
constexpr bool operator==( t::LinkedListIterator< Buffer > lhs, t::LinkedListConstIterator< Buffer > rhs )
{
	return lhs.operator->() == rhs.operator->();
}

template< class Buffer >
constexpr bool operator!=( t::LinkedListIterator< Buffer > lhs, t::LinkedListConstIterator< Buffer > rhs )
{
	return lhs.operator->() != rhs.operator->();
}

template< class Buffer >
constexpr bool operator==( t::LinkedListConstIterator< Buffer > lhs, t::LinkedListIterator< Buffer > rhs )
{
	return lhs.operator->() == rhs.operator->();
}

template< class Buffer >
constexpr bool operator!=( t::LinkedListConstIterator< Buffer > lhs, t::LinkedListIterator< Buffer > rhs )
{
	return lhs.operator->() != rhs.operator->();
}
