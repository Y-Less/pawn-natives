#pragma once

#include <stdexcept>
#include <plugin.h>

namespace plugin_natives
{
	template <typename T>
	class ParamCast
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			value_ = *(T *)&params[idx];
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T()
		{
			return value_;
		}

	private:
		T
			value_;
	};

	template <typename T>
	class ParamCast<T const>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			// In theory, because `T` could contain `const`, we don't actually
			// need specialisations for constant parameters.  The pointer would
			// point in to AMX memory but be `const`.  However, people can cast
			// away `const` if they REALLY want to, so make a copy of the
			// parameter so that they still can't do that to modify the
			// original.  If they REALLY REALLY want to modify the original
			// parameter in AMX memory they will have to re-extract the pointer
			// from `params`.
			value_ = *(T *)&params[idx];
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T const () const
		{
			return value_;
		}

	private:
		T
			value_;
	};

	template <>
	class ParamCast<varargs_t>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(amx, params, idx)
		{
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator varargs_t()
		{
			return &value_;
		}

	private:
		struct varargs_s
			value_;
	};

	template <>
	class ParamCast<varargs_t const>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(amx, params, idx)
		{
			// This is used as "..." - instead of passing actual varargs, which
			// could be complex.
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator varargs_t const()
		{
			return &value_;
		}

	private:
		struct varargs_s
			value_;
	};

	template <typename T>
	class ParamCast<T *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			amx_GetAddr(amx, params[idx], (cell **)&value_);
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T *()
		{
			return value_;
		}

	private:
		T *
			value_;
	};

	template <typename T>
	class ParamCast<T const *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		{
			// In theory, because `T` could contain `const`, we don't actually
			// need specialisations for constant parameters.  The pointer would
			// point in to AMX memory but be `const`.  However, people can cast
			// away `const` if they REALLY want to, so make a copy of the
			// parameter so that they still can't do that to modify the
			// original.  If they REALLY REALLY want to modify the original
			// parameter in AMX memory they will have to re-extract the pointer
			// from `params`.
			cell *
				addr;
			amx_GetAddr(amx, params[idx], &addr);
			value_ = *(T *)&addr;
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T const *() const
		{
			return value_;
		}

	private:
		T *
			value_;
	};

	template <>
	class ParamCast<char *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			fake_('\0'),
			len_((int)params[idx + 1])
		{
			// Can't use `amx_StrParam` here, it allocates on the stack.  This
			// code wraps a lot of `sampgdk`, which fortunately is entirely
			// const-correct so we don't need to worry about strings not being
			// copied incorrectly.  We can also make the assumption that any
			// string is immediately followed by its length when it is an
			// output.
			if (len_ < 0)
				throw std::length_error("Invalid string length.");
			if (len_)
			{
				amx_GetAddr(amx, params[idx], &addr_);
				value_ = (char *)malloc(len_);
				if (!value_)
					throw std::bad_alloc();
				//amx_GetString(value_, addr_, 0, len_);
			}
			else
				value_ = &fake_;
		}

		~ParamCast()
		{
			// This is the only version that actually needs to write data back.
			if (len_)
				amx_SetString(addr_, value_, 0, 0, len_);
			free(value_);
		}

		operator char *()
		{
			return value_;
		}

	private:
		int
			len_;

		cell *
			addr_;
		
		char *
			value_;
		
		char
			fake_;
	};

	template <>
	class ParamCast<char const *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			fake_('\0')
		{
			// Can't use `amx_StrParam` here, it allocates on the stack.  This
			// `const` version is not optional at all - it ensures that the
			// string data is NOT written back.
			cell *
				addr;
			int
				len;
			amx_GetAddr(amx, params[idx], &addr);
			amx_StrLen(addr, &len);
			if (len)
			{
				value_ = (char *)malloc(len + 1);
				if (!value_)
					throw std::bad_alloc();
				amx_GetString(value_, addr, 0, len + 1);
			}
			else
				value_ = &fake_;
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
			free(value_);
		}

		operator char const *()
		{
			return value_;
		}

	private:
		char *
			value_;
		
		char
			fake_;
	};
};

