#pragma once

namespace t
{
template< typename T, size_t S >
class array
{
private:
	T m_data[ S ];
public:
	constexpr array() { m_data = nullptr; };

	constexpr size_t size() const { return S; };

	constexpr const T& operator[]( size_t i ) const
	{ 
		return m_data[ i ];
	}

	constexpr T& operator[]( size_t i ) 
	{ 
		return m_data[ i ];
	}

	constexpr const T& front() const 
	{ 
		return m_data[ 0 ]; 
	}

	constexpr T& front() 
	{ 
		return m_data[ 0 ]; 
	}
};

}