# plugin-natives
Macros and templates for quickly defining PAWN (SA:MP mostly) natives and also exporting them for other plguins to use.

## Example.

### Old method:

(Example taken from the streamer plugin, because why not):

Header:

```cpp
	cell AMX_NATIVE_CALL IsValidDynamicCP(AMX *amx, cell *params);
```

Registration:

```cpp
	{ "IsValidDynamicCP", Natives::IsValidDynamicCP },
```

Implementation:

```cpp
cell AMX_NATIVE_CALL Natives::IsValidDynamicCP(AMX *amx, cell *params)
{
	CHECK_PARAMS(1, "IsValidDynamicCP");
	boost::unordered_map<int, Item::SharedCheckpoint>::iterator c = core->getData()->checkpoints.find(static_cast<int>(params[1]));
	if (c != core->getData()->checkpoints.end())
	{
		return 1;
	}
	return 0;
}
```

### New method:

```cpp
PLUGIN_NATIVE(Natives, IsValidDynamicCP, bool(int id))
{
	boost::unordered_map<int, Item::SharedCheckpoint>::iterator c = core->getData()->checkpoints.find(id);
	return (c != core->getData()->checkpoints.end());
}
```

### Advantages:

* No more messing about with registering natives.
* Correct types - no need to deal with `params`.
* Your natives are exported for other plugins to call directly (instead of going through an AMX).

## Macros

### PLUGIN_NATIVE

This will declare a static native and register it.  If you want the function available to the rest of your code in other files, you need to perform declaration and definition separately - just like prototyping any other normal C function:

Prototype (declaration):

```cpp
NATIVE_DECL(Natives, IsValidDynamicCP, bool(int id))
```

Defintion:

```cpp
NATIVE_DEFN(Natives, IsValidDynamicCP, bool(int id))
{
	boost::unordered_map<int, Item::SharedCheckpoint>::iterator c = core->getData()->checkpoints.find(id);
	return (c != core->getData()->checkpoints.end())
}
```

### NATIVE_DECL

See above.

### NATIVE_DEFN

See above.

### NATIVE_DECLARE

Synonym for NATIVE_DECL.

### NATIVE_DEFINE

Synonym for NATIVE_DEFN.

### PLUGIN_HOOK

This is similar to `PLUGIN_NATIVE`, but hooks an existing native function instead of creating an entirely new one.  It again exports your new version for calling directly (note that this may bypass other hooks on the same function):

```cpp
PLUGIN_HOOK(my_namespace, SetPlayerPos, bool(int playerid, float x, float y, float z))
{
	logprintf("my_namespace::SetPlayerPos called");
	return SetPlayerPos(playerid, x, y, z),
}
```

While this function is being run, the hook is disabled so that calling the original does not get stuck in an infinite loop.

For prototyping, there are also equivalent macros:

```cpp
HOOK_DECL
HOOK_DEFN
HOOK_DECLARE
HOOK_DEFINE
```

If you are using sampgdk (which is required for hooks anyway) you may need to undefine the existing symbols (unless you are not using the C++ wrappers):

```cpp
#undef SetPlayerPos
PLUGIN_HOOK(my_namespace, SetPlayerPos, bool(int playerid, float x, float y, float z))
{
	logprintf("my_namespace::SetPlayerPos called");
	return SetPlayerPos(playerid, x, y, z),
}
```

### PLUGIN_IMPORT

If you want to use the functions from another plugin in your plugin, simply add:

```cpp
PLUGIN_IMPORT(Natives, IsValidDynamicCP, bool(int id));
```

This is the same whether the original was a hook or a new native - so you as a user don't need to worry about which it is or if it is going change (that was a nice string of two-letter words, you don't see that very often).  Calling it then becomes as simple as:

```cpp
// You can also have `using namespace Natives;` if you want.
bool exists = Natives::IsValidDynamicCP(42);
```

## Use

### Inclusion

To use just the native declarations is easiest - there are no additional dependencies.  Just include the following to any file in which you wish to declare or use this new style of native:

```cpp
#include <plugin-natives/NativeFunc>
```

To use hooks, the inclusion is mostly the same:

```cpp
#include <plugin-natives/NativeHook>
```

However note that this include requires you to have [sampgdk](https://github.com/Zeex/sampgdk) - for accessing the original versions of natives, and [subhook](https://github.com/Zeex/subhook) - for installing the hooks at an assembly level.  These are assumed to be includable as `<subhook/file>` and `<sampgdk/file>`.  Again - if you only want to declare natives and not hooks, you do not need these two dependencies.

To import natives from another plugin, use this instead:

```cpp
#include <plugin-natives/NativeImport>
```

### Initialisation

Like most things, this does require initialisation.  Fortunately this is quite simple.  In one file (probably your main file) add:

```cpp
#include <plugin-natives/NativeMain>
```

That will provide storage space for required objects and variables.  It is important that this comes AFTER `NativeFunc` and `NativeHook` if you want them - it only includes objects for the parts it knows you want.

This is good:

```cpp
#include <plugin-natives/NativeFunc>
#include <plugin-natives/NativeHook>
#include <plugin-natives/NativeMain>
```

These are bad:

```cpp
#include <plugin-natives/NativeFunc>
#include <plugin-natives/NativeMain>
#include <plugin-natives/NativeHook>
```

```cpp
#include <plugin-natives/NativeMain>
#include <plugin-natives/NativeHook>
#include <plugin-natives/NativeFunc>
```

etc.

You also need to add a call to `plugin_natives::AmxLoad(amx);` in `AmxLoad`.  If all your natives use this method, return it:

```cpp
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return plugin_natives::AmxLoad(amx);
}
```

If you are only importing nativs, not declaring any, you don't need `NativeMain` or `plugin_natives::AmxLoad`.

### Calls

If you want to call a native or hook directly from your code, that is very easy:

```cpp
PLUGIN_NATIVE(my_namespace, Native2, bool(int id))
{
	return id == 42;
}

PLUGIN_NATIVE(my_namespace, Native1, bool(int id))
{
	return Native2(id);
}
```

If you want to call an original native, bypassing the hook, dereference it first:

```cpp
PLUGIN_HOOK(my_namespace, SetPlayerPos, bool(int playerid, float x, float y, float z))
{
	logprintf("my_namespace::SetPlayerPos called");
	return sampgdk::SetPlayerPos(playerid, x, y, z),
}

PLUGIN_NATIVE(my_namespace, SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a))
{
	(*SetPlayerPos)(playerid, x, y, z);
	return sampgdk::SetPlayerAngle(playerid, a);
}
```

That code will NOT print `my_namespace::SetPlayerPos called` because we bypassed the hook.  This will:

```cpp
PLUGIN_HOOK(my_namespace, SetPlayerPos, bool(int playerid, float x, float y, float z))
{
	logprintf("my_namespace::SetPlayerPos called");
	return sampgdk::SetPlayerPos(playerid, x, y, z),
}

PLUGIN_NATIVE(my_namespace, SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a))
{
	SetPlayerPos(playerid, x, y, z);
	return sampgdk::SetPlayerAngle(playerid, a);
}
```

Importantly, so will this - proving that hooks work even if you just call the original AMX functions from plugins:

```cpp
PLUGIN_HOOK(my_namespace, SetPlayerPos, bool(int playerid, float x, float y, float z))
{
	logprintf("my_namespace::SetPlayerPos called");
	return sampgdk::SetPlayerPos(playerid, x, y, z),
}

PLUGIN_NATIVE(my_namespace, SetPlayerPosAndAngle, bool(int playerid, float x, float y, float z, float a))
{
	sampgdk::SetPlayerPos(playerid, x, y, z);
	return sampgdk::SetPlayerAngle(playerid, a);
}
```

You can deal with the namespaces however you like - `using` or not.  Note that `plugin_natives` is a separate namespace to the one specified in your declarations, it holds the functions used to initialise the system itself.

### Logging

You can add debugging to the system by defining macros first.  For example:

```cpp
#define LOG_NATIVE_ERROR(...)   logprintf("ERROR: " __VA_ARGS__)
#define LOG_NATIVE_WARNING(...) logprintf("WARNING: " __VA_ARGS__)
#define LOG_NATIVE_DEBUG(...)   logprintf("DEBUG: " __VA_ARGS__)
#define LOG_NATIVE_INFO(...)    logprintf("INFO: " __VA_ARGS__)

#include <plugin-natives/NativeFunc>
#include <plugin-natives/NativeHook>
```

Or using [samp-log-core](https://github.com/maddinat0r/samp-log-core) (note that you may need a recent branch to allow variable parameters in the calls):

```cpp
#define LOG_NATIVE_ERROR(...)   gMyLogger(LogLevel::ERROR, __VA_ARGS__)
#define LOG_NATIVE_WARNING(...) gMyLogger(LogLevel::WARNING, __VA_ARGS__)
#define LOG_NATIVE_DEBUG(...)   gMyLogger(LogLevel::DEBUG, __VA_ARGS__)
#define LOG_NATIVE_INFO(...)    gMyLogger(LogLevel::INFO, __VA_ARGS__)

#include <plugin-natives/NativeFunc>
#include <plugin-natives/NativeHook>
```

You will also need to do that each time you include one of the headers in to a new file, so I suggest wrapping the whole lot in a new include.

