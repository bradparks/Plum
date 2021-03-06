#include "../core/audio.h"
#include "script.h"

namespace plum
{
    namespace script
    {
        template<> const char* meta<Sound>()
        {
            return "plum.Sound";
        }

        void initSoundObject(lua_State* L)
        {
            luaL_newmetatable(L, meta<Sound>());
            // Duplicate the metatable on the stack.
            lua_pushvalue(L, -1);
            // metatable.__index = metatable
            lua_setfield(L, -2, "__index");

            // Put the members into the metatable.
            const luaL_Reg functions[] = {
                {"__gc", [](lua_State* L) { return script::wrapped<Sound>(L, 1)->gc(L); }},
                {"__index", [](lua_State* L) { return script::wrapped<Sound>(L, 1)->index(L); }},
                {"__newindex", [](lua_State* L) { return script::wrapped<Sound>(L, 1)->newindex(L); }},
                {"__tostring", [](lua_State* L) { return script::wrapped<Sound>(L, 1)->tostring(L); }},
                {"__pairs", [](lua_State* L) { return script::wrapped<Sound>(L, 1)->pairs(L); }},
                {"play", [](lua_State* L)
                {
                    auto sound = script::ptr<Sound>(L, 1);
                    auto volume = script::get<double>(L, 2, 1.0);
                    auto pan = script::get<double>(L, 3, 0.0);
                    auto pitch = script::get<double>(L, 4, 1.0);

                    auto chan = new Channel();
                    script::instance(L).audio().loadChannel(*sound, false, *chan);
                    chan->setVolume(volume);
                    chan->setPan(pan);
                    chan->setPitch(pitch);
                    chan->play();
                    
                    script::push(L, chan, LUA_NOREF);
                    return 1;
                }},
                {nullptr, nullptr}
            };
            luaL_setfuncs(L, functions, 0);
            lua_pop(L, 1);

            // Push plum namespace.
            lua_getglobal(L, "plum");

            // plum.Sound = <function create>
            script::push(L, "Sound");
            lua_pushcfunction(L, [](lua_State* L)
            {
                auto filename = script::get<const char*>(L, 1);
                auto sound = new Sound();
                script::instance(L).audio().loadSound(filename, *sound);
                script::push(L, sound, LUA_NOREF);
                return 1;
            });
            lua_settable(L, -3);

            // Pop plum namespace.
            lua_pop(L, 1);
        }
    }
}