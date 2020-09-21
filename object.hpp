
#ifndef __AREDIS_OBJECT_HPP__
#define __AREDIS_OBJECT_HPP__

#include <memory>

namespace aredis::detail
{

	/**
	 * the lowest based class used fo CRTP
	 * see : CRTP and multilevel inheritance 
	 * https://stackoverflow.com/questions/18174441/crtp-and-multilevel-inheritance
	 */
	template<class derived_t, bool enable_shared_from_this = true>
	class object_t : public std::enable_shared_from_this<derived_t>
	{
	protected:
		/**
		 * @constructor
		 */
		object_t() = default;

		/**
		 * @destructor
		 */
		~object_t() = default;

	protected:
		/**
		 * @function : obtain derived class object through CRTP mechanism
		 */
		inline const derived_t & derived() const
		{
			return static_cast<const derived_t &>(*this);
		}

		/**
		 * @function : obtain derived class object through CRTP mechanism
		 */
		inline derived_t & derived()
		{
			return static_cast<derived_t &>(*this);
		}

	};

	template<class derived_t>
	class object_t<derived_t, false>
	{
	protected:
		/**
		 * @constructor
		 */
		object_t() = default;

		/**
		 * @destructor
		 */
		~object_t() = default;

	protected:
		/**
		 * @function : obtain derived class object through CRTP mechanism
		 */
		inline const derived_t & derived() const
		{
			return static_cast<const derived_t &>(*this);
		}

		/**
		 * @function : obtain derived class object through CRTP mechanism
		 */
		inline derived_t & derived()
		{
			return static_cast<derived_t &>(*this);
		}

	};

}

#endif // !__AREDIS_OBJECT_HPP__
