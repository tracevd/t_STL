#include "Tint.h"
#include "Type.h"
#include "Error.h"

namespace t
{
	template< typename T = int32, class = type::enable_if< type::is_integer< T > > >
	class Fraction
	{
	public:
		using ValueType = T;
	public:
		explicit constexpr Fraction( T numerator, T denominator = 1 ):
			m_num( numerator ),
			m_den( denominator )
		{
			if ( denominator <= 0 )
			{
				throw Error( "Invalid t::Fraction denominator. Must be greater than 0", 1 );
			}
		}

		constexpr Fraction( Fraction const& other ):
			m_num( other.m_num ),
			m_den( other.m_den ) {}

		[[nodiscard]] constexpr double value() const
		{
			return static_cast< double >( m_num ) / static_cast< double >( m_den );
		}

		[[nodiscard]] constexpr T numerator() const { return m_num; }

		[[nodiscard]] constexpr T denominator() const { return m_den; }

		constexpr Fraction& operator +=( Fraction const rhs )
		{
			m_num = m_num * rhs.m_den + rhs.m_num * m_den;
			m_den *= rhs.m_den;

			return *this;
		}

		constexpr Fraction& operator +=( T val )
		{
			m_num = m_num + val * m_den;

			return *this;
		}

		constexpr Fraction& operator -=( Fraction const rhs )
		{
			m_num = m_num * rhs.m_den - rhs.m_num * m_den;
			m_den *= rhs.m_den;

			return *this;
		}

		constexpr Fraction& operator -=( T val )
		{
			m_num = m_num - val * m_den;

			return *this;
		}

		constexpr Fraction& operator *=( Fraction const rhs )
		{
			m_num *= rhs.m_num;
			m_den *= rhs.m_den;

			return *this;
		}

		constexpr Fraction& operator *=( T val )
		{
			m_num *= val;

			return *this;
		}

		constexpr Fraction& operator /=( Fraction const rhs )
		{
			m_num *= rhs.m_den;
			m_den *= rhs.m_num;

			return *this;
		}

		constexpr Fraction& operator /=( T val )
		{
			m_den *= val;

			return *this;
		}

		void simplify()
		{
			auto const gcd = std::gcd( m_num, m_den );

			m_num /= gcd;
			m_den /= gcd;
		}

		friend std::ostream& operator <<( std::ostream& os, Fraction const frac )
		{
			auto const numerator = static_cast< type::ternary< type::is_same< T, char >, int, T > >( frac.numerator() );
			auto const denominator = static_cast< type::ternary< type::is_same< T, char >, int, T > >( frac.denominator() );
			os << numerator << '/' << denominator << '\n';
			return os;
		}
	private:
		T m_num = 0;
		T m_den = 1;
	};
}
