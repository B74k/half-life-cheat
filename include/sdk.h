#ifndef SDK_H
#define SDK_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef int qboolean;
typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec2_t[2];
typedef unsigned char byte;

#define VectorCopy(src, dst) ((dst)[0]=(src)[0], (dst)[1]=(src)[1], (dst)[2]=(src)[2])
#define VectorClear(v) ((v)[0]=0, (v)[1]=0, (v)[2]=0)

#define FL_ONGROUND (1 << 9)
#define IN_JUMP     (1 << 1)

typedef struct { byte r, g, b; } color24;

typedef struct {
    int entityType, number;
    float msg_time;
    int messagenum;
    vec3_t origin, angles;
    int modelindex, sequence;
    float frame;
    int colormap;
    short skin, solid;
    int effects, flags;
    float scale;
    byte eflags;
    int rendermode, renderamt;
    color24 rendercolor;
    int renderfx, movetype;
    float animtime, framerate;
    int body;
    byte controller[4], blending[4];
    vec3_t velocity, mins, maxs;
    int aiment, owner;
    float friction, gravity;
    int team, playerclass, health;
    qboolean spectator;
    int weaponmodel, gaitsequence;
    vec3_t basevelocity;
    int usehull, oldbuttons, onground, iStepLeft;
    float flFallVelocity, fov;
    int weaponanim;
    vec3_t startpos, endpos;
    float impacttime, starttime;
    int iuser1, iuser2, iuser3, iuser4;
    float fuser1, fuser2, fuser3, fuser4;
    vec3_t vuser1, vuser2, vuser3, vuser4;
} entity_state_t;

typedef struct cl_entity_s {
    int index;
    qboolean player, curstate_valid;
    entity_state_t baseline, prevstate, curstate;
    vec3_t origin, angles;
    vec3_t attachment[4];
    float latched_prevanimtime, latched_prevframe, latched_prevsequence;
    float latched_prevblending[2], latched_prevcontroller[4];
    float mouth_open, syncbase;
    void* model;
    void* efrag;
    int topcolor, bottomcolor, trivial_accept;
    void *visframe, *dlightframe;
    int dlightbits;
} cl_entity_t;

typedef struct {
    short lerp_msec;
    unsigned char msec;
    vec3_t viewangles;
    float forwardmove, sidemove, upmove;
    unsigned char lightlevel;
    unsigned short buttons;
    unsigned char impulse, weaponselect;
    int impact_index;
    vec3_t impact_position;
} usercmd_t;

typedef struct {
    int player_index;
    qboolean server, multiplayer;
    float time, frametime;
    vec3_t forward, right, up;
    vec3_t origin, angles, oldangles;
    vec3_t velocity, movedir, basevelocity, view_ofs;
    float flDuckTime;
    qboolean bInDuck;
    int flTimeStepSound, iStepLeft;
    float flFallVelocity;
    vec3_t punchangle;
    float flSwimTime, flNextPrimaryAttack;
    int effects, flags, usehull;
    float gravity, friction;
    int oldbuttons;
    float waterjumptime;
    qboolean dead;
    int deadflag, spectator, movetype, onground;
    int waterlevel, watertype, oldwaterlevel;
    char sztexturename[256], chtexturetype;
    float maxspeed, clientmaxspeed;
    int iuser1, iuser2, iuser3, iuser4;
    float fuser1, fuser2, fuser3, fuser4;
    vec3_t vuser1, vuser2, vuser3, vuser4;
} playermove_t;

typedef struct { int left, right, top, bottom; } wrect_t;
typedef struct { int iSize, iWidth, iHeight, iFlags, iCharHeight; short charWidths[256]; } SCREENINFO;
typedef struct { char* name; short ping; byte thisplayer, spectator, packetloss; char* model; short topcolor, bottomcolor; } hud_player_info_t;

typedef struct {
    int version;
    void (*RenderMode)(int);
    void (*Begin)(int);
    void (*End)(void);
    void (*Color4f)(float, float, float, float);
    void (*Color4ub)(unsigned char, unsigned char, unsigned char, unsigned char);
    void (*TexCoord2f)(float, float);
    void (*Vertex3fv)(float*);
    void (*Vertex3f)(float, float, float);
    void (*Brightness)(float);
    void (*CullFace)(int);
    int (*SpriteTexture)(void*, int);
    int (*WorldToScreen)(float*, float*);
    void (*Fog)(float[3], float, float, int);
    void (*ScreenToWorld)(float*, float*);
    void (*GetMatrix)(int, float*);
    int (*BoxInPVS)(float*, float*);
    void (*LightAtPoint)(float*, float*);
    void (*Color4fRendermode)(float, float, float, float, int);
    void (*FogParams)(float, int);
} triangleapi_t;

typedef struct {
    HINSTANCE (*pfnSPR_Load)(const char*);
    int (*pfnSPR_Frames)(HINSTANCE);
    int (*pfnSPR_Height)(HINSTANCE, int);
    int (*pfnSPR_Width)(HINSTANCE, int);
    void (*pfnSPR_Set)(HINSTANCE, int, int, int);
    void (*pfnSPR_Draw)(int, int, int, const wrect_t*);
    void (*pfnSPR_DrawHoles)(int, int, int, const wrect_t*);
    void (*pfnSPR_DrawAdditive)(int, int, int, const wrect_t*);
    void (*pfnSPR_EnableScissor)(int, int, int, int);
    void (*pfnSPR_DisableScissor)(void);
    void* (*pfnSPR_GetList)(char*, int*);
    void (*pfnFillRGBA)(int, int, int, int, int, int, int, int);
    int (*pfnGetScreenInfo)(SCREENINFO*);
    void (*pfnSetCrosshair)(HINSTANCE, wrect_t, int, int, int);
    void* (*pfnRegisterVariable)(char*, char*, int);
    float (*pfnGetCvarFloat)(char*);
    char* (*pfnGetCvarString)(char*);
    int (*pfnAddCommand)(char*, void(*)(void));
    int (*pfnHookUserMsg)(char*, void*);
    int (*pfnServerCmd)(char*);
    int (*pfnClientCmd)(char*);
    void (*pfnGetPlayerInfo)(int, hud_player_info_t*);
    void (*pfnPlaySoundByName)(char*, float);
    void (*pfnPlaySoundByIndex)(int, float);
    void (*pfnAngleVectors)(const vec3_t, vec3_t, vec3_t, vec3_t);
    void* (*pfnTextMessageGet)(const char*);
    int (*pfnDrawCharacter)(int, int, int, int, int, int);
    int (*pfnDrawConsoleString)(int, int, char*);
    void (*pfnDrawSetTextColor)(float, float, float);
    void (*pfnDrawConsoleStringLen)(const char*, int*, int*);
    void (*pfnConsolePrint)(const char*);
    void (*pfnCenterPrint)(const char*);
    int (*GetWindowCenterX)(void);
    int (*GetWindowCenterY)(void);
    void (*GetViewAngles)(vec3_t);
    void (*SetViewAngles)(vec3_t);
    int (*GetMaxClients)(void);
    void (*Cvar_SetValue)(const char*, float);
    int (*Cmd_Argc)(void);
    char* (*Cmd_Argv)(int);
    void (*Con_Printf)(const char*, ...);
    void (*Con_DPrintf)(const char*, ...);
    void (*Con_NPrintf)(int, const char*, ...);
    void (*Con_NXPrintf)(void*, const char*, ...);
    const char* (*PhysInfo_ValueForKey)(const char*);
    const char* (*ServerInfo_ValueForKey)(const char*);
    float (*GetClientMaxspeed)(void);
    int (*CheckParm)(const char*, char**);
    void (*Key_Event)(int, int);
    void (*GetMousePosition)(int*, int*);
    int (*IsNoClipping)(void);
    cl_entity_t* (*GetLocalPlayer)(void);
    cl_entity_t* (*GetViewModel)(void);
    cl_entity_t* (*GetEntityByIndex)(int);
    float (*GetClientTime)(void);
    void (*V_CalcShake)(void);
    void (*V_ApplyShake)(vec3_t, vec3_t, float);
    int (*PM_PointContents)(vec3_t, int*);
    int (*PM_WaterEntity)(vec3_t);
    void* (*PM_TraceLine)(vec3_t, vec3_t, int, int, int);
    void* (*CL_LoadModel)(const char*, int*);
    int (*CL_CreateVisibleEntity)(int, cl_entity_t*);
    const void* (*GetSpritePointer)(HINSTANCE);
    void (*pfnPlaySoundByNameAtLocation)(char*, float, vec3_t);
    unsigned short (*pfnPrecacheEvent)(int, const char*);
    void (*pfnPlaybackEvent)(int, const void*, unsigned short, float, vec3_t, vec3_t, float, float, int, int, int, int);
    void (*pfnWeaponAnim)(int, int);
    float (*pfnRandomFloat)(float, float);
    int (*pfnRandomLong)(int, int);
    void (*pfnHookEvent)(char*, void(*)(void*));
    int (*Con_IsVisible)(void);
    const char* (*pfnGetGameDirectory)(void);
    void* (*pfnGetCvarPointer)(const char*);
    const char* (*Key_LookupBinding)(const char*);
    const char* (*pfnGetLevelName)(void);
    void (*pfnGetScreenFade)(void*);
    void (*pfnSetScreenFade)(void*);
    void* (*VGui_GetPanel)(void);
    void (*VGui_ViewportPaintBackground)(int[4]);
    byte* (*COM_LoadFile)(char*, int, int*);
    char* (*COM_ParseFile)(char*, char*);
    void (*COM_FreeFile)(void*);
    triangleapi_t* pTriAPI;
    void *pEfxAPI, *pEventAPI, *pDemoAPI, *pNetAPI, *pVoiceTweak;
    int (*IsSpectateOnly)(void);
    void* (*LoadMapSprite)(const char*);
    void (*COM_AddAppDirectoryToSearchPath)(const char*, const char*);
    int (*COM_ExpandFilename)(const char*, char*, int);
    const char* (*PlayerInfo_ValueForKey)(int, const char*);
    void (*PlayerInfo_SetValueForKey)(const char*, const char*);
    qboolean (*GetPlayerUniqueID)(int, char[16]);
    int (*GetTrackerIDForPlayer)(int);
    int (*GetPlayerForTrackerID)(int);
    int (*pfnServerCmdUnreliable)(char*);
    void (*GetMousePos)(void*);
    void (*SetMousePos)(int, int);
    void (*SetMouseEnable)(qboolean);
} cl_enginefunc_t;

typedef void (*CL_CreateMove_t)(float, usercmd_t*, int);
typedef int (*HUD_Redraw_t)(float, int);
typedef void (*HUD_PlayerMove_t)(playermove_t*, int);
typedef int (*Initialize_t)(cl_enginefunc_t*, int);
typedef void (*HUD_Init_t)(void);

typedef struct {
    Initialize_t pInitFunc;
    HUD_Init_t pHudInitFunc;
    int (*pHudVidInitFunc)(void);
    HUD_Redraw_t pHudRedrawFunc;
    int (*pHudUpdateClientDataFunc)(void*, float);
    void (*pHudResetFunc)(void);
    HUD_PlayerMove_t pClientMove;
    void *pClientMoveInit, *pClientTextureType;
    void (*pIN_ActivateMouse)(void);
    void (*pIN_DeactivateMouse)(void);
    void (*pIN_MouseEvent)(int);
    void (*pIN_ClearStates)(void);
    void (*pIN_Accumulate)(void);
    CL_CreateMove_t pCL_CreateMoveFunc;
    int (*pCL_IsThirdPerson)(void);
    void (*pCL_GetCameraOffsets)(vec3_t);
    void* (*pFindKey)(const char*);
    void (*pCamThink)(void);
    void* pCalcRefdef;
    void* pAddEntity;
    void (*pCreateEntities)(void);
    void (*pDrawNormalTriangles)(void);
    void (*pDrawTransparentTriangles)(void);
    void (*pStudioEvent)(const void*, const cl_entity_t*);
    void* pPostRunCmd;
    void (*pShutdown)(void);
    void (*pTxferLocalOverrides)(void*, const void*);
    void (*pProcessPlayerState)(void*, const void*);
    void (*pTxferPredictionData)(void*, void*, void*, const void*, const void*, const void*);
    void (*pDemo_ReadBuffer)(int, unsigned char*);
    int (*pConnectionlessPacket)(void*, const char*, char*, int*);
    int (*pGetHullBounds)(int, vec3_t, vec3_t);
    void* pHudFrame;
    void* pKeyEvent;
    void (*pTempEntUpdate)(double, double, double, void**, void**, int(*)(cl_entity_t*), void(*)(void*, float));
    void* (*pGetUserEntity)(int);
    void* pVoiceStatus;
    void (*pDirectorMessage)(int, void*);
    int (*pGetStudioModelInterface)(int, void**, void*);
    void (*pChatInputPosition)(int*, int*);
    int (*pGetPlayerTeam)(int);
    void* (*pClientFactory)(void);
} cldll_func_t;

extern cl_enginefunc_t* g_engine;
extern playermove_t* g_player_move;
extern SCREENINFO g_screen;

#endif
