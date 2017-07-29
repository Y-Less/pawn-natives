#pragma once

#include <stdexcept>
#include <plugin.h>

namespace plugin_natives
{
	template <typename T>
	class TypeResolver
	{
	};

	template <typename T>
	class TypeResolver<void(T)>
	{
	public:
		// We need a specialisation because you can't just pass a type with a
		// parameter name directly as a typename, so we have to pass it as a
		// function type with no return and a parameter.  This is used to
		// (potentially) rename an already named parameter.  This is because
		// extracting the type is much simpler than extracting the name.
		typedef T type;
	};

	typedef struct varargs_s
	{
		explicit varargs_s(int num)
		:
			Count(num),
			Params((cell **)malloc(Count * sizeof (cell *)))
		{
			// This is used as "..." - instead of passing actual varargs, which
			// could be complex.
			if (!Params)
				throw std::bad_alloc();
		}

		varargs_s(AMX * amx, cell * params, int idx)
		:
			Count((int)params[0] / sizeof (cell) - idx + 1),
			Params((cell **)malloc(Count * sizeof (cell *)))
		{
			// This is used as "..." - instead of passing actual varargs, which
			// could be complex.
			if (!Params)
				throw std::bad_alloc();
			for (int i = 0; i != Count; ++i)
			{
				amx_GetAddr(amx, params[idx + i], Params + i);
			}
		}

		~varargs_s()
		{
			free(Params);
		}

		int     const Count;
		cell ** const Params;
	} * varargs_t;
};


#define SAMP_NATIVES_DEFER(a,b) a b

#define SAMP_NATIVES_TYPE(tt) typename ::plugin_natives::TypeResolver<void(tt)>::type

#define SAMP_NATIVES_WITHOUT_PARAMS_int(...)   int
#define SAMP_NATIVES_WITHOUT_PARAMS_float(...) float
#define SAMP_NATIVES_WITHOUT_PARAMS_bool(...)  bool
#define SAMP_NATIVES_WITHOUT_PARAMS_void(...)  void
#define SAMP_NATIVES_WITHOUT_PARAMS_cell(...)  cell

#define SAMP_NATIVES_WITHOUT_RETURN_int(...)   (__VA_ARGS__)
#define SAMP_NATIVES_WITHOUT_RETURN_float(...) (__VA_ARGS__)
#define SAMP_NATIVES_WITHOUT_RETURN_bool(...)  (__VA_ARGS__)
#define SAMP_NATIVES_WITHOUT_RETURN_void(...)  (__VA_ARGS__)
#define SAMP_NATIVES_WITHOUT_RETURN_cell(...)  (__VA_ARGS__)

#define SAMP_NATIVES_MAYBE_RETURN_int(...)   return
#define SAMP_NATIVES_MAYBE_RETURN_float(...) return
#define SAMP_NATIVES_MAYBE_RETURN_bool(...)  return
#define SAMP_NATIVES_MAYBE_RETURN_void(...)  
#define SAMP_NATIVES_MAYBE_RETURN_cell(...)  return

#define SAMP_NATIVES_NUM_ARGS_IMPL(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,N,...) N
#define SAMP_NATIVES_NUM_ARGS(...) SAMP_NATIVES_DEFER(SAMP_NATIVES_NUM_ARGS_IMPL, (0,__VA_ARGS__,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

#define SAMP_NATIVES_TOKENISE(a,b) SAMP_NATIVES_DEFER(SAMP_NATIVES_TOKENISE_, (a,b))
#define SAMP_NATIVES_TOKENISE_(a,b) a##b

#define SAMP_NATIVES_INPUT_0() 
#define SAMP_NATIVES_INPUT_1(a)      SAMP_NATIVES_TYPE(a) p0
#define SAMP_NATIVES_INPUT_2(a,...)  SAMP_NATIVES_TYPE(a) p1 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_1, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_3(a,...)  SAMP_NATIVES_TYPE(a) p2 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_2, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_4(a,...)  SAMP_NATIVES_TYPE(a) p3 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_3, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_5(a,...)  SAMP_NATIVES_TYPE(a) p4 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_4, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_6(a,...)  SAMP_NATIVES_TYPE(a) p5 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_5, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_7(a,...)  SAMP_NATIVES_TYPE(a) p6 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_6, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_8(a,...)  SAMP_NATIVES_TYPE(a) p7 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_7, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_9(a,...)  SAMP_NATIVES_TYPE(a) p8 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_8, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_10(a,...) SAMP_NATIVES_TYPE(a) p9 , SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_9, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_11(a,...) SAMP_NATIVES_TYPE(a) p10, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_10, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_12(a,...) SAMP_NATIVES_TYPE(a) p11, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_11, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_13(a,...) SAMP_NATIVES_TYPE(a) p12, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_12, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_14(a,...) SAMP_NATIVES_TYPE(a) p13, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_13, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_15(a,...) SAMP_NATIVES_TYPE(a) p14, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_14, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_16(a,...) SAMP_NATIVES_TYPE(a) p15, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_15, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_17(a,...) SAMP_NATIVES_TYPE(a) p16, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_16, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_18(a,...) SAMP_NATIVES_TYPE(a) p17, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_17, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_19(a,...) SAMP_NATIVES_TYPE(a) p18, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_18, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_20(a,...) SAMP_NATIVES_TYPE(a) p19, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_19, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_21(a,...) SAMP_NATIVES_TYPE(a) p20, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_20, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_22(a,...) SAMP_NATIVES_TYPE(a) p21, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_21, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_23(a,...) SAMP_NATIVES_TYPE(a) p22, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_22, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_24(a,...) SAMP_NATIVES_TYPE(a) p23, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_23, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_25(a,...) SAMP_NATIVES_TYPE(a) p24, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_24, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_26(a,...) SAMP_NATIVES_TYPE(a) p25, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_25, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_27(a,...) SAMP_NATIVES_TYPE(a) p26, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_26, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_28(a,...) SAMP_NATIVES_TYPE(a) p27, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_27, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_29(a,...) SAMP_NATIVES_TYPE(a) p28, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_28, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_30(a,...) SAMP_NATIVES_TYPE(a) p29, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_29, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_31(a,...) SAMP_NATIVES_TYPE(a) p30, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_30, (__VA_ARGS__))
#define SAMP_NATIVES_INPUT_32(a,...) SAMP_NATIVES_TYPE(a) p31, SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_31, (__VA_ARGS__))

#define SAMP_NATIVES_OUTPUT_0() 
#define SAMP_NATIVES_OUTPUT_1(a)      p0
#define SAMP_NATIVES_OUTPUT_2(a,...)  p1 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_1, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_3(a,...)  p2 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_2, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_4(a,...)  p3 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_3, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_5(a,...)  p4 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_4, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_6(a,...)  p5 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_5, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_7(a,...)  p6 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_6, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_8(a,...)  p7 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_7, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_9(a,...)  p8 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_8, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_10(a,...) p9 , SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_9, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_11(a,...) p10, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_10, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_12(a,...) p11, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_11, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_13(a,...) p12, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_12, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_14(a,...) p13, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_13, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_15(a,...) p14, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_14, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_16(a,...) p15, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_15, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_17(a,...) p16, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_16, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_18(a,...) p17, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_17, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_19(a,...) p18, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_18, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_20(a,...) p19, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_19, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_21(a,...) p20, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_20, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_22(a,...) p21, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_21, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_23(a,...) p22, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_22, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_24(a,...) p23, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_23, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_25(a,...) p24, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_24, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_26(a,...) p25, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_25, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_27(a,...) p26, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_26, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_28(a,...) p27, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_27, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_29(a,...) p28, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_28, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_30(a,...) p29, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_29, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_31(a,...) p30, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_30, (__VA_ARGS__))
#define SAMP_NATIVES_OUTPUT_32(a,...) p31, SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_31, (__VA_ARGS__))

#define SAMP_NATIVES_INPUT_IMPL(...) SAMP_NATIVES_TOKENISE(SAMP_NATIVES_INPUT_,SAMP_NATIVES_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define SAMP_NATIVES_OUTPUT_IMPL(...) SAMP_NATIVES_TOKENISE(SAMP_NATIVES_OUTPUT_,SAMP_NATIVES_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define SAMP_NATIVES_PARAMETERS(params) SAMP_NATIVES_DEFER(SAMP_NATIVES_INPUT_IMPL, SAMP_NATIVES_WITHOUT_RETURN_##params)
#define SAMP_NATIVES_CALLING(params) SAMP_NATIVES_DEFER(SAMP_NATIVES_OUTPUT_IMPL, SAMP_NATIVES_WITHOUT_RETURN_##params)
#define SAMP_NATIVES_RETURN(params) SAMP_NATIVES_WITHOUT_PARAMS_##params
#define SAMP_NATIVES_MAYBE_RETURN(params) SAMP_NATIVES_MAYBE_RETURN_##params

// I don't know what the GCC/Clang equivalent to this is:
//   
//   __pragma(comment(linker, "/EXPORT:_"#func"=_NATIVE_"#func));
//   
// That code takes a function called `_NATIVE_SetPlayerPos` (say) and exports it
// from the .dll as `_SetPlayerPos` (which is the `_cdecl` name mangling version
// of `SetPlayerPos`).  This means that we can internally have `SetPlayerPos` as
// an instance of a class with an `operator()` for calling, while providing an
// external API that looks quite similar in the form of a function with the same
// name that wraps the object.
// 
// I think that it might be possible by setting all the other versions as hidden
// with `__attribute__((visibility("hidden")))` and the wrapper function with
// exactly the same name as exported with `__attribute__((visibility("default")))`
// AND weak (so as to not have the names conflict) with `__attribute__((weak))`.
// This (I think) will make all internal code use the object, since that is the
// strong name, but will export the identical weak name for others to call.
#define PLUGIN_NATIVE(func,type) \
	extern "C" _declspec(dllimport) SAMP_NATIVES_RETURN(type) _cdecl            \
	    func(SAMP_NATIVES_PARAMETERS(type))

