#include "../plum.h"

namespace Plum
{
	namespace ScriptLibrary
	{
		namespace ImageObject
		{
			SCRIPT_OBJ_GETTER(index, Wrapper<Image>*, libraryName)
			SCRIPT_OBJ_SETTER(newindex, Wrapper<Image>*, libraryName)

			int create(lua_State* L)
			{
				if(lua_isstring(L, 1))
				{
					const char* filename = lua_tostring(L, 1);

					PLUM_PUSH_DATA(L, Image, new Image(filename), LUA_NOREF);

					return 1;
				}
				else if(PLUM_IS_DATA(L, 1, Canvas))
				{
					Wrapper<Canvas>* canvas = PLUM_CHECK_DATA(L, 1, Canvas);
					PLUM_PUSH_DATA(L, Image, new Image(canvas->data), LUA_NOREF);

					return 1;
				}
				luaL_error(L, "Attempt to call plum.Image constructor with invalid argument types.\r\nMust be (string filename) or (plum.Canvas canvas).");
				return 0;
			}

			int gc(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);

				// Only delete if it doesn't belong to a parent of some sort.
				if(t->parentRef != LUA_NOREF)
				{
					delete t->data;
				}
				else
				{
					luaL_unref(L, LUA_REGISTRYINDEX, t->parentRef);
				}

				return 0;
			}

			int tostring(lua_State* L)
			{
				PLUM_CHECK_DATA(L, 1, Image);
				lua_pushstring(L, "(plum.Image object)");
				return 1;
			}

			int blit(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int x = luaL_checkint(L, 2);
				int y = luaL_checkint(L, 3);
				BlendMode mode = (BlendMode) luaL_optint(L, 4, BlendUnspecified);
				Color tint = luaL_optint(L, 5, Color::White);

				t->data->blit(x, y, mode, tint);

				return 0;
			}

			int blitRegion(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int sx = luaL_checkint(L, 2);
				int sy = luaL_checkint(L, 3);
				int sx2 = luaL_checkint(L, 4);
				int sy2 = luaL_checkint(L, 5);
				int dx = luaL_checkint(L, 6);
				int dy = luaL_checkint(L, 7);
				BlendMode mode = (BlendMode) luaL_optint(L, 8, BlendUnspecified);
				Color tint = luaL_optint(L, 9, Color::White);

				t->data->blitRegion(sx, sy, sx2, sy2, dx, dy, mode, tint);

				return 0;
			}

			int scaleBlit(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int x = luaL_checkint(L, 2);
				int y = luaL_checkint(L, 3);
				int width = luaL_checkint(L, 4);
				int height = luaL_checkint(L, 5);
				BlendMode mode = (BlendMode) luaL_optint(L, 6, BlendUnspecified);
				Color tint = luaL_optint(L, 7, Color::White);

				t->data->scaleBlit(x, y, width, height, mode, tint);

				return 0;
			}

			int scaleBlitRegion(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int sx = luaL_checkint(L, 2);
				int sy = luaL_checkint(L, 3);
				int sx2 = luaL_checkint(L, 4);
				int sy2 = luaL_checkint(L, 5);
				int dx = luaL_checkint(L, 6);
				int dy = luaL_checkint(L, 7);
				int scaledWidth = luaL_checkint(L, 8);
				int scaledHeight = luaL_checkint(L, 9);
				BlendMode mode = (BlendMode) luaL_optint(L, 10, BlendUnspecified);
				Color tint = luaL_optint(L, 11, Color::White);

				t->data->scaleBlitRegion(sx, sy, sx2, sy2, dx, dy, scaledWidth, scaledHeight, mode, tint);

				return 0;
			}

			int rotateBlit(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int x = luaL_checkint(L, 2);
				int y = luaL_checkint(L, 3);
				double angle = luaL_checknumber(L, 4);
				BlendMode mode = (BlendMode) luaL_optint(L, 5, BlendUnspecified);
				Color tint = luaL_optint(L, 6, Color::White);

				t->data->rotateBlit(x, y, angle, mode, tint);

				return 0;
			}

			int rotateBlitRegion(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int sx = luaL_checkint(L, 2);
				int sy = luaL_checkint(L, 3);
				int sx2 = luaL_checkint(L, 4);
				int sy2 = luaL_checkint(L, 5);
				int dx = luaL_checkint(L, 6);
				int dy = luaL_checkint(L, 7);
				double angle = luaL_checknumber(L, 8);
				BlendMode mode = (BlendMode) luaL_optint(L, 9, BlendUnspecified);
				Color tint = luaL_optint(L, 10, Color::White);

				t->data->rotateBlitRegion(sx, sy, sx2, sy2, dx, dy, angle, mode, tint);
				return 0;
			}

			int rotateScaleBlit(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int x = luaL_checkint(L, 2);
				int y = luaL_checkint(L, 3);
				double angle = luaL_checknumber(L, 4);
				double scale = luaL_checknumber(L, 5);
				BlendMode mode = (BlendMode) luaL_optint(L, 6, BlendUnspecified);
				Color tint = luaL_optint(L, 7, Color::White);

				t->data->rotateScaleBlit(x, y, angle, scale, mode, tint);

				return 0;
			}

			int rotateScaleBlitRegion(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				int sx = luaL_checkint(L, 2);
				int sy = luaL_checkint(L, 3);
				int sx2 = luaL_checkint(L, 4);
				int sy2 = luaL_checkint(L, 5);
				int dx = luaL_checkint(L, 6);
				int dy = luaL_checkint(L, 7);
				double angle = luaL_checknumber(L, 8);
				double scale = luaL_checknumber(L, 9);
				BlendMode mode = (BlendMode) luaL_optint(L, 10, BlendUnspecified);
				Color tint = luaL_optint(L, 11, Color::White);

				t->data->rotateScaleBlitRegion(sx, sy, sx2, sy2, dx, dy, angle, scale, mode, tint);
				return 0;
			}

			int refresh(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				t->data->refresh();

				return 1;
			}

			int getwidth(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				lua_pushnumber(L, t->data->getCanvasWidth());

				return 1;
			}

			int getheight(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				lua_pushnumber(L, t->data->getCanvasHeight());

				return 1;
			}

			int gettrueWidth(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				lua_pushnumber(L, t->data->getTextureWidth());

				return 1;
			}

			int gettrueHeight(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image);
				lua_pushnumber(L, t->data->getTextureHeight());

				return 1;
			}

			int getcanvas(lua_State* L)
			{
				Wrapper<Image>* t = PLUM_CHECK_DATA(L, 1, Image); 

				// Push reference to this, so the image stays around
				// as long as it's required for the child.
				lua_pushvalue(L, 1);
				int ref = luaL_ref(L, LUA_REGISTRYINDEX);

				PLUM_PUSH_DATA(L, Canvas, t->data->getCanvas(), LUA_NOREF);
				return 1;
			}

			void openLibrary(lua_State* L)
			{
				luaL_newmetatable(L, libraryName);
				// Duplicate the metatable on the stack.
				lua_pushvalue(L, -1);
				// metatable.__index = metatable
				lua_setfield(L, -2, "__index");

				// Put the members into the metatable.
				PLUM_BIND_FUNC_BEGIN()
				PLUM_BIND_META(gc)
				PLUM_BIND_META(index)
				PLUM_BIND_META(newindex)
				PLUM_BIND_META(tostring)
				PLUM_BIND_FUNC(blit)
				PLUM_BIND_FUNC(blitRegion)
				PLUM_BIND_FUNC(scaleBlit)
				PLUM_BIND_FUNC(scaleBlitRegion)
				PLUM_BIND_FUNC(rotateBlit)
				PLUM_BIND_FUNC(rotateBlitRegion)
				PLUM_BIND_FUNC(rotateScaleBlit)
				PLUM_BIND_FUNC(rotateScaleBlitRegion)
				PLUM_BIND_FUNC(refresh)
				PLUM_BIND_FUNC(getwidth)
				PLUM_BIND_FUNC(getheight)
				PLUM_BIND_FUNC(getcanvas)
				PLUM_BIND_FUNC(gettrueWidth)
				PLUM_BIND_FUNC(gettrueHeight)
				PLUM_BIND_FUNC_END_NULL()

				lua_pop(L, 1);
				
				// Push plum namespace.
				lua_getglobal(L, "plum");

				// plum.Image = <function create>
				lua_pushstring(L, "Image");
				lua_pushcfunction(L, create);
				lua_settable(L, -3);

				// Pop plum namespace.
				lua_pop(L, 1);
			}
		}
	}
}