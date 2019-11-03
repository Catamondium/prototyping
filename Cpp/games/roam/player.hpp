#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "gameobject.hpp"
#include "vec.hpp"
#include "common.hpp"

static int getppos(lua_State *L);
static int setppos(lua_State *L);
static int getpscore(lua_State *L);
static int player2string(lua_State *L);
static const struct luaL_Reg playerlib_m [] = {
    {"getpos", getppos},
    {"setpos", setppos},
    {"score", getpscore},
    {"__tostring", player2string},
    {NULL, NULL}
};

struct Player {
    size_t score = 0;
    Vec pos = Vec{0, 0};

    /// Build Player closure
    void lua_serialize(lua_State *L) {
        Player *p = (Player *)lua_newuserdata(L, sizeof(Player));
        p->pos = pos;
        p->score = score;
        luaL_setmetatable(L, "player");
        
        lua_getglobal(L, "roam");
        lua_swap(L);
        lua_setfield(L, -2, "player");
    }

    void lua_refresh(lua_State *L) {
        lua_getglobal(L, "roam");
        lua_getfield(L, -1, "player");

        Player *p = (Player *)luaL_checkudata(L, -1, "player");
        pos.x = p->pos.x;
        pos.y = p->pos.y;
        score = p->score;
    }

    static void open(lua_State *L) {
        // player { __index = self }
        luaL_newmetatable(L, "player");
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);

        //lua_newtable(L);
        assert(lua_istable(L, -1));
        luaL_setfuncs(L, playerlib_m, 0);
    }
};

static int player2string(lua_State *L) {
    Player *p = (Player *)luaL_checkudata(L, -1, "player");
    lua_pushfstring(L, "Player(%d, pos{%d, %d})", p->score, p->pos.x, p->pos.y);
    return 1;
}

static int getppos(lua_State *L){
    Player *p = (Player *)luaL_checkudata(L, -1, "player");
    lua_newtable(L);
    lua_pushinteger(L, p->pos.x);
    lua_setfield(L, -2, "x");

    lua_pushinteger(L, p->pos.y);
    lua_setfield(L, -2, "y");
    return 1;
}

static int setppos(lua_State *L) {
    Player *p = (Player *)luaL_checkudata(L, 1, "player");
    int x = p->pos.x;
    int y = p->pos.y;

    lua_getfield(L, 2, "x");
    if (lua_isinteger(L, -1)) {
        x = luaL_checkinteger(L, -1);
    }

    lua_getfield(L, 2, "y");
    if (lua_isinteger(L, -1)) {
        y = luaL_checkinteger(L, -1);
    }

    p->pos.x = x;
    p->pos.y = y;
    return 0;
}

static int getpscore(lua_State *L) {
    Player *p = (Player *)luaL_checkudata(L, -1, "player");
    lua_pushinteger(L, p->score);
    return 1;
}