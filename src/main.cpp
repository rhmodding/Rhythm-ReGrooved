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


static int lua_log(lua_State *L) {
    int n = lua_gettop(L);

    if (n != 1 || !lua_isstring(L, 1))
        lua_error(L);

    const char* data = lua_tostring(L, 1);
    
    hk::svc::OutputDebugString(data, strlen(data)+1);
    hk::svc::OutputDebugString("\n", 2);

    return 0;
}

HkTrampoline gameInit = [](TrampolineStatic(), void* a1) -> void {
    orig(a1);
    
    gLua = lua_newstate(luaL_alloc, NULL, 0x5417);

    lua_register(gLua, "log", lua_log);
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

    renderer->setGlyphSize(0.45);

    renderer->drawQuad(
        { { 30, 30 }, { 0, 0 }, 0xef000000 },
        { { 300, 30 }, { 1.0, 0 }, 0xef000000 },
        { { 300, 100 }, { 1.0, 1.0 }, 0xef000000 },
        { { 30, 100 }, { 0, 1.0 }, 0xef000000 });

    renderer->setCursor({ 50, 50 });
    renderer->printf("Rhythm ReGrooved\n\n by techmuse, et al.");

    renderer->end();

    _this->endCommandBufferRecording();

   // return _this->commandBuffer->EndRecording();

}

const char* a = 
    "function a()\n"
    "   log(\"shit\")"
    "end"
;

HkTrampoline cueTestHook = [](TrampolineStatic(), void* self, void* seq) -> void {
    orig(self, seq);
    
    luaL_loadbuffer(gLua, a, strlen(a)+1, "_main");
    lua_pcall(gLua, 0, 0, 0);
};

extern "C" void hkMain() {
    gameInit.installAtMainOffset(0x50cba0); // GameRun::GameRun(GameRun*)
    LMS_GetTextHook.installAtMainOffset(0x4f0ce0); // LMS_GetText

    // this breaks
    // hk::hook::writeBranchLink(hk::ro::getMainModule(), 0x510b6c, &renderHook);

    cueTestHook.installAtMainOffset(0x42c0c0);

    hk::gfx::DebugRenderer::instance()->installHooks();
}


