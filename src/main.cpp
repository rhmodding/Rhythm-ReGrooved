#include "hk/gfx/DebugRenderer.h"
#include "hk/hook/Trampoline.h"

extern "C"{
#include "lua.h"
#include "lauxlib.h"
}

// TODO: figure out better solution
FILE* const stdin = NULL;

#include "nvn/nvn_Cpp.h"

lua_State* gLua = nullptr;

char logData[0x1000] = {0};
int result = 999;
int result2 = 999;

static int lua_log(lua_State *L) {
    int n = lua_gettop(L);

    if (n != 1 || !lua_isstring(L, 1))
        lua_error(L);

    const char* data = lua_tostring(L, 1);
    
    strncat(logData, data, 0x1000);

    return 0;
}

HkTrampoline gameInit = [](TrampolineStatic(), void* a1) -> void {
    gLua = lua_newstate(luaL_alloc, NULL, 0x5417);
    lua_register(gLua, "log", lua_log);

    orig(a1);
};

HkTrampoline LMS_GetTextHook = [](TrampolineStatic(), void* a1, int a2) -> int {
    return orig(a1, gLua ? 5 : 4);
};

struct someRenderClass {
    char unk_0[0xb0 - 0x0];
    nvn::CommandBuffer* commandBuffer;
    // ...

    void endCommandBufferRecording();
};

// Runs every frame
void renderHook(someRenderClass* _this) {
   // defer {ret = cmdBuf->EndRecording(); };

   // hk::svc::OutputDebugString("Hi!\n", 5);

    /* DebugRenderer */
    auto* renderer = hk::gfx::DebugRenderer::instance();

    renderer->clear();
    renderer->begin(_this->commandBuffer);

    renderer->setGlyphSize(1);

    renderer->drawQuad(
        { { 30, 30 }, { 0, 0 }, 0xef000000 },
        { { 500, 30 }, { 1.0, 0 }, 0xef000000 },
        { { 500, 300 }, { 1.0, 1.0 }, 0xef000000 },
        { { 30, 300 }, { 0, 1.0 }, 0xef000000 });

    renderer->setCursor({ 50, 50 });
    renderer->printf("%s\n%d %d", logData, result, result2);

    renderer->end();

    _this->endCommandBufferRecording();

   // return _this->commandBuffer->EndRecording();

}

const char* a = R"(
function a()
    log("shit")
end
log("fuck")
)";

HkTrampoline cueTestHook = [](TrampolineStatic(), void* self, void* seq) -> void {
    luaL_loadbuffer(gLua, a, strlen(a)+1, "_main");
    result2 = luaL_dostring(gLua,a);
    result = lua_getglobal(gLua, "a");
    lua_pcall(gLua, 0, 0, 0);

    orig(self, seq);
};

extern "C" void hkMain() {
    gameInit.installAtMainOffset(0x50cba0); // GameRun::GameRun(GameRun*)
    //LMS_GetTextHook.installAtMainOffset(0x4f0ce0); // LMS_GetText

    hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x510b6c, &renderHook);

    cueTestHook.installAtMainOffset(0x42c0c0);

    hk::gfx::DebugRenderer::instance()->installHooks();
}


