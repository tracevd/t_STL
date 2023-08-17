#pragma once

#include "Tint.h"
#include "Type.h"

namespace t
{
    template< typename T >
    class UniquePtr
    {
    public:
        constexpr UniquePtr()                        = default;
        UniquePtr( const UniquePtr& al )             = delete;
        UniquePtr& operator=( const UniquePtr& rhs ) = delete;

        template< class... Args >
        constexpr UniquePtr( Args&&... args ):
            m_data( new T( std::forward< Args >( args )... ) ) {}

        constexpr UniquePtr( T* allocation ):
            m_data( allocation ) {}

        constexpr UniquePtr( const T& in ):
            m_data ( new T( in ) ) {}

        constexpr UniquePtr( T&& in ):
            m_data ( new T( move( in ) ) ) {}

        constexpr UniquePtr( UniquePtr&& other ) noexcept
        {
            m_data = other.m_data;
            other.m_data = nullptr;
        }        

        constexpr UniquePtr& operator=( UniquePtr&& rhs ) noexcept
        {
            if ( this == &rhs )
                return *this;
            DestroyData();
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            return *this;
        }

        constexpr UniquePtr& operator=( T* allocation ) noexcept
        {
            if ( allocation == m_data )
                return *this;
            DestroyData();

            m_data = allocation;
            return *this;
        }

        constexpr ~UniquePtr()
        {
            DestroyData();
        }

        constexpr T* release() { auto cpy_ptr = m_data; m_data = nullptr; return cpy_ptr; }

        constexpr T* get() const noexcept { return m_data; }

        constexpr T* operator->() const noexcept { return m_data; }

        constexpr T& operator*() { return *m_data; }

        constexpr T const& operator*() const { return *m_data; }

        constexpr bool operator==( const UniquePtr& rhs ) const { return m_data == rhs.m_data; }

        constexpr bool operator!=( const UniquePtr& rhs ) const { return m_data != rhs.m_data; }

        constexpr bool operator< ( const UniquePtr& rhs ) const { return m_data < rhs.m_data; }

        constexpr bool operator>=( const UniquePtr& rhs ) const { return m_data >= rhs.m_data; }

        constexpr bool operator> ( const UniquePtr& rhs ) const { return m_data > rhs.m_data; }

        constexpr bool operator<=( const UniquePtr& rhs ) const { return m_data <= rhs.m_data; }

        constexpr bool operator==( const void* const rhs ) const { return m_data == rhs; }

        constexpr bool operator!=( const void* const rhs ) const { return m_data != rhs; }

        constexpr bool operator< ( const void* const rhs ) const { return m_data < rhs; }

        constexpr bool operator>=( const void* const rhs ) const { return m_data >= rhs; }

        constexpr bool operator> ( const void* const rhs ) const { return m_data > rhs; }

        constexpr bool operator<=( const void* const rhs ) const { return m_data <= rhs; }

        constexpr operator bool() const { return m_data != nullptr; }
    private:
        constexpr void DestroyData()
        {
            if constexpr ( type::is_array< T > )
            {
                delete[] m_data;
            }
            else
            {
                delete m_data;
            }
        }
    private:
        T* m_data = nullptr;
    };

    template< typename T >
    class SharedPtr
    {
    public:
        constexpr SharedPtr() = default;

        constexpr SharedPtr( const T& in ):
            m_data( new Data( in ) ) {}

        constexpr SharedPtr( T&& in ):
            m_data( new Data( move( in ) ) ) {}

        template< class... Args >
        constexpr SharedPtr( Args... args ):
            m_data( new Data( std::forward< Args >( args )... ) ) {}

        constexpr SharedPtr( const SharedPtr& ptr ):
            m_data( ptr.m_data )
        {
            if ( m_data != nullptr )
            {
                m_data->references += 1;
            }
        }

        constexpr SharedPtr( SharedPtr&& ptr ) noexcept:
            m_data( ptr.m_data )
        {
            ptr.m_data = nullptr;
        }

        constexpr ~SharedPtr()
        {
            DestroyData();
        }

        constexpr SharedPtr& operator=( SharedPtr const& rhs )
        {
            if ( this == &rhs || m_data == rhs.m_data )
                return *this;
            DestroyData();
            m_data = rhs.m_data;

            if ( rhs.m_data != nullptr )
                m_data->references += 1;
        }

        constexpr SharedPtr& operator=( SharedPtr&& rhs )
        {
            if ( this == &rhs )
                return *this;
            DestroyData();

            m_data = rhs.m_data;
            rhs.m_data = nullptr;
        }

        constexpr T* get() const noexcept { return &m_data->data; }

        constexpr T* operator->() const noexcept { return get(); }

        constexpr T& operator*() { return m_data->data; }

        constexpr T const& operator*() const { return m_data->data; }

        constexpr bool operator==( const SharedPtr& rhs ) const { return m_data == rhs.m_data; }

        constexpr bool operator!=( const SharedPtr& rhs ) const { return m_data != rhs.m_data; }

        constexpr bool operator< ( const SharedPtr& rhs ) const { return m_data < rhs.m_data; }

        constexpr bool operator>=( const SharedPtr& rhs ) const { return m_data >= rhs.m_data; }

        constexpr bool operator> ( const SharedPtr& rhs ) const { return m_data > rhs.m_data; }

        constexpr bool operator<=( const SharedPtr& rhs ) const { return m_data <= rhs.m_data; }

        constexpr bool operator==( const T* const rhs )   const { return getPointer() == rhs; }

        constexpr bool operator!=( const T* const rhs )   const { return getPointer() != rhs; }

        constexpr bool operator< ( const T* const rhs )   const { return getPointer() < rhs; }

        constexpr bool operator>=( const T* const rhs )   const { return getPointer() >= rhs; }

        constexpr bool operator> ( const T* const rhs )   const { return getPointer() > rhs; }

        constexpr bool operator<=( const T* const rhs )   const { return getPointer() <= rhs; }

        constexpr bool operator==( std::nullptr_t )       const { return m_data == nullptr; }

        constexpr bool operator!=( std::nullptr_t )       const { return m_data !=  nullptr; }

        constexpr operator bool() const { return m_data != nullptr; }
    private:
        constexpr void DestroyData()
        {
            if ( m_data == nullptr )
                return;
            
            m_data->references -= 1;

            if ( m_data->references == 0 )
                delete m_data;
        }
        constexpr inline const void* const getPointer() const { return m_data ? &m_data->data : m_data; }
        struct Data
        {
            constexpr Data( T data ):
                data( move( data ) ),
                references( 1 ) {}
            template< class... Args >
            constexpr Data( Args... args ):
                data( std::forward< Args >( args )... ),
                references( 1 ) {}
            uint64 references;
            T data;
        };
    private:
        Data* m_data;
    };
}
