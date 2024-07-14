#pragma once

#include "Tint.h"
#include "Error.h"

namespace t
{
	template< class T, class SizeTy = uint64 >
	class BufferView
	{
	public:
		using SizeType = SizeTy;
		using ValueType = T;
	public:
		BufferView() = delete;

		BufferView( T* data, SizeTy numel ) noexcept:
			m_data( data ),
			m_size( numel ) {}

		BufferView( BufferView const& buf ) noexcept:
			m_data( buf.m_data ),
			m_size( buf.m_size ) {}

		BufferView( BufferView&& buf ) noexcept:
			m_data( buf.m_data ),
			m_size( buf.m_size ) {}

		BufferView& operator=( BufferView const& buf ) noexcept
		{
			m_data = buf.m_data;
			m_size = buf.m_size;
			return *this;
		}

		BufferView& operator=( BufferView&& buf ) noexcept
		{
			m_data = buf.m_data;
			m_size = buf.m_size;
		}

		T& operator[]( SizeTy index )
		{
			return m_data[ index ];
		}

		T const& operator[]( SizeTy index ) const
		{
			return m_data[ index ];
		}

		auto& at( SizeTy index )
		{
			if ( index >= m_size )
				throw Error( "Index exceeds size!", 1 );
			return operator[]( index );
		}

		auto& at( SizeTy index ) const
		{
			if ( index >= m_size )
				throw Error( "Index exceeds size!", 1 );
			return operator[]( index );
		}

		T* begin() noexcept { return m_data; }
		T* end() noexcept { return m_data + m_size; }
		T const* cbegin() const noexcept { return m_data; }
		T const* cend() const noexcept { return m_data + m_size; }
		auto begin() const noexcept { return cbegin(); }
		auto end() const noexcept { return cend(); }
	private:
		T* m_data;
		SizeTy m_size;
	};
}
