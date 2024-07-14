#pragma once

//#include <exception>

#include "Lib.h"

namespace t
{
	class Error
	{
	public:
		/*
		 * Expects a null-terminated string that will be copied into a new dynamically allocated string
		 */
		constexpr explicit Error( const char* message ):
			m_message( copyString( message ) ),
			m_deleteData( true ) {}

		/*
		 * Expects a null-terminated string that will live for the duration of the program		 
		 */
		constexpr Error( const char* message, int ):
			m_message( message ),
			m_deleteData( false ) {}

		constexpr const char* what()
		{
			return m_message ? m_message : "Unknown error";
		}

		constexpr virtual ~Error()
		{
			if ( m_deleteData )
			{
				delete[] m_message;
			}
		}
	private:
		static constexpr const char* copyString( const char* message )
		{
			auto const length = strlen( message );

			if ( length )
			{
				auto _message = new char[length + 1];
				strcpy( _message, message, length );
				_message[ length ] = '\0';
				return _message;
			}

			return nullptr;
		}

	private:
		const char* m_message = nullptr;
		bool m_deleteData;
	};
}