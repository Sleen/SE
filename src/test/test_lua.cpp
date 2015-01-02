
#include "lua.hpp"

static int log(lua_State *L)
{
	int n = lua_gettop(L);
	string s = "";

	string sep = " ";
	
	for (int i = 1; i <= n; i++)
	{
		if (lua_isstring(L, i))
			s += lua_tostring(L, i) + sep;
		else if (lua_isnumber(L, i))
			s += toStr(lua_tonumber(L, i)) + sep;
		else if (lua_isboolean(L, i))
			s += toStr(lua_toboolean(L, i)) + sep;
		else
		{
			lua_pushstring(L, "Incorrect argument to 'log'");
			lua_error(L);
			return 0;
		}
	}

	s += '\n';

	DEBUG_LOG(s.c_str());

	return 0;
}


int main(){
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	
	lua_register(L, "log", log);

	string code = R"(
log("time: ", os.date())
)";

	int status = luaL_dostring(L, code.c_str());
	if (status != LUA_OK) {
		const char *msg = (lua_type(L, -1) == LUA_TSTRING) ? lua_tostring(L, -1)
			: NULL;
		if (msg == NULL) msg = "(error object is not a string)";
		luai_writestringerror("%s\n", msg);
		lua_pop(L, 1);
	}

	lua_close(L);
}