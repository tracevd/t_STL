#pragma once

namespace t
{
	template< typename Vector >
	class VectorIterator
	{
	public:
		using ValueType = Vector::ValueType;
	private:
		ValueType* m_ptr;
	public:
		VectorIterator( ValueType* ptr )
			: m_ptr(ptr) {}
		VectorIterator& operator++()
		{
			m_ptr++;
			return *this;
		}
		VectorIterator operator++(int)
		{
			VectorIterator it = *this;
			++(*this);
			return it;
		}
		VectorIterator& operator--()
		{
			m_ptr--;
			return *this;
		}
		VectorIterator operator--( int )
		{
			VectorIterator it = *this;
			--(*this);
			return it;
		}
		VectorIterator operator+( size_t offset )
		{
			return VectorIterator( m_ptr + offset );
		}
		ValueType& operator[]( int i )
		{
			return *( m_ptr[i] );
		}
		ValueType* operator->()
		{
			return m_ptr;
		}
		ValueType& operator*()
		{
			return *m_ptr;
		}
		bool operator==( const VectorIterator& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		bool operator!=( const VectorIterator& other ) const
		{
			return !( *this == other );
		}
		bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	};
	template< typename Vector >
	class VectorIteratorConst
	{
	public:
		using ValueType = typename Vector::ValueType;
		using PointerType = ValueType* const;
		using ReferenceType = ValueType const&;
	private:
		PointerType m_ptr;
	public:
		VectorIteratorConst( PointerType ptr )
			: m_ptr( ptr ) {}
		VectorIteratorConst& operator++()
		{
			m_ptr++;
			return *this;
		}
		VectorIteratorConst operator++( int )
		{
			VectorIterator it = *this;
			++(*this);
			return it;
		}
		VectorIteratorConst& operator--()
		{
			m_ptr--;
			return *this;
		}
		VectorIteratorConst operator--( int )
		{
			VectorIterator it = *this;
			--(*this);
			return it;
		}
		ReferenceType operator[]( int i )
		{
			return *( m_ptr[ i ] );
		}
		PointerType operator->()
		{
			return m_ptr;
		}
		ReferenceType operator*()
		{
			return *m_ptr;
		}
		bool operator==( const VectorIteratorConst& other ) const
		{
			return m_ptr == other.m_ptr;
		}
		bool operator!=( const VectorIteratorConst& other ) const
		{
			return !(*this == other);
		}
		bool operator==( const void* other ) const
		{
			return m_ptr == other;
		}
		bool operator!=( const void* other ) const
		{
			return !(*this == other);
		}
	};
template< typename T >
class Vector
{
public:
	using ValueType = T;
	using Iterator = VectorIterator< Vector< T > >;
	using Const_Iterator = VectorIteratorConst< Vector< T > >;
public:
	Vector()
	{
		m_capacity = 2;
		m_data = new T[ m_capacity ];
		m_size = 0;
	}
	explicit Vector( size_t size )
	{
		m_data = new T[ size ];
		m_size = size;
		m_capacity = size;
	}
	Vector( const Vector& vec )
	{
		m_data = new T[ vec.m_capacity ];
		for ( size_t i = 0; i < vec.m_size; i++ )
			m_data[ i ] = vec[ i ];
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;
	}
	Vector( Vector&& vec ) noexcept
	{
		m_data = vec.m_data;
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;
		vec.m_size = 0;
		vec.m_capacity = 0;
		vec.m_data = nullptr;
	}
	template< typename U >
	Vector( Vector< U > const& other )
	{
		m_size = other.m_size;
		m_capacity = m_size;
		m_data = new T[ other.m_size ];
		for ( size_t i = 0; i < m_size; ++i )
		{
			m_data[ i ] = T{ other.m_data[ i ] };
		}
	}
	template< typename U >
	Vector( Vector< U >&& other )
	{
		m_size = other.m_size;
		m_capacity = m_size;
		m_data = new T[ other.m_size ];
		for ( size_t i = 0; i < m_size; ++i )
		{
			m_data[ i ] = T{ std::move( other.m_data[ i ] ) };
		}
	}
	Vector( std::initializer_list< T > list )
	{
		resize( list.size() );
		size_t i = 0;
		for ( auto it = list.begin(); it != list.end(); ++it, ++i )
		{
			m_data[ i ] = std::move( *it );
		}
	}
	~Vector()
	{
		if ( m_size != 0 )
			delete[] m_data;
	}
	Vector& operator=( Vector& vec )
	{
		if ( this == &vec )
			return *this;
		delete[] m_data;
		m_data = new T[ vec.m_capacity ];
		for ( size_t i = 0; i < vec.m_size; i++ )
			m_data[ i ] = vec[ i ];
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;

		return *this;
	}
	Vector& operator=( Vector&& vec )
	{
		m_data = vec.m_data;
		m_size = vec.m_size;
		m_capacity = vec.m_capacity;
		vec.m_data = nullptr;

		return *this;
	}
	bool operator==( Vector const& rhs ) const
	{
		if ( m_size != rhs.m_size )
			return false;
		for ( size_t i = 0; i < m_size; ++i )
		{
			if ( m_data[ i ] != rhs.m_data[ i ] )
				return false;
		}
		return true;
	}
	void reserve( size_t capacity )
	{
		if ( m_data != nullptr )
			delete[] m_data;
		m_data = new T[ capacity ];
		m_capacity = capacity;
		m_size = 0;
	}
	void resize( size_t size )
	{
		T* newData = new T[ size ];
		for ( size_t i = 0; i < m_size; ++i )
			newData[ i ] = std::move( m_data[ i ] );
		delete[] m_data;
		m_data = newData;
		m_size = size;
		m_capacity = size;
	}
	void shrinkToFit()
	{
		auto newdata = new T[ m_size ];
		for ( size_t i = 0; i < m_size; ++i )
		{
			newdata[ i ] = std::move( m_data[ i ] );
		}
		m_capacity = m_size;
		delete[] m_data;
		m_data = newdata;
	}

	[[nodiscard]] size_t size() const
	{
		return m_size;
	}
	template< typename Fn >
	void forEach( const Fn& fn )
	{
		for ( T& elem : *this )
		{
			fn( elem );
		}
	}
	template< typename Fn >
	void forEach( const Fn& fn ) const
	{
		for ( const T& elem : *this )
		{
			fn( elem );
		}
	}
	T& pop()
	{
		if ( m_size == 0 )
			throw std::runtime_error("Empty Vector!");
		return m_data[ --m_size ];
	}
	void removeFirst( const T& elem )
	{
		if ( m_size == 1 )
		{
			m_data[ 0 ] = T();
			--m_size;
			return;
		}
		bool deleted = false;
		for ( size_t i = 0; i < m_size; i++ )
		{
			if ( deleted )
			{
				m_data[ i - 1 ] = std::move( m_data[ i ] );
				continue;
			}
			if ( m_data[ i ] == elem )
			{
				m_data[ i ] = T();
				deleted = true;
			}
		}
		if ( deleted )
			--m_size;
	}
	Iterator begin()
	{
		return Iterator( m_data );
	}
	const Const_Iterator cbegin() const
	{
		return Const_Iterator( m_data );
	}
	Iterator end()
	{
		return Iterator( m_data + m_size );
	}
	const Const_Iterator cend() const
	{
		return Const_Iterator( m_data + m_size );
	}
	T& pushBack( const T& in )
	{
		if ( m_size == m_capacity )
			realloc();
		return m_data[ m_size++ ] = in;
	}
	T& pushBack( T&& in )
	{
		if ( m_size == m_capacity )
			realloc();
		return m_data[ m_size++ ] = std::move( in );
	}
	T& operator[]( size_t i )
	{
		if ( i > m_size )
			throw std::runtime_error("Exceeded Max Array Bounds");
		return m_data[ i ];
	}
	const T& operator[]( size_t i ) const
	{
		if ( i > m_size )
			throw std::runtime_error("Exceeded Max Array Bounds");
		return m_data[ i ];
	}
	T* find( const T& elem )
	{
		if ( m_size == 0 )
			return nullptr;
		if ( m_size < 5 )
		{
			return find_w_linearSweep( elem );
		}
		return find_w_convergingSweep( elem );
	}
	const T* const find( const T& elem ) const
	{
		if ( m_size == 0 )
			return nullptr;
		if ( m_size < 5 )
		{
			return find_w_linearSweep( elem );
		}
		return find_w_convergingSweep( elem );
	}
	void remove( const T& elem )
	{
		T* toRemove = find( elem );
		if ( toRemove == nullptr )
			return;
		uint64_t offset = toRemove - m_data;
		while ( offset < m_size-1 )
		{
			m_data[ offset ] = std::move( m_data[ ++offset ] );
		}
		--m_size;
	}
private:
	T* m_data = nullptr;
	size_t m_size = 0;
	size_t m_capacity = 0;
private:
	T* find_w_linearSweep( const T& elem )
	{
		for ( size_t i = 0; i < m_size; i++ )
		{
			if ( m_data[ i ] == elem )
				return m_data + i;
		}
		return nullptr;
	}
	const T* const find_w_linearSweep( const T& elem ) const
	{
		for ( size_t i = 0; i < m_size; i++ )
		{
			if ( m_data[i] == elem )
				return m_data + i;
		}
		return nullptr;
	}
	T* find_w_convergingSweep( const T& elem )
	{
		for ( size_t i = 0, j = m_size-1; i < m_size && i <= j; i++, j-- )
		{
			if ( m_data[ i ] == elem )
				return m_data + i;
			if ( m_data[ j ] == elem )
				return m_data + j;
		}
		return nullptr;
	}
	const T* const find_w_convergingSweep( const T& elem ) const
	{
		for ( size_t i = 0, j = m_size - 1; i < m_size && i <= j; i++, j-- )
		{
			if ( m_data[i] == elem )
				return m_data + i;
			if ( m_data[j] == elem )
				return m_data + j;
		}
		return nullptr;
	}
	void realloc()
	{
		size_t newSize = m_capacity * 2;
		// std::cout << "Realloc Vector: " << m_capacity << " -> " << newSize << "\n";
		auto temp = m_data;
		m_data = new T[ newSize ];
		for ( size_t i = 0; i < m_size; i++ )
			m_data[ i ] = std::move( temp[ i ] );
		m_capacity = newSize;
		delete[] temp;
	}
	void realloc( size_t newSize )
	{
		// std::cout << "Realloc Vector: " << m_capacity << " -> " << newSize << "\n";
		auto temp = m_data;
		m_data = new T[newSize];
		for ( size_t i = 0; i < m_size; i++ )
			m_data[ i ] = std::move( temp[i] );
		m_capacity = newSize;
		delete[] temp;
	}
};
}