
#ifndef EDUKE32_EVENTS_DEFS_H_
#define EDUKE32_EVENTS_DEFS_H_

// the order of these can't be changed or else compatibility with EDuke 2.0 mods will break
// KEEPINSYNC with EventNames[] and lunatic/con_lang.lua
enum GameEvent_t {
    EVENT_INIT,  // 0
    EVENT_ENTERLEVEL,
    EVENT_RESETWEAPONS,
    EVENT_RESETINVENTORY,
    EVENT_HOLSTER,
    EVENT_LOOKLEFT,  // 5
    EVENT_LOOKRIGHT,
    EVENT_SOARUP,
    EVENT_SOARDOWN,
    EVENT_CROUCH,
    EVENT_JUMP,  // 10
    EVENT_RETURNTOCENTER,
    EVENT_LOOKUP,
    EVENT_LOOKDOWN,
    EVENT_AIMUP,
    EVENT_FIRE,  // 15
    EVENT_CHANGEWEAPON,
    EVENT_GETSHOTRANGE,
    EVENT_GETAUTOAIMANGLE,
    EVENT_GETLOADTILE,
    EVENT_CHEATGETSTEROIDS,  // 20
    EVENT_CHEATGETHEAT,
    EVENT_CHEATGETBOOT,
    EVENT_CHEATGETSHIELD,
    EVENT_CHEATGETSCUBA,
    EVENT_CHEATGETHOLODUKE,  // 25
    EVENT_CHEATGETJETPACK,
    EVENT_CHEATGETFIRSTAID,
    EVENT_QUICKKICK,
    EVENT_INVENTORY,
    EVENT_USENIGHTVISION,  // 30
    EVENT_USESTEROIDS,
    EVENT_INVENTORYLEFT,
    EVENT_INVENTORYRIGHT,
    EVENT_HOLODUKEON,
    EVENT_HOLODUKEOFF,  // 35
    EVENT_USEMEDKIT,
    EVENT_USEJETPACK,
    EVENT_TURNAROUND,
    EVENT_DISPLAYWEAPON,
    EVENT_FIREWEAPON,  // 40
    EVENT_SELECTWEAPON,
    EVENT_MOVEFORWARD,
    EVENT_MOVEBACKWARD,
    EVENT_TURNLEFT,
    EVENT_TURNRIGHT,  // 45
    EVENT_STRAFELEFT,
    EVENT_STRAFERIGHT,
    EVENT_WEAPKEY1,
    EVENT_WEAPKEY2,
    EVENT_WEAPKEY3,  // 50
    EVENT_WEAPKEY4,
    EVENT_WEAPKEY5,
    EVENT_WEAPKEY6,
    EVENT_WEAPKEY7,
    EVENT_WEAPKEY8,  // 55
    EVENT_WEAPKEY9,
    EVENT_WEAPKEY10,
    EVENT_DRAWWEAPON,
    EVENT_DISPLAYCROSSHAIR,
    EVENT_DISPLAYREST,  // 60
    EVENT_DISPLAYSBAR,
    EVENT_RESETPLAYER,
    EVENT_INCURDAMAGE,
    EVENT_AIMDOWN,
    EVENT_GAME,  // 65
    EVENT_PREVIOUSWEAPON,
    EVENT_NEXTWEAPON,
    EVENT_SWIMUP,
    EVENT_SWIMDOWN,
    EVENT_GETMENUTILE,  // 70
    EVENT_SPAWN,
    EVENT_LOGO,
    EVENT_EGS,
    EVENT_DOFIRE,
    EVENT_PRESSEDFIRE,  // 75
    EVENT_USE,
    EVENT_PROCESSINPUT,
    EVENT_FAKEDOMOVETHINGS,
    EVENT_DISPLAYROOMS,
    EVENT_KILLIT,  // 80
    EVENT_LOADACTOR,
    EVENT_DISPLAYBONUSSCREEN,
    EVENT_DISPLAYMENU,
    EVENT_DISPLAYMENUREST,
    EVENT_DISPLAYLOADINGSCREEN,  // 85
    EVENT_ANIMATESPRITES,
    EVENT_NEWGAME,
    EVENT_SOUND,
    EVENT_CHECKTOUCHDAMAGE,
    EVENT_CHECKFLOORDAMAGE,  // 90
    EVENT_LOADGAME,
    EVENT_SAVEGAME,
    EVENT_PREGAME,
    EVENT_CHANGEMENU,
    EVENT_DAMAGEHPLANE,  // 95
    EVENT_ACTIVATECHEAT,
    EVENT_DISPLAYINACTIVEMENU,
    EVENT_DISPLAYINACTIVEMENUREST,
    EVENT_CUTSCENE,
    EVENT_DISPLAYCURSOR,
    EVENT_DISPLAYLEVELSTATS,
    EVENT_DISPLAYCAMERAOSD,
    EVENT_DISPLAYROOMSCAMERA,
    EVENT_DISPLAYSTART,
    EVENT_WORLD,
    EVENT_PREWORLD,
    EVENT_PRELEVEL,
    EVENT_DISPLAYSPIT,
    EVENT_DISPLAYFIST,
    EVENT_DISPLAYKNEE,
    EVENT_DISPLAYKNUCKLES,
    EVENT_DISPLAYSCUBA,
    EVENT_DISPLAYTIP,
    EVENT_DISPLAYACCESS,
    EVENT_MOVESECTOR,
    EVENT_MOVEEFFECTORS,
    EVENT_DISPLAYOVERHEADMAPTEXT,
    EVENT_PRELOADGAME,
    EVENT_POSTSAVEGAME,
    EVENT_PRECUTSCENE,
    EVENT_SKIPCUTSCENE,
    EVENT_SCREEN,
    EVENT_DISPLAYROOMSEND,
    EVENT_DISPLAYEND,
    EVENT_OPENMENUSOUND,
    EVENT_RECOGSOUND,
    EVENT_UPDATESCREENAREA,
    EVENT_DISPLAYBORDER,
    EVENT_SETDEFAULTS,
    EVENT_MAINMENUSCREEN,
    EVENT_NEWGAMESCREEN,
    EVENT_ENDLEVELSCREEN,
    EVENT_EXITGAMESCREEN,
    EVENT_EXITPROGRAMSCREEN,
    EVENT_ALTFIRE,
    EVENT_ALTWEAPON,
    EVENT_DISPLAYOVERHEADMAPPLAYER,
    EVENT_MENUCURSORLEFT,
    EVENT_MENUCURSORRIGHT,
    EVENT_MENUCURSORSHADE,
    EVENT_MENUSHADESELECTED,
    EVENT_PLAYLEVELMUSICSLOT,
    EVENT_CONTINUELEVELMUSICSLOT,
    EVENT_DISPLAYPOINTER,
    EVENT_LASTWEAPON,
    EVENT_DAMAGESPRITE,
    EVENT_POSTDAMAGESPRITE,
    EVENT_DAMAGEWALL,
    EVENT_DAMAGEFLOOR,
    EVENT_DAMAGECEILING,
    EVENT_DISPLAYROOMSCAMERATILE,
    EVENT_RESETGOTPICS,
    EVENT_VALIDATESTART,
    EVENT_NEWGAMECUSTOM,
    EVENT_INITCOMPLETE,
#ifdef EDUKE32_STANDALONE
    EVENT_CAPIR,
#endif
#ifdef LUNATIC
    EVENT_ANIMATEALLSPRITES,
#endif
    MAXEVENTS
};

#endif
