#pragma once

#include <stdexcept>
#include <list>

#include "NativeFuncGen.hpp"
#include "NativeCast.hpp"

#define PLUGIN_NATIVES_HAS_FUNC

namespace plugin_natives
{
	int AmxLoad(AMX * amx);

	class NativeFuncBase
	{
	public:
		void Init()
		{
			// Install this as a native.
			//Install(NULL);
		}

	protected:
		NativeFuncBase(unsigned int count, char const * const name, AMX_NATIVE native)
		:
			count_(count * sizeof (cell)),
			name_(name),
			native_(native),
			amx_(0),
			params_(0)
		{
			if (!all_)
				all_ = new std::list<NativeFuncBase *>();
			if (all_)
				all_->push_back(this);
		}
		
		~NativeFuncBase() = default;

		AMX * GetAMX() const { return amx_; }
		cell * GetParams() const { return params_; }
		
		cell CallDoOuter(AMX * amx, cell * params)
		{
			cell
				ret = 0;
			if (amx && params)
			{
				// Check that there are enough parameters.
				amx_ = amx;
				params_ = params;
				try
				{
					if (count_ > (unsigned int)params[0])
						throw std::invalid_argument("Insufficient arguments.");
					ret = this->CallDoInner(amx, params);
				}
				catch (std::exception & e)
				{
					char
						msg[1024];
					sprintf(msg, "Exception in %s: \"%s\"", name_, e.what());
					Log(LogLevel::ERROR, msg);
				}
				catch (...)
				{
					char
						msg[1024];
					sprintf(msg, "Unknown exception in in %s", name_);
					Log(LogLevel::ERROR, msg);
					params_ = 0;
					amx_ = 0;
					throw;
				}
				params_ = 0;
				amx_ = 0;
			}
			return (cell)ret;
		}

	private:
		virtual cell CallDoInner(AMX *, cell *) = 0;

		friend int AmxLoad(AMX * amx);

		NativeFuncBase() = delete;
		NativeFuncBase(NativeFuncBase const &) = delete;
		NativeFuncBase(NativeFuncBase const &&) = delete;
		NativeFuncBase const & operator=(NativeFuncBase const &) const = delete;
		NativeFuncBase const & operator=(NativeFuncBase const &&) const = delete;

		unsigned int
			count_;

		char const * const
			name_;

		AMX_NATIVE const
			native_;

		AMX *
			amx_;

		cell *
			params_;

		static std::list<NativeFuncBase *> *
			all_;
	};

	template <typename FUNC_TYPE>
	class NativeFunc {};

	// A pretty horrible combination of templates that make hooks quite seamless.
	template <typename RET>
	class NativeFunc0 : protected NativeFuncBase
	{
	public:
		inline RET operator()()
		{
			return Do();
		}

	protected:
		NativeFunc0(char const * const name, AMX_NATIVE native) : NativeFuncBase(0, name, native) {}
		~NativeFunc0() = default;

	private:
		cell CallDoInner(AMX *, cell *)
		{
			RET
				ret = this->Do();
			return *(cell *)&ret;
		}

		virtual RET Do() const = 0;
	};

	// Template specialisation for void returns, since they can't use "return X()".
	template <>
	class NativeFunc0<void> : protected NativeFuncBase
	{
	public:
		inline void operator()()
		{
			Do();
		}

	protected:
		NativeFunc0(char const * const name, AMX_NATIVE native) : NativeFuncBase(0, name, native) {}
		~NativeFunc0() = default;

	private:
		cell CallDoInner(AMX *, cell *)
		{
			this->Do();
			return 1;
		}

		virtual void Do() const = 0;
	};

	template <typename RET>
	class NativeFunc<RET()> : public NativeFunc0<RET> { protected: NativeFunc(char const * const name, AMX_NATIVE native) : NativeFunc0(name, native) {} };
};

// Defer declaring the other classes to a super macro file.
#define NATIVE_HOOK_TEMPLATE   typename A
#define NATIVE_HOOK_NAME       NativeFunc1
#define NATIVE_HOOK_TYPES      A
#define NATIVE_HOOK_PARAMETERS A a
#define NATIVE_HOOK_CALLING    a
#define NATIVE_HOOK_NUMBER     1
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B
#define NATIVE_HOOK_NAME       NativeFunc2
#define NATIVE_HOOK_TYPES      A, B
#define NATIVE_HOOK_PARAMETERS A a, B b
#define NATIVE_HOOK_CALLING    a, b
#define NATIVE_HOOK_NUMBER     2
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C
#define NATIVE_HOOK_NAME       NativeFunc3
#define NATIVE_HOOK_TYPES      A, B, C
#define NATIVE_HOOK_PARAMETERS A a, B b, C c
#define NATIVE_HOOK_CALLING    a, b, c
#define NATIVE_HOOK_NUMBER     3
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D
#define NATIVE_HOOK_NAME       NativeFunc4
#define NATIVE_HOOK_TYPES      A, B, C, D
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d
#define NATIVE_HOOK_CALLING    a, b, c, d
#define NATIVE_HOOK_NUMBER     4
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E
#define NATIVE_HOOK_NAME       NativeFunc5
#define NATIVE_HOOK_TYPES      A, B, C, D, E
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e
#define NATIVE_HOOK_CALLING    a, b, c, d, e
#define NATIVE_HOOK_NUMBER     5
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F
#define NATIVE_HOOK_NAME       NativeFunc6
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f
#define NATIVE_HOOK_NUMBER     6
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G
#define NATIVE_HOOK_NAME       NativeFunc7
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g
#define NATIVE_HOOK_NUMBER     7
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H
#define NATIVE_HOOK_NAME       NativeFunc8
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h
#define NATIVE_HOOK_NUMBER     8
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I
#define NATIVE_HOOK_NAME       NativeFunc9
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i
#define NATIVE_HOOK_NUMBER     9
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J
#define NATIVE_HOOK_NAME       NativeFunc10
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j
#define NATIVE_HOOK_NUMBER     10
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K
#define NATIVE_HOOK_NAME       NativeFunc11
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k
#define NATIVE_HOOK_NUMBER     11
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L
#define NATIVE_HOOK_NAME       NativeFunc12
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l
#define NATIVE_HOOK_NUMBER     12
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M
#define NATIVE_HOOK_NAME       NativeFunc13
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m
#define NATIVE_HOOK_NUMBER     13
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N
#define NATIVE_HOOK_NAME       NativeFunc14
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M, N
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m, n
#define NATIVE_HOOK_NUMBER     14
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N, typename O
#define NATIVE_HOOK_NAME       NativeFunc15
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M, N, O
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o
#define NATIVE_HOOK_NUMBER     15
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

#define NATIVE_HOOK_TEMPLATE   typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H, typename I, typename J, typename K, typename L, typename M, typename N, typename O, typename P
#define NATIVE_HOOK_NAME       NativeFunc16
#define NATIVE_HOOK_TYPES      A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P
#define NATIVE_HOOK_PARAMETERS A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p
#define NATIVE_HOOK_CALLING    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p
#define NATIVE_HOOK_NUMBER     16
#include "NativeFuncImpl.hpp"
#undef NATIVE_HOOK_NUMBER
#undef NATIVE_HOOK_CALLING
#undef NATIVE_HOOK_PARAMETERS
#undef NATIVE_HOOK_TYPES
#undef NATIVE_HOOK_NAME
#undef NATIVE_HOOK_TEMPLATE

// The hooks and calls for each class are always static, because otherwise it
// would make installing hooks MUCH harder - we would need stubs that could
// handle class pointers.  Doing that would negate needing a different class for
// every hook type, even when the parameters are the same, but this way is
// probably not much more generated code, and vastly simpler.
// 
// The inheritance from `NativeFuncBase` is protected, because we don't want
// normal users getting in to that data.  However, we do want them to be able to
// use the common `IsEnabled` method, so re-export it.
#define NATIVE_DECL(func,type) \
	extern "C" SAMP_NATIVES_RETURN(type) _cdecl  \
		NATIVE_##func SAMP_NATIVES_WITHOUT_RETURN_##type ;                      \
                                                                                \
	namespace plugin_natives                                                    \
	{                                                                           \
	    extern class Native_##func : public NativeFunc<type>                    \
	    {                                                                       \
	    public:                                                                 \
	        Native_##func() :                                                   \
	            NativeFunc<type>(#func, &Call) {}                               \
                                                                                \
	    private:                                                                \
	        friend SAMP_NATIVES_RETURN(type) _cdecl                             \
	            ::NATIVE_##func SAMP_NATIVES_WITHOUT_RETURN_##type ;            \
                                                                                \
	        static cell AMX_NATIVE_CALL                                         \
	            Call(AMX * amx, cell * params)                                  \
	        {                                                                   \
	            return ::plugin_natives::func.CallDoOuter(amx, params);         \
	        }                                                                   \
                                                                                \
	        SAMP_NATIVES_RETURN(type)                                           \
	            Do SAMP_NATIVES_WITHOUT_RETURN_##type const;                    \
	    } func;                                                                 \
	}

#if 0

// Example:

// In your header:
NATIVE_DECL(SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a));

// In your code:
NATIVE_DEFN(SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a))
{
	// Implementation here...
	SetPlayerPos(playerid, x, y, z);
	return SetPlayerFacingAngle(playerid, a);
}

#endif

// We can't pass exceptions to another module easily, so just don't...
// 
// I quite like this:
//   
//   SAMP_NATIVES_MAYBE_RETURN(type) {};
//   
// If there is a return type, it will compile as:
//   
//   return {};
//   
// Which means "return default value" in new C++ versions.  If there is no
// return type (void), it will compile as:
//   
//   {};
//   
// Which means nothing.
#define NATIVE_DEFN(func,type) \
	extern "C" SAMP_NATIVES_RETURN(type) _cdecl                                 \
	    NATIVE_##func(SAMP_NATIVES_PARAMETERS(type))                            \
	{                                                                           \
	    __pragma(comment(linker, "/EXPORT:_" #func "=_NATIVE_" #func));         \
	    try                                                                     \
	    {                                                                       \
	        SAMP_NATIVES_MAYBE_RETURN(type)                                     \
	            ::plugin_natives::func.Do(SAMP_NATIVES_CALLING(type));          \
	    }                                                                       \
	    catch (std::exception & e)                                              \
	    {                                                                       \
	        char                                                                \
	            msg[1024];                                                      \
	        sprintf(msg, "Exception in _" #func ": \"%s\"", e.what());          \
	        Log(LogLevel::ERROR, msg);                                          \
	    }                                                                       \
	    catch (...)                                                             \
	    {                                                                       \
	        Log(LogLevel::ERROR, "Unknown exception in _" #func);               \
	    }                                                                       \
	    SAMP_NATIVES_MAYBE_RETURN(type) {};                                     \
	}                                                                           \
                                                                                \
	plugin_natives::Native_##func plugin_natives::func;                         \
	SAMP_NATIVES_RETURN(type)                                                   \
	    plugin_natives::Native_##func::                                         \
	    Do SAMP_NATIVES_WITHOUT_RETURN_##type const

#define NATIVE_DECLARE NATIVE_DECL
#define NATIVE_DEFINE  NATIVE_DEFN

