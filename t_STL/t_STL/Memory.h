#pragma once

#include <stdint.h>
#include <type_traits>

namespace t
{
    template< typename T >
    class UniquePtr
    {
    public:
        constexpr UniquePtr() = default;
        UniquePtr( T* allocation ):
            m_data( allocation ) {}
        UniquePtr( const T& in ):
            m_data ( new T{ in } ) {}
        UniquePtr( T&& in ):
            m_data ( new T{ std::move( in ) } ) {}
        UniquePtr( const UniquePtr& al ) = delete;
        UniquePtr( UniquePtr&& other ) noexcept
        {
            m_data = other.m_data;
            other.m_data = nullptr;
        }
        UniquePtr& operator=( const UniquePtr& rhs ) = delete;
        UniquePtr& operator=( UniquePtr&& rhs ) noexcept
        {
            if ( this == &rhs )
                return *this;
            DestroyData();
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            return *this;
        }
        ~UniquePtr()
        {
            DestroyData();
        }
        T* release() { auto cpy_ptr = m_data; m_data = nullptr; return cpy_ptr; }
        T* get() const noexcept { return m_data; }
        T* operator->() const noexcept { return m_data; }
        T& operator*() { return *m_data; }
        T const& operator*() const { return *m_data; }
        bool operator==( const UniquePtr& rhs ) const { return m_data == rhs.m_data; }
        bool operator!=( const UniquePtr& rhs ) const { return m_data != rhs.m_data; }
        bool operator< ( const UniquePtr& rhs ) const { return m_data < rhs.m_data; }
        bool operator>=( const UniquePtr& rhs ) const { return m_data >= rhs.m_data; }
        bool operator> ( const UniquePtr& rhs ) const { return m_data > rhs.m_data; }
        bool operator<=( const UniquePtr& rhs ) const { return m_data <= rhs.m_data; }
        bool operator==( const void* const rhs ) const { return m_data == rhs; }
        bool operator!=( const void* const rhs ) const { return m_data != rhs; }
        bool operator< ( const void* const rhs ) const { return m_data < rhs; }
        bool operator>=( const void* const rhs ) const { return m_data >= rhs; }
        bool operator> ( const void* const rhs ) const { return m_data > rhs; }
        bool operator<=( const void* const rhs ) const { return m_data <= rhs; }
        operator bool() const { return m_data != nullptr; }
    private:
        void DestroyData()
        {
            if constexpr ( std::is_array_v< T > )
            {
                delete[] m_data;
            }
            else
            {
                delete m_data;
            }
        }
        T* m_data = nullptr;
    };

    template< typename T >
    class SharedPtr
    {
    public:
        constexpr SharedPtr() = default;
        SharedPtr( const T& in ):
            m_data( new Data{ in } ) {}
        SharedPtr( T&& in ):
            m_data( new Data{ std::move( in ) } ) {}
        SharedPtr( const SharedPtr& ptr ):
            m_data( ptr.m_data )
        {
            if ( m_data != nullptr )
            {
                m_data->references += 1;
            }
        }
        SharedPtr( SharedPtr&& ptr ) noexcept:
            m_data( ptr.m_data )
        {
            ptr.m_data = nullptr;
        }
        ~SharedPtr()
        {
            DestroyData();
        }
        SharedPtr& operator=( SharedPtr const& rhs )
        {
            if ( this == &rhs || m_data == rhs.m_data )
                return *this;
            DestroyData();
            m_data = rhs.m_data;

            if ( rhs.m_data != nullptr )
                m_data->references += 1;
        }
        SharedPtr& operator=( SharedPtr&& rhs )
        {
            if ( this == &rhs )
                return *this;
            DestroyData();

            m_data = rhs.m_data;
            rhs.m_data = nullptr;
        }
        T* get() const noexcept { return &m_data->data; }
        T* operator->() const noexcept { return get(); }
        T& operator*() { return m_data->data; }
        T const& operator*() const { return m_data->data; }
        bool operator==( const SharedPtr& rhs ) const { return m_data == rhs.m_data; }
        bool operator!=( const SharedPtr& rhs ) const { return m_data != rhs.m_data; }
        bool operator< ( const SharedPtr& rhs ) const { return m_data < rhs.m_data; }
        bool operator>=( const SharedPtr& rhs ) const { return m_data >= rhs.m_data; }
        bool operator> ( const SharedPtr& rhs ) const { return m_data > rhs.m_data; }
        bool operator<=( const SharedPtr& rhs ) const { return m_data <= rhs.m_data; }
        bool operator==( const void* const rhs ) const { return m_data == rhs; }
        bool operator!=( const void* const rhs ) const { return m_data != rhs; }
        bool operator< ( const void* const rhs ) const { return m_data < rhs; }
        bool operator>=( const void* const rhs ) const { return m_data >= rhs; }
        bool operator> ( const void* const rhs ) const { return m_data > rhs; }
        bool operator<=( const void* const rhs ) const { return m_data <= rhs; }
    private:
        void DestroyData()
        {
            if ( m_data == nullptr )
                return;
            m_data->references -= 1;
            if ( m_data->references == 0 )
                delete m_data;
        }
        struct Data
        {
            Data( T data ):
                data( std::move( data ) ),
                references( 1 ) {}
            uint64_t references;
            T data;
        };
        Data* m_data;
    };
}