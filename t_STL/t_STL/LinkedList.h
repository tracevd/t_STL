#pragma once

#include "Memory.h"

namespace t
{
	namespace details
	{
		template< class T >
		struct ListNode
		{
		public:
			constexpr ListNode( T&& data ):
				data( std::move( data ) ) {}
			constexpr ListNode( T const& data ):
				data( data ) {}

			constexpr T& addNext( T&& data )
			{
				auto oldNext = next;
				next = new ListNode( std::move( data ) );
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
				prev = new ListNode( std::move( data ) );
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
			ListNode* next = nullptr;
			ListNode* prev = nullptr;
		};
	}

	template< class List >
	class ListConstIterator;

	template< class List >
	class ListIterator
	{
	public:
#ifdef _MSC_VER
		using ValueType = typename List::ValueType;
#else
		using ValueType = List::ValueType;
#endif
	private:
		using NodeType = details::ListNode< ValueType >;
	public:
		constexpr ListIterator( NodeType* node ):
			m_node( node ) {}

		ListIterator() = delete;

		constexpr ListIterator& operator++()
		{
			m_node = m_node->next;
			return *this;
		}

		constexpr bool operator==( ListIterator rhs )
		{
			return m_node == rhs.m_node;
		}

		constexpr bool operator!=( ListIterator rhs )
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
		friend ListConstIterator< List >;
	};

	template< class List >
	class ListConstIterator
	{
	public:
#ifdef _MSC_VER
		using ValueType = typename const List::ValueType;
	private:
		using InnerValueType = typename List::ValueType;
#else
		using ValueType = const List::ValueType;
	private:
		using InnerValueType = List::ValueType;
#endif
	private:
		using NodeType = details::ListNode< InnerValueType >;
	public:
		constexpr ListConstIterator( NodeType* node ):
			m_node( node ) {}

		constexpr ListConstIterator( ListIterator< List > it ):
			m_node( it.m_node ) {}

		ListConstIterator() = delete;

		constexpr ListConstIterator& operator++()
		{
			m_node = m_node->next;
			return *this;
		}

		constexpr bool operator==( ListConstIterator rhs )
		{
			return m_node == rhs.m_node;
		}

		constexpr bool operator!=( ListConstIterator rhs )
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
		using Iterator = ListIterator< LinkedList >;
		using ConstIterator = ListConstIterator< LinkedList >;
	private:
		using NodeType = details::ListNode< T >;
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
						it->next->prev = it->prev;
					if ( it->prev )
						it->prev->next = it->next;
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

template< class List >
constexpr bool operator==( t::ListIterator< List > lhs, t::ListConstIterator< List > rhs )
{
	return lhs.operator->() == rhs.operator->();
}

template< class List >
constexpr bool operator!=( t::ListIterator< List > lhs, t::ListConstIterator< List > rhs )
{
	return lhs.operator->() != rhs.operator->();
}

template< class List >
constexpr bool operator==( t::ListConstIterator< List > lhs, t::ListIterator< List > rhs )
{
	return lhs.operator->() == rhs.operator->();
}

template< class List >
constexpr bool operator!=( t::ListConstIterator< List > lhs, t::ListIterator< List > rhs )
{
	return lhs.operator->() != rhs.operator->();
}
