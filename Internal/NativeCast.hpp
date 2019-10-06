#pragma once

#include <stdexcept>
#include <string>
#include <amx/amx.h>

// This is in the global namespace, not the pawn_natives namespace.  It
// completely extends the class as transparently as I can make it.
template <typename T>
struct DI : public T
{
};

// This is in the global namespace, not the pawn_natives namespace.  It
// completely extends the class as transparently as I can make it.
template <typename T, size_t N>
struct ARG : public T
{
};

namespace pawn_natives
{
	// This is for any casts that can't go on, but where this is somewhat expected.  For example, a
	// cast to a player when there is no player.
	class ParamCastFailure : public ::std::invalid_argument
	{
	public:
		explicit ParamCastFailure() : ::std::invalid_argument("ParamCast failed acceptably.") {}
	};

	// This is for true cast errors.
	class ParamCastError : public ::std::invalid_argument
	{
	public:
		explicit ParamCastError() : ::std::invalid_argument("ParamCast had an exception.") {}
	};

	template <typename T, typename = void>
	struct ParamLookup
	{
		static T Val(cell ref)
		{
			return (T)ref;
		}

		static T * Ptr(cell * ref)
		{
			return (T *)ref;
		}
	};

	template <typename T, typename = void>
	struct ParamInject
	{
	};

	template <>
	struct ParamLookup<float>
	{
		static float Val(cell ref)
		{
			return amx_ctof(ref);
		}

		static float * Ptr(cell * ref)
		{
			return (float *)ref;
		}
	};

	template <typename T>
	class ParamCast
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(ParamLookup<T>::Val(params[idx]))
		{
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T()
		{
			return value_;
		}

		static constexpr int Size = 1;

	private:
		T
			value_;
	};

	template <typename T>
	class ParamCast<T const>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(ParamLookup<T>::Val(params[idx]))
		{
			// In theory, because `T` could contain `const`, we don't actually
			// need specialisations for constant parameters.  The pointer would
			// point in to AMX memory but be `const`.  However, people can cast
			// away `const` if they REALLY want to, so make a copy of the
			// parameter so that they still can't do that to modify the
			// original.  If they REALLY REALLY want to modify the original
			// parameter in AMX memory they will have to re-extract the pointer
			// from `params`.
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T const () const
		{
			return value_;
		}

		static constexpr int Size = 1;

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

		static constexpr int Size = 1;

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

		operator varargs_t()
		{
			return &value_;
		}

		static constexpr int Size = 1;

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
			cell *
				src;
			amx_GetAddr(amx, params[idx], &src);
			value_ = ParamLookup<T>::Ptr(src);
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
			// This one doesn't because we are passing the direct pointer, which means any writes
			// are done directly in to AMX memory.
		}

		operator T *()
		{
			return value_;
		}

		static constexpr int Size = 1;

	private:
		T *
			value_;
	};

	template <typename T>
	class ParamCast<T &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
			value_(ParamLookup<T>::Ref(params[idx]))
		{
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
			// This one doesn't because we are passing the direct pointer, which means any writes
			// are done directly in to AMX memory.
		}

		operator T &()
		{
			return value_;
		}

		static constexpr int Size = 1;

	private:
		T &
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
				src;
			amx_GetAddr(amx, params[idx], &src);
			value_ = ParamLookup<T>::Ptr(src);
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator T const *() const
		{
			return value_;
		}

		static constexpr int Size = 1;

	private:
		T *
			value_;
	};

	// Use `string *`.
	template <>
	class ParamCast<char *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx) = delete;
	};

	// Use `string const &`.
	template <>
	class ParamCast<char const *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx) = delete;
	};

	// `string &` doesn't exist any more.  If it is a return value, the convention
	// is to use `string *`.  If it is an input, use `string const &`.
	template <>
	class ParamCast<std::string &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx) = delete;
	};

	template <>
	class ParamCast<std::string *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
		:
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
				char *
					src = (char *)alloca(len_);
				amx_GetString(src, addr_, 0, len_);
				value_ = src;
			}
			else
				value_.clear();
		}

		~ParamCast()
		{
			// This is the only version that actually needs to write data back.
			if (len_)
				amx_SetString(addr_, value_.c_str(), 0, 0, len_);
		}

		operator std::string *()
		{
			return &value_;
		}

		static constexpr int Size = 2;

	private:
		int
			len_;

		cell *
			addr_;

		std::string
			value_;
	};

	template <>
	class ParamCast<std::string const &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int idx)
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
				char *
					src = (char *)alloca(len + 1);
				amx_GetString(src, addr, 0, len + 1);
				value_ = src;
			}
			else
				value_.clear();
		}

		~ParamCast()
		{
			// Some versions may need to write data back here, but not this one.
		}

		operator std::string const &()
		{
			return value_;
		}

		static constexpr int Size = 1;

	private:
		std::string
			value_;
	};

	template <size_t N, typename ... TS>
	struct ParamArray {};

	template <size_t N, typename T, typename ... TS>
	struct ParamArray<N, T, TS ...>
	{
		template <class F, typename ... NS>
		static inline auto Call(F that, AMX * amx, cell * params, size_t prev, NS ... vs)
			-> decltype(ParamArray<N - 1, TS ...>::Call(that, amx, params, prev + ParamCast<T>::Size, vs ..., ParamCast<T>(amx, params, prev)))
		{
			return ParamArray<N - 1, TS ...>::Call(that, amx, params, prev + ParamCast<T>::Size, vs ..., ParamCast<T>(amx, params, prev));
		}
	};

	template <>
	struct ParamArray<0>
	{
		template <class F, typename ... NS>
		static inline auto Call(F that, AMX * amx, cell * params, size_t prev, NS ... vs)
			-> decltype(that->Do(vs ...))
		{
			return that->Do(vs ...);
		}
	};

	template <typename ... TS>
	struct ParamData {};

	template <typename T, typename ... TS>
	struct ParamData<T, TS ...>
	{
		static inline constexpr int Sum()
		{
			return ParamCast<T>::Size + ParamData<TS ...>::Sum();
			//return 0 + ... + ParamCast<TS>::Size;
		}

		template <class F>
		static inline auto Call(F that, AMX * amx, cell * params)
			-> decltype(ParamArray<sizeof... (TS) + 1, T, TS ...>::Call(that, amx, params, 1))
		{
			return ParamArray<sizeof... (TS) + 1, T, TS ...>::Call(that, amx, params, 1);
		}
	};

	template <>
	struct ParamData<>
	{
		static inline constexpr int Sum()
		{
			return 0;
		}

		template <class F>
		static inline auto Call(F that, AMX *, cell *)
			-> decltype(that->Do())
		{
			return that->Do();
		}
	};

	/*
	    88888888ba,    88                             
	    88      `"8b   88                             
	    88        `8b  88                             
	    88         88  88                             
	    88         88  88                             
	    88         8P  88                             
	    88      .a8P   88                             
	    88888888Y"'    88                             
	*/

	template <typename T>
	class ParamCast<DI<T> *>
	{
	public:
		ParamCast(AMX *, cell *, int) = delete;
		ParamCast() = delete;
	};

	template <typename T>
	class ParamCast<DI<T>>
	{
	public:
		ParamCast(AMX *, cell *, int) = delete;
		ParamCast() = delete;
	};

	template <typename T>
	class ParamCast<DI<T> &>
	{
	public:
		ParamCast(AMX *, cell *, int)
		{
		}

		~ParamCast()
		{
		}

		operator DI<T> &()
		{
			return static_cast<DI<T> &>(ParamInject<T>::Get());
		}

		static constexpr int Size = 0;

		using type = T & ;
	};

	template <typename T>
	class ParamCast<DI<T> const &>
	{
	public:
		ParamCast(AMX *, cell *, int)
		{
		}

		~ParamCast()
		{
		}

		operator DI<T> const &()
		{
			return static_cast<DI<T> const &>(ParamInject<T>::Get());
		}

		static constexpr int Size = 0;

		using type = T const &;
	};

	/*
	           db         88888888ba     ,ad8888ba,   
	          d88b        88      "8b   d8"'    `"8b  
	         d8'`8b       88      ,8P  d8'            
	        d8'  `8b      88aaaaaa8P'  88             
	       d8YaaaaY8b     88""""88'    88      88888  
	      d8""""""""8b    88    `8b    Y8,        88  
	     d8'        `8b   88     `8b    Y8a.    .a88  
	    d8'          `8b  88      `8b    `"Y88888P"   
	*/

	template <typename T, size_t N>
	class ParamCast<ARG<T, N>> : private ParamCast<T>
	{
	public:
		ParamCast(AMX * amx, cell * params, int)
		:
			ParamCast<T>(amx, params, N + 1)
		{
		}

		~ParamCast()
		{
		}

		operator ARG<T, N>()
		{
			return static_cast<ARG<T, N>>((T)*this);
		}

		static constexpr int Size = 0;
	};

	template <typename T, size_t N>
	class ParamCast<ARG<T, N> const> : private ParamCast<T const>
	{
	public:
		ParamCast(AMX * amx, cell * params, int)
		:
			ParamCast<T const>(amx, params, N + 1)
		{
		}

		~ParamCast()
		{
		}

		operator ARG<T, N> const()
		{
			return static_cast<ARG<T, N> const>((T const)*this);
		}

		static constexpr int Size = 0;
	};

	template <typename T, size_t N>
	class ParamCast<ARG<T, N> *> : private ParamCast<T *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int)
		:
			ParamCast<T *>(amx, params, N + 1)
		{
		}

		~ParamCast()
		{
		}

		operator ARG<T, N> *()
		{
			return static_cast<ARG<T, N> *>((T *)*this);
		}

		static constexpr int Size = 0;
	};

	template <typename T, size_t N>
	class ParamCast<ARG<T, N> &> : private ParamCast<T &>
	{
	public:
		ParamCast(AMX * amx, cell * params, int)
		:
			ParamCast<T &>(amx, params, N + 1)
		{
		}

		~ParamCast()
		{
		}

		operator ARG<T, N> &()
		{
			return static_cast<ARG<T, N> &>((T &)*this);
		}

		static constexpr int Size = 0;
	};

	template <typename T, size_t N>
	class ParamCast<ARG<T, N> const *> : private ParamCast<T const *>
	{
	public:
		ParamCast(AMX * amx, cell * params, int)
		:
			ParamCast<T const *>(amx, params, N + 1)
		{
		}

		~ParamCast()
		{
		}

		operator ARG<T, N> const *()
		{
			return static_cast<ARG<T, N> const *>((T const *)*this);
		}

		static constexpr int Size = 0;
	};
}

