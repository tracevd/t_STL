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
        UniquePtr( UniquePtr&& al ) noexcept
        {
            m_data = al.m_data;
            al.m_data = nullptr;
        }
        UniquePtr& operator=( const UniquePtr& rhs ) = delete;
        UniquePtr& operator=( UniquePtr&& rhs ) noexcept
        {
            if ( this == &rhs )
                return *this;
            if ( m_data != nullptr )
                delete m_data;
            m_data = rhs.m_data;
            rhs.m_data = nullptr;
            return *this;
        }
        UniquePtr& operator=( const T& in )
        {
            delete m_data;
            m_data = new T{ in };
            return *this;
        }
        UniquePtr& operator=( T&& in )
        {
            delete m_data;
            m_data = new T{ std::move( in ) };
            return *this;
        }
        ~UniquePtr()
        {
            if ( m_data == nullptr )
                return;
            /*if ( m_data != nullptr )
                std::cout << "UniquePtr destructor\n";*/
            if constexpr ( std::is_array_v< T > )
                delete[] m_data;
            else
                delete m_data;
        }
        T* release() { auto cpy = m_data; m_data = nullptr; return cpy; }
        T* get() const noexcept { return m_data; }
        T* operator->() const noexcept { return m_data; }
        T& operator*() { return *m_data; }
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
        T* m_data = nullptr;
    };

    template< typename T >
    class SharedPtr
    {
    public:
        constexpr SharedPtr() = default;
        SharedPtr( const T& in ):
            m_data( new T{ in } ),
            m_refCount( new uint64_t{ 1 } ) {}
        SharedPtr( T&& in ):
            m_data( new T{ std::move( in ) } ),
            m_refCount( new uint64_t{ 1 } ) {}
        SharedPtr( const SharedPtr& ptr ):
            m_data( ptr.m_data ),
            m_refCount( ptr.m_refCount )
        {
            ( *m_refCount )++;
        }
        SharedPtr( SharedPtr&& ptr ) noexcept:
            m_data( ptr.m_data ),
            m_refCount( ptr.m_refCount )
        {
            ptr.m_data = nullptr;
            ptr.m_refCount = nullptr;
        }
        ~SharedPtr()
        {
            if ( --( *m_refCount ) != 0 )
                return;
            if constexpr ( std::is_array_v< T > )
                delete[] m_data;
            else
                delete m_data;
            delete m_refCount;
        }
        SharedPtr& operator=( const T& in )
        {
            if ( m_refCount != nullptr )
            {
                *m_refCount -= 1;
                if ( *m_refCount == 0 )
                    delete m_data;
            }
            m_data = new T{ in };
            m_refCount = new uint64_t{ 1 };
            return *this;
        }
        SharedPtr& operator=( T&& in )
        {
            if ( m_refCount != nullptr )
            {
                *m_refCount -= 1;
                if ( *m_refCount == 0 )
                    delete m_data;
            }
            m_data = new T{ std::move( in ) };
            m_refCount = new uint64_t{ 1 };
            return *this;
        }
        T* get() const noexcept { return m_data; }
        T* operator->() const noexcept { return m_data; }
        T& operator*() { return *m_data; }
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
        uint64_t* m_refCount = nullptr;
        T* m_data = nullptr;
    };
}