#pragma once

#include "Memory.h"

namespace t
{
	template< typename T >
	struct Node
	{
	public:
		using ValueType = T;
	public:
		constexpr Node( Node&& n ) noexcept:
			m_data( std::move( n.m_data ) ),
			m_next( std::move( n.m_next ) ),
			m_prev( n.m_prev ) {}
		constexpr Node( T&& val ):
			m_data( std::move( val ) ) {}
		constexpr Node( const T& val ):
			m_data( val ) {}
		constexpr Node& operator=( Node&& n ) noexcept
		{
			m_data = std::move( n.m_data );
			m_next = UniquePtr< Node< T > >( std::move( n.m_next ) );
			m_prev = n.m_prev;
		}
	public:
		T m_data;
		UniquePtr< Node< T > > m_next;
		Node< T >* m_prev;
	};

	template< typename LinkedList >
	struct LinkedListIterator
	{
	public:
		using T = typename LinkedList::ValueType;
	public:
		LinkedListIterator( Node< T >* data ):
			m_ptr( data ) {}
		LinkedListIterator& operator++()
		{
			m_ptr = m_ptr->m_next.get();
			return *this;
		}
		LinkedListIterator operator++( int )
		{
			LinkedListIterator l = *this;
			this->operator++();
			return l;
		}
		LinkedListIterator& operator--()
		{
			m_ptr = m_ptr->m_prev;
			return *this;
		}
		LinkedListIterator operator--( int )
		{
			LinkedListIterator l = *this;
			this->operator--();
			return l;
		}
		T& operator*() { return m_ptr->m_data; }
		bool operator==( const LinkedListIterator& rhs ) { return m_ptr == rhs.m_ptr; }
		bool operator!=( const LinkedListIterator& rhs ) { return !( *this == rhs ); }
		bool operator==( const void* const ptr ) { return m_ptr == ptr; }
		bool operator!=( const void* const ptr ) { return m_ptr != ptr; }
	private:
		Node< T >* m_ptr;
	};

	template< typename T >
	class LinkedList
	{
	public:
		using ValueType = T;
		using Iterator = LinkedListIterator< LinkedList< T > >;
		constexpr LinkedList() = default;
		constexpr LinkedList( LinkedList&& list ) noexcept:
			m_head( list.m_head.release() ),
			m_size( list.m_size ) {}
		constexpr LinkedList& operator=( LinkedList&& list ) noexcept
		{
			m_head = list.m_head.release();
			m_size = list.m_size;
		}
		constexpr ~LinkedList()
		{
			if ( m_tail == nullptr )
				return;
			UniquePtr< Node< T > >* tail = m_tail;
			if ( m_size > 5 )
			{
				size_t count = 0;
				while ( tail != nullptr && m_size > 5 )
				{
					tail = &tail->get()->m_prev->m_prev->m_next;
					count++;
					--m_size;
					if ( count == 5 )
					{
						UniquePtr< Node< T > > ptr = std::move( tail->get()->m_next );
						count = 0;
					}
				}
				if ( count > 0 )
				{
					UniquePtr< Node< T > > ptr = std::move( *tail );
					count = 0;
				}
			}
		}
		constexpr T& pushBack( const T& val )
		{
			++m_size;
			if ( m_head == nullptr )
			{
				m_head = UniquePtr< Node< T > >( val );
				m_tail = &m_head;
				return m_head.get()->m_data;
			}
			UniquePtr< Node< T > >* prev = m_tail;
			m_tail->get()->m_next = UniquePtr< Node< T > >( val );
			m_tail = &(m_tail->get()->m_next);
			m_tail->get()->m_prev = prev->get();
			return m_tail->get()->m_data;
		}
		constexpr T& pushFront( const T& val )
		{
			++m_size;
			if ( m_head == nullptr )
			{
				m_head = UniquePtr< Node< T > >( val );
				m_tail = &m_head;
				return m_head.get()->m_data;
			}
			
			UniquePtr< Node< T > > head = UniquePtr< Node< T > >( val );
			m_head.get()->m_prev = head.get();
			head.get()->m_next = std::move( m_head );
			m_head = std::move( head );
			if ( &m_head == m_tail )
				m_tail = &m_tail->get()->m_next;
			return m_head.get()->m_data;
		}
		constexpr T& pushBack( T&& val )
		{
			++m_size;
			if ( m_head == nullptr )
			{
				m_head = UniquePtr< Node< T > >( std::move( val ) );
				m_tail = &m_head;
				return m_head.get()->m_data;
			}
			UniquePtr< Node< T > >* prev = m_tail;
			m_tail->get()->m_next = UniquePtr< Node< T > >( std::move( val ) );
			m_tail = &( m_tail->get()->m_next );
			m_tail->get()->m_prev = prev->get();
			return m_tail->get()->m_data;
		}
		constexpr T& pushFront( T&& val )
		{
			++m_size;
			if ( m_head == nullptr )
			{
				m_head = UniquePtr< Node< T > >( std::move( val ) );
				m_tail = &m_head;
				return m_head.get()->m_data;
			}
			
			UniquePtr< Node< T > > head = UniquePtr< Node< T > >( std::move( val ) );
			m_head.get()->m_prev = head.get();
			head.get()->m_next = std::move( m_head );
			m_head = std::move( head );
			if ( &m_head == m_tail )
				m_tail = &m_tail->get()->m_next;
			return m_head.get()->m_data;
		}
		constexpr T* find( const T& val )
		{
			if ( m_size == 0 )
				return nullptr;
			const PtrType* head = &m_head;
			DataType* tail = m_tail->get();

			if ( head->get() == nullptr )
				return nullptr;

			while ( head->get() != nullptr || (tail != head->get() && &tail->m_next != head) )
			{
				if ( head->get()->m_data == val )
					return &head->get()->m_data;
				if ( tail->m_data == val )
					return &tail->m_data;

				head = &(head->get()->m_next);
				tail = tail->m_prev;
			}

			return nullptr;
		}
		constexpr T const* find( const T& val ) const
		{
			if ( m_size == 0 )
				return nullptr;
			if ( m_size == 1 )
			{
				if ( m_head.get()->m_data == val )
					return &m_head.get()->m_data;
				return nullptr;
			}
			if ( m_size == 2 )
			{
				if ( m_head.get()->m_data == val )
					return &m_head.get()->m_data;
				if ( m_tail->get()->m_data == val )
					return &m_tail->get()->m_data;
				return nullptr;
			}
			const PtrType* head = &m_head;
			DataType* tail = m_tail->get();

			if ( head->get() == nullptr )
				return nullptr;

			while ( head->get() != nullptr && tail != head->get() && &tail->m_next != head )
			{
				if ( head->get()->m_data == val )
					return &head->get()->m_data;
				if ( tail->m_data == val )
					return &tail->m_data;

				head = &(head->get()->m_next);
				tail = tail->m_prev;
			}

			return nullptr;
		}
		constexpr void remove( const T& val )
		{
			PtrType* head = &m_head;
			DataType* tail = m_tail->get();

			if ( head->get() == nullptr )
				return;

			while ( head->get() != nullptr || ( tail != head->get() && &tail->m_next != head) )
			{
				if ( head->get()->m_data == val )
				{
					if ( head->get() == m_head.get() )
					{
						removeHead();
						return;
					}
					--m_size;
					DataType* prev = head->get()->m_prev;
					DataType* newhead = head->get()->m_next.release();
					if ( &head->get()->m_next == m_tail )
					{
						m_tail = head;
					}
					*head = newhead;
					head->get()->m_prev = prev;
					return;
				}
				if ( tail->m_data == val )
				{
					if ( tail == m_tail->get() )
					{
						removeTail();
						return;
					}
					--m_size;
					DataType* prev = tail->m_prev;
					DataType* next = tail->m_next.release();
					next->m_prev = prev;
					prev->m_next = next;
					if ( m_tail == &tail->m_next )
					{
						m_tail = &prev->m_next;
					}
					return;
				}
				head = &( head->get()->m_next );
				tail = tail->m_prev;
			}
		}
		constexpr uint64 size() const { return m_size; }
		constexpr Iterator begin() { return Iterator( m_head.get() ); }
		constexpr Iterator end() { return Iterator( m_tail->get()->m_next.get() ); }
	private:
		using DataType = Node< T >;
		using PtrType = UniquePtr< DataType >;
		size_t m_size = 0;
		UniquePtr< Node< T > > m_head;
		UniquePtr< Node< T > >* m_tail;
	private:
		constexpr void removeHead()
		{
			--m_size;
			if ( m_size == 0 )
			{
				m_head = nullptr;
				return;
			}
			DataType* newhead = m_head.get()->m_next.release();
			m_head = newhead;
		}
		constexpr void removeTail()
		{
			--m_size;
			*m_tail = m_tail->get()->m_prev;
		}
	};
}
