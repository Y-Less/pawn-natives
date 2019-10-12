#pragma once

#include <stdexcept>
#include <list>

#include "NativeImport.hpp"
#include "Internal/NativeCast.hpp"

#define PAWN_NATIVES_HAS_FUNC

namespace pawn_natives
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
				catch (ParamCastFailure const &)
				{
					// Acceptable failure (lookup failed etc.)
				}
				catch (std::exception const & e)
				{
					char
						msg[1024];
					sprintf(msg, "Exception in %s: \"%s\"", name_, e.what());
					LOG_NATIVE_ERROR(msg);
				}
				catch (...)
				{
					char
						msg[1024];
					sprintf(msg, "Unknown exception in in %s", name_);
					LOG_NATIVE_ERROR(msg);
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

	template <typename RET, typename ... TS>
	class NativeFunc : protected NativeFuncBase
	{
	public:
		inline RET operator()(TS ... args)
		{
			return Do(args ...);
		}

		virtual RET Do(TS ...) const = 0;

	protected:
		NativeFunc(char const * const name, AMX_NATIVE native) : NativeFuncBase(ParamData<TS ...>::Sum(), name, native) {}
		~NativeFunc() = default;

	private:

		cell CallDoInner(AMX * amx, cell * params)
		{
			RET
				ret = ParamData<TS ...>::Call(this, amx, params);
			return *(cell *)&ret;
		}
	};

	template <typename ... TS>
	class NativeFunc<void, TS ...> : protected NativeFuncBase
	{
	public:
		inline void operator()(TS ... args)
		{
			Do(args ...);
		}

		virtual void Do(TS ...) const = 0;

	protected:
		NativeFunc(char const * const name, AMX_NATIVE native) : NativeFuncBase(ParamData<TS ...>::Sum(), name, native) {}
		~NativeFunc() = default;

	private:
		cell CallDoInner(AMX * amx, cell * params)
		{
			ParamData<TS ...>::Call(this, amx, params);
			return 0;
		}
	};

	template <typename RET>
	class NativeFunc<RET> : protected NativeFuncBase
	{
	public:
		inline RET operator()()
		{
			return Do();
		}

		virtual RET Do() const = 0;

	protected:
		NativeFunc(char const * const name, AMX_NATIVE native) : NativeFuncBase(0, name, native) {}
		~NativeFunc() = default;

	private:
		cell CallDoInner(AMX * amx, cell * params)
		{
			RET
				ret = ParamData<>::Call(this, amx, params);
			return *(cell *)&ret;
		}
	};

	template <>
	class NativeFunc<void> : protected NativeFuncBase
	{
	public:
		inline void operator()()
		{
			Do();
		}

		virtual void Do() const = 0;

	protected:
		NativeFunc(char const * const name, AMX_NATIVE native) : NativeFuncBase(0, name, native) {}
		~NativeFunc() = default;

	private:
		cell CallDoInner(AMX * amx, cell * params)
		{
			ParamData<>::Call(this, amx, params);
			return 0;
		}
	};
}

// The hooks and calls for each class are always static, because otherwise it
// would make installing hooks MUCH harder - we would need stubs that could
// handle class pointers.  Doing that would negate needing a different class for
// every hook type, even when the parameters are the same, but this way is
// probably not much more generated code, and vastly simpler.
// 
// The inheritance from `NativeFuncBase` is protected, because we don't want
// normal users getting in to that data.  However, we do want them to be able to
// use the common `IsEnabled` method, so re-export it.
#define PAWN_NATIVE_DECL(nspace, func, type) PAWN_NATIVE_DECL_(nspace, func, type)

#define PAWN_NATIVE_DECL_(nspace, func, type) \
	template <typename F>                                                       \
	class Native_##func##_ {};                                                  \
	                                                                            \
	using Native_##func = Native_##func##_<type>;                               \
	                                                                            \
	template <typename RET, typename ... TS>                                    \
	class Native_##func##_<RET(TS ...)> :                                       \
	    public pawn_natives::NativeFunc<RET, TS ...>                            \
	{                                                                           \
	public:                                                                     \
	    Native_##func##_()                                                      \
	    :                                                                       \
	        pawn_natives::NativeFunc<RET, TS ...>(#func, (AMX_NATIVE)&Call)     \
	    {                                                                       \
	    }                                                                       \
	                                                                            \
	    RET Do(TS ...) const override;                                          \
	                                                                            \
	    static Native_##func##_<RET(TS ...)>                                    \
	        Instance;                                                           \
	                                                                            \
	private:                                                                    \
	    static cell AMX_NATIVE_CALL Call(AMX * amx, cell * params)              \
	    {                                                                       \
	        return Instance.CallDoOuter(amx, params);                           \
	    }                                                                       \
	}

//	    using namespace ::pawn_natives::detail;
//	    RET Do(typename UnDI<TS>::type_t ...) const override;

// We can't pass exceptions to another module easily, so just don't...
// 
// I quite like this:
//   
//   PAWN_NATIVE__MAYBE_RETURN(type) {};
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
#define PAWN_NATIVE_DEFN(nspace, func, params) PAWN_NATIVE_DEFN_(nspace, func, params)

#define PAWN_NATIVE_DEFN_(nspace, func, params) \
	Native_##func Native_##func::Instance;                                      \
	                                                                            \
	PAWN_NATIVE__RETURN(params)                                                 \
	    Native_##func::                                                         \
	    Do(PAWN_NATIVE__PARAMETERS(params)) const;                              \
		                                                                        \
	template <typename RET, typename ... TS>                                    \
	typename pawn_natives::ReturnResolver<RET>::type NATIVE_##func(TS ... args) \
	{                                                                           \
	    try                                                                     \
	    {                                                                       \
	        return PAWN_NATIVE__MAYBE_GET(params)(Native_##func##_<RET(TS ...)>::Instance.Do(args ...)); \
	    }                                                                       \
	    catch (std::exception & e)                                              \
	    {                                                                       \
	        char msg[1024];                                                     \
	        sprintf(msg, "Exception in _" #func ": \"%s\"", e.what());          \
	        LOG_NATIVE_ERROR(msg);                                              \
	    }                                                                       \
	    catch (...)                                                             \
	    {                                                                       \
	        LOG_NATIVE_ERROR("Unknown exception in _" #func);                   \
	    }                                                                       \
	    PAWN_NATIVE__MAYBE_RETURN(params) {};                                   \
	}                                                                           \
	                                                                            \
	PAWN_NATIVE_EXTERN template PAWN_NATIVE_DLLEXPORT                           \
	typename pawn_natives::ReturnResolver<PAWN_NATIVE__RETURN(params)>::type    \
	PAWN_NATIVE_API                                                             \
	    NATIVE_##func<PAWN_NATIVE__RETURN(params)>(PAWN_NATIVE__PARAMETERS(params)); \
	                                                                            \
	PAWN_NATIVE__RETURN(params)                                                 \
	    Native_##func::                                                         \
	    Do(PAWN_NATIVE__PARAMETERS(params)) const

#define PAWN_NATIVE_DECLARE PAWN_NATIVE_DECL
#define PAWN_NATIVE_DEFINE  PAWN_NATIVE_DEFN

#define PAWN_NATIVE(nspace, func, params) PAWN_NATIVE_DECL_(nspace, func, params); PAWN_NATIVE_DEFN_(nspace, func, params)

#if 0

// Example:

// In your header:
PAWN_NATIVE_DECL(SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a));

// In your code:
PAWN_NATIVE_DEFN(SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a))
{
	// Implementation here...
	SetPlayerPos(playerid, x, y, z);
	return SetPlayerFacingAngle(playerid, a);
}

#endif


