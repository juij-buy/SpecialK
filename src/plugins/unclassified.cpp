﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
 
// A bunch of stupid "plug-ins," not even worth copyrighting.
//
#include <SpecialK/stdafx.h>

#ifdef  __SK_SUBSYSTEM__
#undef  __SK_SUBSYSTEM__
#endif
#define __SK_SUBSYSTEM__ L"BasicPlugN"


#include <SpecialK/render/d3d9/d3d9_backend.h>
#include <SpecialK/render/d3d11/d3d11_core.h>
#include <SpecialK/render/d3d11/d3d11_state_tracker.h>
#include <SpecialK/plugin/plugin_mgr.h>

struct SK_MemScan_Params__v0
{
  enum Privilege
  {
    Allowed    = true,
    Disallowed = false,
    DontCare   = (DWORD_PTR)-1
  };

  struct
  {
    Privilege execute = DontCare;
    Privilege read    = Allowed;
    Privilege write   = DontCare;
  } privileges;

  enum MemType
  {
    ImageCode  = SEC_IMAGE,
    FileData   = SEC_FILE,
    HeapMemory = SEC_COMMIT
  } mem_type;

  bool testPrivs (const MEMORY_BASIC_INFORMATION& mi)
  {
    if (mi.AllocationProtect == 0)
      return false;

    bool valid = true;


    if (privileges.execute != DontCare)
    {
      bool exec_matches = true;

      switch (mi.Protect)
      {
        case PAGE_EXECUTE:
        case PAGE_EXECUTE_READ:
        case PAGE_EXECUTE_READWRITE:
        case PAGE_EXECUTE_WRITECOPY:
          if (privileges.execute != Allowed)
            exec_matches = false;
          break;

        default:
          if (privileges.execute == Disallowed)
            exec_matches = false;
          break;
      }

      valid &= exec_matches;
    }


    if (privileges.read != DontCare)
    {
      bool read_matches = true;

      switch (mi.Protect)
      {
        case PAGE_READONLY:
        case PAGE_READWRITE:
        case PAGE_EXECUTE_READ:
        case PAGE_EXECUTE_READWRITE:
        case PAGE_EXECUTE_WRITECOPY:
          if (privileges.read != Allowed)
            read_matches = false;
          break;

        default:
          if (privileges.read == Disallowed)
            read_matches = false;
          break;
      }

      valid &= read_matches;
    }


    if (privileges.write != DontCare)
    {
      bool write_matches = true;

      switch (mi.Protect)
      {
        case PAGE_READWRITE:
        case PAGE_WRITECOPY:
        case PAGE_EXECUTE_READWRITE:
        case PAGE_EXECUTE_WRITECOPY:
          if (privileges.write != Allowed)
            write_matches = false;
          break;

        default:
          if (privileges.write == Disallowed)
            write_matches = false;
          break;
      }

      valid &= write_matches;
    }

    return valid;
  }
};


void*
__stdcall
SKX_ScanAlignedEx ( const void* pattern, size_t len,   const void* mask,
                          void* after,   int    align,    uint8_t* base_addr,

                          SK_MemScan_Params__v0 params =
                          SK_MemScan_Params__v0 ()       );

bool
SK_FarCry6_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Far Cry 6", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    static void* pLimitSet =
      (void *)((uintptr_t)
       ( SK_GetProcAddress ( L"FC_m64d3d12.dll",
             "?Count@ReadBytesSkip@AK@@QEAAJXZ" )
       ) + 0x2F342 );
    // FC_m64d3d12.dll.reloc+3A0E02 - 8B 89 BC000000        - mov ecx,[rcx+000000BC]

    static bool patchable = false;
    static bool init      = false;

    if (! init)
    {
      patchable =
        SK_IsAddressExecutable (pLimitSet);

      init = true;

      if (patchable)
      {
        DWORD                                                      dwOrigProt = 0;
        if (VirtualProtect (pLimitSet, 6, PAGE_EXECUTE_READWRITE, &dwOrigProt))
        {
          patchable =
            (! memcmp (pLimitSet, "\x8B\x89\xBC\x00\x00\x00",
                                       6));
            VirtualProtect (pLimitSet, 6, dwOrigProt,             &dwOrigProt);
        }
      }
    }

    if (patchable)
    {
      static bool patched = false;

      if (ImGui::Checkbox ("Disable Framerate Limit", &patched))
      {
        DWORD                                                      dwOrigProt = 0;
        if (VirtualProtect (pLimitSet, 6, PAGE_EXECUTE_READWRITE, &dwOrigProt))
        {
          if (patched)
          {
            memcpy (pLimitSet, "\xB9\x00\x00\x00\x00\x90", 6);
          }

          else
          {
            memcpy (pLimitSet, "\x8B\x89\xBC\x00\x00\x00", 6);
          }
          VirtualProtect (pLimitSet, 6, dwOrigProt, &dwOrigProt);
        }

        else
          patched = (! patched);
      }
    }
    ImGui::TreePop  (  );

    return true;
  }

  return false;
}

bool
SK_GalGun_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Gal*Gun: Double Peace", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static bool emperor_has_no_clothes = false;

    ImGui::TreePush ("");

    if (ImGui::Checkbox ("The emperor of Japan has no clothes", &emperor_has_no_clothes))
    {
      const uint32_t ps_primary = 0x9b826e8a;
      const uint32_t vs_outline = 0x2e1993cf;

      static auto& _Shaders = SK::D3D9::Shaders.get ();

      if (emperor_has_no_clothes)
      {
        _Shaders.vertex.blacklist.emplace (vs_outline);
        _Shaders.pixel.blacklist.emplace  (ps_primary);
      }

      else
      {
        _Shaders.vertex.blacklist.erase (vs_outline);
        _Shaders.pixel.blacklist.erase  (ps_primary);
      }
    }

    if (ImGui::IsItemHovered ())
      ImGui::SetTooltip ( emperor_has_no_clothes ? "And neither do the girls in this game!" :
                                                   "But the prudes in this game do." );

    ImGui::TreePop ();

    return true;
  }

  return false;
}

bool
SK_LSBTS_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Life is Strange: Before the Storm", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static bool evil          = false;
    static bool even_stranger = false;
    static bool wired         = false;

    const uint32_t vs_eyes = 0x223ccf2d;
    const uint32_t ps_face = 0xbde11248;
    const uint32_t ps_skin = 0xa79e425c;

    static auto& shaders =
      SK_D3D11_Shaders.get ();

    ImGui::TreePush ("");

    if (ImGui::Checkbox ("Life is Wired", &wired))
    {
      if (wired)
      {
        shaders.pixel.addTrackingRef (shaders.pixel.wireframe, ps_skin);
        shaders.pixel.addTrackingRef (shaders.pixel.wireframe, ps_face);
      }

      else
      {
        shaders.pixel.releaseTrackingRef (shaders.pixel.wireframe, ps_skin);
        shaders.pixel.releaseTrackingRef (shaders.pixel.wireframe, ps_face);
      }
    }

    if (ImGui::Checkbox ("Life is Evil", &evil))
    {
      if (evil)
      {
        shaders.vertex.addTrackingRef (shaders.vertex.blacklist, vs_eyes);
      }

      else
      {
        shaders.vertex.releaseTrackingRef (shaders.vertex.blacklist, vs_eyes);
      }
    }

    if (ImGui::Checkbox ("Life is Even Stranger", &even_stranger))
    {
      if (even_stranger)
      {
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, ps_face);
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, ps_skin);
      }

      else
      {
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, ps_face);
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, ps_skin);
      }
    }

    //bool enable = evil || even_stranger || wired;
    //
    //SK_D3D11_EnableTracking (enable || show_shader_mod_dlg);

    ImGui::TreePop ();

    return true;
  }

  return false;
}


static const int priority_levels [] =
  { THREAD_PRIORITY_NORMAL,  THREAD_PRIORITY_ABOVE_NORMAL,
    THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL };

#ifdef _M_AMD64
struct SK_FFXV_Thread
{
  ~SK_FFXV_Thread (void) {///noexcept {
    if (hThread)
      SK_CloseHandle (hThread);
  }

  HANDLE               hThread = 0;
  volatile LONG        dwPrio  = THREAD_PRIORITY_NORMAL;

  sk::ParameterInt* prio_cfg   = nullptr;

  void setup (HANDLE __hThread);
};

SK_LazyGlobal <SK_FFXV_Thread> sk_ffxv_swapchain,
                               sk_ffxv_vsync,
                               sk_ffxv_async_run;

typedef DWORD (WINAPI *GetEnvironmentVariableA_pfn)(
  LPCSTR lpName,
  LPCSTR lpBuffer,
  DWORD  nSize
);

GetEnvironmentVariableA_pfn
GetEnvironmentVariableA_Original = nullptr;

DWORD
WINAPI
GetEnvironmentVariableA_Detour ( LPCSTR lpName,
                                 LPSTR  lpBuffer,
                                 DWORD  nSize )
{
  if (_stricmp (lpName, "USERPROFILE") == 0)
  {
    char     szDocs [MAX_PATH + 2] = { };
    strcpy ( szDocs,
               SK_WideCharToUTF8 (SK_GetDocumentsDir ()).c_str () );

    PathRemoveFileSpecA (szDocs);

    if (lpBuffer != nullptr)
    {
      strncpy (lpBuffer, szDocs, nSize);

      //dll_log.Log ( L"GetEnvornmentVariableA (%hs) = %hs",
      //                lpName, lpBuffer );
    }

    return
      (DWORD)strlen (szDocs);
  }

  return
    GetEnvironmentVariableA_Original (lpName, lpBuffer, nSize);
}

void
SK_FFXV_InitPlugin (void)
{
  plugin_mgr->config_fns.emplace (SK_FFXV_PlugInCfg);

  std::unordered_set <uint32_t>
  __SK_FFXV_UI_Pix_Shaders =
  {
    0x224cc7df, 0x7182460b,
    0xe9716459, 0xe7015770,
    0xf15a90ab
  };

  for (                 auto    it : __SK_FFXV_UI_Pix_Shaders)
    SK_D3D11_DeclHUDShader_Pix (it);

  //SK_CreateDLLHook2 (      L"kernel32",
  //                          "GetEnvironmentVariableA",
  //                           GetEnvironmentVariableA_Detour,
  //  static_cast_p2p <void> (&GetEnvironmentVariableA_Original) );
}

bool fix_sleep_0 = false;

extern DWORD WINAPI SleepEx_Detour (DWORD, BOOL);
             static SleepEx_pfn
                    SleepEx_Override = nullptr;

DWORD
WINAPI
SK_FFXV_SleepEx (DWORD dwMilliseconds, BOOL bAlertable)
{
  if ( dwMilliseconds == 0 && fix_sleep_0 )
  {
    SwitchToThread ();
    return 0;
  }

  return
    SleepEx_Override (dwMilliseconds, bAlertable);
}

void
SK_FFXV_Thread::setup (HANDLE __hThread)
{
  HANDLE hThreadCopy;

  if (! DuplicateHandle ( GetCurrentProcess (), __hThread,
                          GetCurrentProcess (), &hThreadCopy, THREAD_ALL_ACCESS, FALSE, 0 ))
    return;

  auto dll_ini =
    SK_GetDLLConfig ();

  hThread = hThreadCopy;

  prio_cfg =
    dynamic_cast <sk::ParameterInt *> (
      g_ParameterFactory->create_parameter <int> (L"Thread Priority")
    );

  if (! prio_cfg)
    return;

  if (this == &*sk_ffxv_swapchain)
  {
    SK_CreateFuncHook (      L"SleepEx_Detour",
                               SleepEx_Detour,
                               SK_FFXV_SleepEx,
      static_cast_p2p <void> (&SleepEx_Override) );
    SK_EnableHook        (     SleepEx_Detour );
#if 0
    SK_CreateDLLHook2 (      L"kernel32",
                              "SleepConditionVariableCS",
                               SleepConditionVariableCS_Detour,
      static_cast_p2p <void> (&SleepConditionVariableCS_Original) );

    SK_ApplyQueuedHooks ();
#endif

    prio_cfg->register_to_ini ( dll_ini, L"FFXV.CPUFix",
                                         L"SwapChainPriority" );
  }

  else if (this == &*sk_ffxv_vsync)
  {
    prio_cfg->register_to_ini ( dll_ini, L"FFXV.CPUFix", L"VSyncPriority" );
  }

  else if (this == &*sk_ffxv_async_run)
  {
    prio_cfg->register_to_ini ( dll_ini, L"FFXV.DiskFix", L"AsyncFileRun" );
  }

  else
  {
    return;
  }

  dwPrio = GetThreadPriority ( hThread );

  int                  prio                       = 0;
  if ( prio_cfg->load (prio) && prio < 4 && prio >= 0 )
  {
    InterlockedExchange ( &dwPrio,
                            priority_levels [prio] );

    SetThreadPriority ( hThread, ReadAcquire (&dwPrio) );
  }
}

void
SK_FFXV_SetupThreadPriorities (void)
{
  static int iters = 0;

  if (sk_ffxv_swapchain->hThread == 0)
  {
    SK_AutoHandle hThread (
      OpenThread ( THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId () )
    );

    sk_ffxv_swapchain->setup (hThread.m_h);
  }

  else  if ((iters++ % 120) == 0)
  {
    SetThreadPriority (sk_ffxv_swapchain->hThread, sk_ffxv_swapchain->dwPrio);
    SetThreadPriority (sk_ffxv_vsync->hThread,     sk_ffxv_vsync->dwPrio);
    SetThreadPriority (sk_ffxv_async_run->hThread, sk_ffxv_async_run->dwPrio);
  }
}

bool
SK_FFXV_PlugInCfg (void)
{
  auto dll_ini =
    SK_GetDLLConfig ();

  if (ImGui::CollapsingHeader ("Final Fantasy XV Windows Edition", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    static bool ignis_vision = false;
    static bool hair_club    = false;

    if (ignis_vision || hair_club)
      SK_D3D11_EnableTracking = true;

    static auto& shaders =
      SK_D3D11_Shaders.get ();

    if (ImGui::Checkbox ((const char *)u8R"(Ignis Vision ™)", &ignis_vision))
    {
      if (ignis_vision)
      {
        shaders.vertex.addTrackingRef (shaders.vertex.wireframe, 0x89d01dda);
        shaders.vertex.addTrackingRef (shaders.vertex.on_top,    0x89d01dda);
      } else {
        shaders.vertex.releaseTrackingRef (shaders.vertex.wireframe, 0x89d01dda);
        shaders.vertex.releaseTrackingRef (shaders.vertex.on_top,    0x89d01dda);
      }
    }

    ImGui::SameLine ();

    if (ImGui::Checkbox ((const char *)u8R"((No)Hair Club for Men™)", &hair_club))
    {
      if (hair_club)
      {
        // Normal Hair
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, 0x1a77046d);
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, 0x132b907a);
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, 0x8a0dbca1);
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, 0xc9bb3e7f);

        // Wet Hair
        //shaders.pixel.blacklist.emplace (0x41c6add3);
        //shaders.pixel.blacklist.emplace (0x4524bf4f);
        //shaders.pixel.blacklist.emplace (0x62f9cfe8);
        //shaders.pixel.blacklist.emplace (0x95f7de71);

        // HairWorks
        shaders.pixel.addTrackingRef (shaders.pixel.blacklist, 0x2d6f6ee8);
      } else {
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, 0x1a77046d);
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, 0x132b907a);
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, 0x8a0dbca1);
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, 0xc9bb3e7f);
        shaders.pixel.releaseTrackingRef (shaders.pixel.blacklist, 0x2d6f6ee8);
        //shaders.pixel.blacklist.erase (0x41c6add3);
        //shaders.pixel.blacklist.erase (0x4524bf4f);
        //shaders.pixel.blacklist.erase (0x62f9cfe8);
        //shaders.pixel.blacklist.erase (0x95f7de71);
      }
    }


    auto ConfigThreadPriority = [&](const char* name, SK_FFXV_Thread& thread) ->
    int
    {
      ImGui::PushID (name);

      int idx = ( sk::narrow_cast <int> (thread.dwPrio) == priority_levels [0] ? 0 :
                ( sk::narrow_cast <int> (thread.dwPrio) == priority_levels [1] ? 1 :
                ( sk::narrow_cast <int> (thread.dwPrio) == priority_levels [2] ? 2 : 3 ) ) );

      if ( thread.hThread )
      {
        if (ImGui::Combo (name, &idx, "Normal Priority\0Above Normal\0Highest\0Time Critical\0\0"))
        {
          InterlockedExchange ( &thread.dwPrio, priority_levels [idx]);
          SetThreadPriority   ( thread.hThread, ReadAcquire (&thread.dwPrio) );

          thread.prio_cfg->store ( idx );
                  dll_ini->write ( dll_ini->get_filename () );
        }


      const int dwPrio = idx;
        idx = ( dwPrio == priority_levels [0] ? 0 :
              ( dwPrio == priority_levels [1] ? 1 :
              ( dwPrio == priority_levels [2] ? 2 : 3 ) ) );

        if (ImGui::IsItemHovered ())
        {
          ImGui::BeginTooltip ( );
          ImGui::PushStyleColor (ImGuiCol_Text, ImColor::HSV (0.075f, 0.8f, 0.9f).Value);
          ImGui::Text         ( "The graphics engine has bass-acwkwards scheduling priorities." );
          ImGui::PopStyleColor ();
          ImGui::Separator    ( );

          ImGui::BulletText  ("Time Critical Scheduling is for simple threads that write data constantly and would break if ever interrupted.");
          ImGui::TreePush    ("");
          ImGui::BulletText  ("Audio, for example.");
          ImGui::TreePop     (  );

          ImGui::Text        ("");

          ImGui::BulletText  ("--- Rendering is completely different ---");
          ImGui::TreePush    ("");
          ImGui::BulletText  ("The engine starves threads with more important work to do because it assigned them the wrong priority too.");
          ImGui::PushStyleColor (ImGuiCol_Text, ImColor::HSV (0.25f, 0.8f, 0.9f).Value);
          ImGui::BulletText  ("LOWER the priority of all render-related threads for best results.");
          ImGui::PopStyleColor ();
          ImGui::TreePop     (  );
          ImGui::EndTooltip  (  );
        }
      }

      ImGui::PopID ();

      return idx;
    };

    ImGui::BeginGroup ();
    int x =
    ConfigThreadPriority ("VSYNC Emulation Thread###VSE_Thr", sk_ffxv_vsync);
    int y =
    ConfigThreadPriority ("SwapChain Flip Thread###SWF_Thr",  sk_ffxv_swapchain);
    int z =
    ConfigThreadPriority ("Aync. File Run Thread###AFR_Thr",  sk_ffxv_async_run);

    ImGui::EndGroup   ();
    ImGui::SameLine   ();
    ImGui::BeginGroup ();

    for ( auto* label_me : { &x, &y, &z } )
    {
      if ( *label_me == 3 &&
            label_me != &z   )
      {
        ImGui::PushStyleColor (ImGuiCol_Text, (ImVec4&&)ImColor::HSV (0.12f, 0.9f, 0.95f));
        ImGui::BulletText     ("Change this for better performance!");
        ImGui::PopStyleColor  ();
      }

      else
        ImGui::Text ("");
    }
    ImGui::EndGroup (  );
    //ImGui::SameLine (  );
    //
    //extern bool fix_sleep_0;
    //ImGui::Checkbox ("Sleep (0) --> SwitchToThread ()", &fix_sleep_0);

    ImGui::TreePop  (  );

    return true;
  }

  return false;
}



bool
SK_SO4_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("STAR OCEAN - THE LAST HOPE - 4K & Full HD Remaster", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    extern float SK_SO4_MouseScale;
    ImGui::SliderFloat ("Mouselook Deadzone Compensation", &SK_SO4_MouseScale, 2.0f, 33.333f);

    if (ImGui::IsItemHovered ())
    {
      ImGui::SetTooltip ("Reduces mouse deadzone, but may cause Windows to draw the system cursor if set too high.");
    }

    ImGui::TreePop  ();

    return false;
  }

  return true;
}


volatile LONG SK_POE2_Horses_Held        = 0;
volatile LONG SK_POE2_SMT_Assists        = 0;
volatile LONG SK_POE2_ThreadBoostsKilled = 0;
         bool SK_POE2_FixUnityEmployment = false;
         bool SK_POE2_Stage2UnityFix     = false;
         bool SK_POE2_Stage3UnityFix     = false;


bool
SK_POE2_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Pillars of Eternity II: Deadfire", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    ImGui::Checkbox        ("Supervise Unity Worker Thread Scheduling", &SK_POE2_FixUnityEmployment); ImGui::SameLine ();
    ImGui::TextUnformatted (" (Advanced, not saved)");

    if (SK_POE2_FixUnityEmployment)
    {
      ImGui::BeginGroup ();
      int lvl = SK_POE2_Stage3UnityFix ? 2 :
                SK_POE2_Stage2UnityFix ? 1 : 0;

      if (
        ImGui::Combo ( "Scheduling Supervisory Level",
                       &lvl, "Avoid GPU Thread Starvation\0"
                             "SMT Thread Custody Mediation\0"
                             "Apply Unity Rules on All Threads\0" )
      ) {
        SK_POE2_Stage3UnityFix = (lvl > 1);
        SK_POE2_Stage2UnityFix = (lvl > 0);
      }
      ImGui::EndGroup   ();

      ImGui::SameLine   ();

      ImGui::BeginGroup ();
      ImGui::Text       ("Events Throttled:");
      ImGui::Text       ("SMT Microsleep Yields:");
      ImGui::Text       ("Pre-Emption Adjustments:");
      ImGui::EndGroup   ();

      ImGui::SameLine   ();

      ImGui::BeginGroup ();
      ImGui::Text       ("%lu", ReadAcquire (&SK_POE2_Horses_Held));
      ImGui::Text       ("%lu", ReadAcquire (&SK_POE2_SMT_Assists));
      ImGui::Text       ("%lu", ReadAcquire (&SK_POE2_ThreadBoostsKilled));
      ImGui::EndGroup   ();
    }

    ImGui::Separator ();

    static int orig =
      config.render.framerate.override_num_cpus;

    bool spoof = (config.render.framerate.override_num_cpus != SK_NoPreference);

    static SYSTEM_INFO             si = { };
    SK_RunOnce (SK_GetSystemInfo (&si));

    if ((! spoof) || sk::narrow_cast <DWORD> (config.render.framerate.override_num_cpus) > (si.dwNumberOfProcessors / 2))
    {
      ImGui::PushStyleColor (ImGuiCol_Text, (ImVec4&&)ImColor::HSV (.14f, .8f, .9f));
      ImGui::BulletText     ("It is strongly suggested that you reduce worker threads to 1/2 max. or lower");
      ImGui::PopStyleColor  ();
    }

    if ( ImGui::Checkbox   ("Reduce Worker Threads", &spoof) )
    {
      config.render.framerate.override_num_cpus =
        ( spoof ? si.dwNumberOfProcessors : -1 );
    }

    if (spoof)
    {
      ImGui::SameLine  (                                             );
      ImGui::SliderInt ( "Number of Worker Threads",
                        &config.render.framerate.override_num_cpus,
                        1, si.dwNumberOfProcessors              );
    }

    if (config.render.framerate.override_num_cpus != orig)
    {
      ImGui::PushStyleColor (ImGuiCol_Text, (ImVec4&&)ImColor::HSV (.3f, .8f, .9f));
      ImGui::BulletText     ("Game Restart Required");
      ImGui::PopStyleColor  ();
    }

    ImGui::TreePop  ();

    return false;
  }

  return true;
}

volatile LONG __SK_SHENMUE_FinishedButNotPresented = 0;
volatile LONG __SK_SHENMUE_FullAspectCutscenes     = 1;
         bool  bSK_SHENMUE_FullAspectCutscenes     = true;

sk::ParameterBool*    _SK_SM_FullAspectCutscenes = nullptr;
sk::ParameterStringW* _SK_SM_FullAspectToggle    = nullptr;

sk::ParameterFloat*   _SK_SM_ClockFuzz           = nullptr;
sk::ParameterBool*    _SK_SM_BypassLimiter       = nullptr;

bool SK_Shenmue_UseNtDllQPC = true;

struct shenmue_limit_ctrl_s {
public:
  bool
    initialize (LPVOID lpQueryAddr)
  {
    if (lpQueryAddr != nullptr)
    {
      if ( SK_IsAddressExecutable (lpQueryAddr) &&
           SK_GetModuleFromAddr   (lpQueryAddr) ==
           SK_Modules->HostApp    (           )    )
      {
        qpc_loop_addr =
           lpQueryAddr;

        if ( branch_addr == nullptr )
        {
          branch_addr =
            SK_ScanAlignedEx ( "\x7C\xEE", 2, nullptr, (void *)qpc_loop_addr );

          // Damn, we couldn't find the loop control code
          if (branch_addr == nullptr)
              branch_addr = (LPVOID)-1;
        }

        if (want_bypass)
          toggle ();

        return true;
      }
    }

    return false;
  }

  bool
    toggle (void)
  {
    if (qpc_loop_addr != nullptr)
    {
      if (reinterpret_cast <intptr_t> (branch_addr) > 0)
      {
        DWORD dwOldProt = (DWORD)-1;

        VirtualProtect ( branch_addr,                     2,
                         PAGE_EXECUTE_READWRITE, &dwOldProt );

        if (enabled)
        {
          memcpy ( branch_addr,
                     "\x90\x90", 2 );
        }

        else
        {
          memcpy ( branch_addr,
                     orig_instns, 2 );
        }

        enabled = (! enabled);

        VirtualProtect ( branch_addr,          2,
                         dwOldProt,   &dwOldProt );
      }
    }

    return enabled;
  }

//protected:
  // ---------------------------------------
  // The first return addr. we encounter in our
  //   QPC hook for a call originating on the render
  //     thread is within jumping distance of the
  //       evil busy-loop causing framerate problems.
  LPCVOID qpc_loop_addr    =        nullptr;
  LPVOID  branch_addr      =        nullptr;
  uint8_t orig_instns  [2] = { 0x7C, 0xEE };

  // It takes a few frames to locate the limiter's
  //   inner-loop -- but the config file's preference
  //     needs to be respected.
  //
  //  * So ... once we find the addr. -> Install Bypass?
  //
  bool    want_bypass      = true;
  bool    enabled          = true;
} SK_Shenmue_Limiter;

bool
SK_Shenmue_IsLimiterBypassed (void)
{
  return
    ( SK_Shenmue_Limiter.enabled == false );
}

bool
SK_Shenmue_InitLimiterOverride (LPVOID pQPCRetAddr)
{
  return
    SK_Shenmue_Limiter.initialize (pQPCRetAddr);
}

SK_IVariable *pVarWideCutscenes;
SK_IVariable *pVarBypassLimiter;

void
SK_SM_PlugInInit (void)
{
  plugin_mgr->config_fns.emplace (SK_SM_PlugInCfg);

  auto cp =
    SK_GetCommandProcessor ();

       if (0 == _wcsicmp (SK_GetHostApp (), L"Shenmue.exe"))
    __SK_SHENMUE_ClockFuzz = 20.0f;
  else if (0 == _wcsicmp (SK_GetHostApp (), L"Shenmue2.exe"))
    __SK_SHENMUE_ClockFuzz = 166.0f;

  _SK_SM_FullAspectCutscenes =
    _CreateConfigParameterBool  ( L"Shenmue.Misc",
                                  L"FullAspectCutscenes", bSK_SHENMUE_FullAspectCutscenes,
                                  L"Enable Full Aspect Ratio Cutscenes" );

  _SK_SM_ClockFuzz =
    _CreateConfigParameterFloat ( L"Shenmue.Misc",
                                  L"ClockFuzz", __SK_SHENMUE_ClockFuzz,
                                  L"Framerate Limiter Variance" );

  _SK_SM_BypassLimiter =
    _CreateConfigParameterBool  ( L"Shenmue.Misc",
                                  L"BypassFrameLimiter", SK_Shenmue_Limiter.want_bypass,
                                  L"Die you evil hellspawn!" );

  if (bSK_SHENMUE_FullAspectCutscenes)
  {
    InterlockedExchange  (&__SK_SHENMUE_FullAspectCutscenes, 1);
    InterlockedIncrement (&SK_D3D11_DrawTrackingReqs);
  }

  else
  {
    InterlockedExchange (&__SK_SHENMUE_FullAspectCutscenes, 0);
  }


  cp->AddVariable ( "Shenmue.ClockFuzz",
                      new SK_IVarStub <float> ((float *)&__SK_SHENMUE_ClockFuzz));

  class listen : public SK_IVariableListener
  {
  public:
    virtual ~listen (void) = default;

    bool OnVarChange (SK_IVariable* var, void* val = nullptr) override
    {
      if (var == pVarWideCutscenes)
      {
        if (val != nullptr)
        {
          InterlockedExchange (&__SK_SHENMUE_FullAspectCutscenes, *(bool *) val != 0 ? 1 : 0);

          if (ReadAcquire (&__SK_SHENMUE_FullAspectCutscenes))
          {
            InterlockedIncrement (&SK_D3D11_DrawTrackingReqs);
          }
          else
            InterlockedDecrement (&SK_D3D11_DrawTrackingReqs);
        }
      }

      if (var == pVarBypassLimiter)
      {
        if (val != nullptr)
        {
          SK_Shenmue_Limiter.want_bypass = *(bool *)val;

          if (SK_Shenmue_Limiter.enabled ==
              SK_Shenmue_Limiter.want_bypass)
          {
            SK_Shenmue_Limiter.toggle ();
          }
        }
      }

      return true;
    }
  } static stay_a_while_and_listen;


  pVarWideCutscenes =
    new SK_IVarStub <bool> (
      (bool *)&__SK_SHENMUE_FullAspectCutscenes,
              &stay_a_while_and_listen
    );

  pVarBypassLimiter =
    new SK_IVarStub <bool> (
      (bool *)&SK_Shenmue_Limiter.want_bypass,
              &stay_a_while_and_listen
    );

  cp->AddVariable ("Shenmue.NoCrops",       pVarWideCutscenes);
  cp->AddVariable ("Shenmue.BypassLimiter", pVarBypassLimiter);


  if (SK_Shenmue_Limiter.want_bypass)
  {
    if (config.render.framerate.target_fps == 0.0f)
      cp->ProcessCommandLine ("TargetFPS 30.0");

    else
    {
      cp->ProcessCommandFormatted (
        "TargetFPS %f", config.render.framerate.target_fps
      );
    }
  }
}

bool
SKX_Keybinding (SK_Keybind* binding, sk::ParameterStringW* param)
{
  if (param == nullptr)
    return false;

  std::string label =
    SK_WideCharToUTF8 (binding->human_readable) + "###";
              label += binding->bind_name;

  if (SK_ImGui_KeybindSelect (binding, label.c_str ()))
    ImGui::OpenPopup (        binding->bind_name);

  std::wstring original_binding = binding->human_readable;

  SK_ImGui_KeybindDialog (binding);

  if (original_binding != binding->human_readable)
  {
    param->store (binding->human_readable);

    SK_SaveConfig ();

    return true;
  }

  return false;
};

bool
SK_SM_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Shenmue I & II", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    if (SK_GetCurrentGameID () == SK_GAME_ID::Shenmue)
    {
      bSK_SHENMUE_FullAspectCutscenes =
        ( ReadAcquire (&__SK_SHENMUE_FullAspectCutscenes) != 0 );

      bool changed =
        ImGui::Checkbox ( "Enable 16:9 Aspect Ratio Cutscenes",
                          &bSK_SHENMUE_FullAspectCutscenes );

      //ImGui::SameLine ();

      //static std::set <SK_ConfigSerializedKeybind *>
      //  keybinds = {
      //    &config.steam.screenshots.game_hud_free_keybind
      //  };

      ////ImGui::SameLine   ();
      ////ImGui::BeginGroup ();
      ////for ( auto& keybind : keybinds )
      ////{
      ////  ImGui::Text          ( "%s:  ",
      ////                        keybind->bind_name );
      ////}
      ////ImGui::EndGroup   ();
      ////ImGui::SameLine   ();
      ////ImGui::BeginGroup ();
      ////for ( auto& keybind : keybinds )
      ////{
      ////  Keybinding ( keybind, keybind->param );
      ////}
      ////ImGui::EndGroup   ();

      if (SK_Shenmue_Limiter.branch_addr != nullptr)
      {
        bool bypass =
          (! SK_Shenmue_Limiter.enabled);

        bool want_change =
          ImGui::Checkbox ( "Complete Framerate Limit Bypass###SHENMUE_FPS_BYPASS",
                           &bypass );

        if (ImGui::IsItemHovered ())
        {
          ImGui::SetTooltip ( "Limiter Branch Addr. location %ph",
                                SK_Shenmue_Limiter.branch_addr );
        }

        if (bypass)
        {
          ImGui::SameLine ();
          ImGui::Checkbox ("Use  -mode timer", &SK_Shenmue_UseNtDllQPC);
        }

        if (want_change)
        {
          SK_Shenmue_Limiter.toggle ();

          SK_Shenmue_Limiter.want_bypass =
            (! SK_Shenmue_Limiter.enabled);

          changed = true;
        }
      }

      if (SK_Shenmue_Limiter.enabled)
      {
        changed |=
          ImGui::SliderFloat ( "30 FPS Clock Fuzzing",
                                 &__SK_SHENMUE_ClockFuzz, -20.f, 200.f );

        if (ImGui::IsItemHovered ())
        {
          ImGui::SetTooltip ( "Ancient Chinese Secret Technique for Pacifying "
                              "Uncooperative Framerate Limiters" );
        }
      }

      if (changed)
      {
        _SK_SM_BypassLimiter->store      ((! SK_Shenmue_Limiter.enabled));
        _SK_SM_FullAspectCutscenes->store ( bSK_SHENMUE_FullAspectCutscenes);
        _SK_SM_ClockFuzz->store          ( __SK_SHENMUE_ClockFuzz);

        SK_GetDLLConfig   ()->write (
          SK_GetDLLConfig ()->get_filename ()
        );

        if (bSK_SHENMUE_FullAspectCutscenes)
          InterlockedIncrement (&SK_D3D11_DrawTrackingReqs);
        else
          InterlockedDecrement (&SK_D3D11_DrawTrackingReqs);

        InterlockedExchange ( &__SK_SHENMUE_FullAspectCutscenes,
                                bSK_SHENMUE_FullAspectCutscenes ? 1L : 0L );
      }
    }

    ImGui::TreePop  (  );
  }

  return true;


}

sk::ParameterBool*  _SK_ACO_AlternateTaskScheduling;
sk::ParameterFloat* _SK_ACO_AutoRebalanceInterval;

void
SK_ACO_PlugInInit (void)
{
  plugin_mgr->config_fns.emplace (SK_ACO_PlugInCfg);

  __SK_MHW_KillAntiDebug = false;

  _SK_ACO_AlternateTaskScheduling =
    _CreateConfigParameterBool  ( L"AssassinsCreed.Threads",
                                  L"AltTaskSchedule", __SK_MHW_KillAntiDebug,
                                  L"Make Task Threads More Cooperative" );

  _SK_ACO_AutoRebalanceInterval =
    _CreateConfigParameterFloat ( L"AssassinsCreed.Threads",
                                  L"AutoRebalanceInterval", __SK_Thread_RebalanceEveryNSeconds,
                                  L"Periodically Rebalance Task Threads" );

  SK_SaveConfig ();
}

bool
SK_ACO_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Assassin's Creed Odyssey / Origins", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    bool changed = false;

    if (ImGui::Checkbox ("Alternate Task Scheduling", &__SK_MHW_KillAntiDebug))
    {
      changed = true;
      _SK_ACO_AlternateTaskScheduling->store (__SK_MHW_KillAntiDebug);
    }

    if (ImGui::IsItemHovered ())
    {
      ImGui::BeginTooltip    ();
      ImGui::TextUnformatted ("Causes task threads to yield CPU time to other threads more often");
      ImGui::Separator       ();
      ImGui::BulletText      ("The task threads are not switching to other threads cooperatively and that produces high CPU load on lower-end systems.");
      ImGui::BulletText      ("Try both on/off, I cannot give one-answer-fits-all advice about this setting.");
      ImGui::EndTooltip      ();
    }

    ImGui::PushStyleColor  (ImGuiCol_Text, (ImVec4&&)ImColor::HSV (0.18f, 0.85f, 0.95f));
    ImGui::TextUnformatted ("Thread Rebalancing is CRITICAL for AMD Ryzen CPUs");
    ImGui::PopStyleColor   ();

    if (ImGui::IsItemHovered ())
    {
      ImGui::BeginTooltip    ();
      ImGui::TextUnformatted ("Ryzen CPUs need this because of a scheduler bug in Windows that tries to run all threads on a single CPU core.");
      ImGui::Separator       ();
      ImGui::BulletText      ("Other CPU brands and models may see marginal improvements by enabling auto-rebalance.");
      ImGui::EndTooltip      ();
    }

    SK_ImGui_RebalanceThreadButton ();

    if (ImGui::IsItemHovered ())
      ImGui::SetTooltip ("Redistribute CPU core assignment based on CPU usage history.");

    ImGui::SameLine ();


    if (ImGui::SliderFloat ("Re-Balance Interval", &__SK_Thread_RebalanceEveryNSeconds, 0.0f, 60.0f, "%.3f Seconds"))
    {
      changed = true;
      _SK_ACO_AutoRebalanceInterval->store (__SK_Thread_RebalanceEveryNSeconds);
    }

    if (ImGui::IsItemHovered ())
      ImGui::SetTooltip ("0=Disabled, anything else periodically redistributes threads for optimal CPU utilization.");

    if (changed)
      SK_SaveConfig ();

    ImGui::TreePop  (  );
  }

  return true;
}
#endif


#ifdef _M_AMD64
void __stdcall
SK_HatsuneMiku_BeginFrame (void)
{
  // Skip a few frames before doing the pointer scan
  //
  if (SK_GetFramesDrawn () < 333)
    return;

  // 1.00: 0x14B2A78
  // 1.01: 0x14ACA68
  // 1.02, 1.03: 0x14ABBB8
  static uint32_t* puiGameLimit =
        (uint32_t *)((uintptr_t)SK_Debug_GetImageBaseAddr () + 0x14ABBB8);

  // Pointer at DivaMegaMix.exe, Offset=780h
  // 1.00: 0x114EFF8
  // 1.01: 0x1148FC8
  // 1.02, 1.03: 0x11481E8
  static auto menu_flag_addr =
      *(uintptr_t *)((uintptr_t)SK_Debug_GetImageBaseAddr () + 0x11481E8) + 0x780;

  // Menu is Active, 60 FPS (or lower) framerate cap is required
  if ((*(uint8_t *)menu_flag_addr) & 0x1)
  {
    __target_fps =
      std::min ( 60.0f,  config.render.framerate.target_fps <= 0.0f ?
                 60.0f : config.render.framerate.target_fps );

    *puiGameLimit = 60;
  }

  // Gameplay: Game needs no cap
  else
  {
    __target_fps =
      config.render.framerate.target_fps;

    *puiGameLimit = 0;
  }
}
#endif


#include <SpecialK/render/dxgi/dxgi_util.h>

bool
SK_D3D11_SanitizeFP16RenderTargets ( ID3D11DeviceContext *pDevCtx,
                                     UINT                 dev_idx )
{
  if (dev_idx == UINT_MAX)
  {
    dev_idx =
      SK_D3D11_GetDeviceContextHandle (pDevCtx);
  }

  // This Pixel Shader is the source of all artifacts
  if ( 0xeef0923 ==
         SK_D3D11_Shaders->pixel.current.shader [dev_idx] )
  {
    SK_ComPtr <ID3D11RenderTargetView> pRTV;
    pDevCtx->OMGetRenderTargets  ( 1, &pRTV.p, nullptr ); 
    
    if (pRTV.p != nullptr)
    {
      SK_ComPtr <ID3D11Texture2D>     pTexCopy;
      SK_ComPtr <ID3D11Device>            pDev;
      pDevCtx->GetDevice                (&pDev.p);
      SK_ComPtr <ID3D11Resource>          pRes;
      pRTV->GetResource                 (&pRes.p);
      SK_ComQIPtr <ID3D11Texture2D> pTex (pRes.p);

      if ( pRes.p != nullptr &&
           pTex.p != nullptr &&
           pDev.p != nullptr )
      {
        D3D11_TEXTURE2D_DESC    texDesc = { };
        pTex->GetDesc         (&texDesc);
        pDev->CreateTexture2D (&texDesc, nullptr,
           &pTexCopy.p);
        if (pTexCopy.p != nullptr)
        {
          pDevCtx->CopyResource (
            pTexCopy.p, pTex.p
          );
  
          return
            SK_D3D11_BltCopySurface (
              pTexCopy, pTex
            );
        }
      }
    }
  }

  return false;
}


#ifdef _M_AMD64
bool SK_SO2R_UltraWidescreen = false;

void SK_SO2R_InitPlugin (void)
{
  auto dll_ini =
    SK_GetDLLConfig ();

  SK_SO2R_UltraWidescreen =
    SK_IsTrue (
      dll_ini->get_section (L"SO2R.PlugIn").
               get_cvalue  (L"UnlockAspectRatio").c_str ()
    );

  if (SK_SO2R_UltraWidescreen)
    InterlockedIncrement (&SK_D3D11_DrawTrackingReqs);

  plugin_mgr->config_fns.emplace (SK_SO2R_PlugInCfg);
}

bool SK_SO2R_PlugInCfg (void)
{
  auto dll_ini =
    SK_GetDLLConfig ();

  if (ImGui::CollapsingHeader ("STAR OCEAN THE SECOND STORY R", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    if (ImGui::Checkbox ("Enable Wide Aspect Ratios", &SK_SO2R_UltraWidescreen))
    {
      dll_ini->get_section (L"SO2R.PlugIn").
             add_key_value (L"UnlockAspectRatio",
      SK_SO2R_UltraWidescreen ? L"true"
                              : L"false");

      if (SK_SO2R_UltraWidescreen) InterlockedIncrement (&SK_D3D11_DrawTrackingReqs);
      else                         InterlockedDecrement (&SK_D3D11_DrawTrackingReqs);

      dll_ini->write ();
    }

    if (ImGui::IsItemHovered ())
    {
      ImGui::SetTooltip ("Some visual glitches may occur.");
    }

    ImGui::TreePop  ();

    return false;
  }

  return true;
}

bool
SK_SO2R_DrawHandler (ID3D11DeviceContext *pDevCtx, uint32_t current_ps, int num_verts)
{
  if (pDevCtx != nullptr && SK_SO2R_UltraWidescreen && current_ps == 0x7ee4636e)
  {
    SK_ComPtr <ID3D11ShaderResourceView>  pSRV;
    pDevCtx->PSGetShaderResources (0, 1, &pSRV.p);
  
    if (pSRV.p != nullptr)
    {
      SK_ComPtr <ID3D11Resource>
                          pRes;
      pSRV->GetResource (&pRes.p);
  
      SK_ComQIPtr <ID3D11Texture2D>
          pTex (pRes);
      if (pTex != nullptr)
      {
        D3D11_TEXTURE2D_DESC
                        texDesc = { };
        pTex->GetDesc (&texDesc);
  
        if (texDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM && (num_verts == 24 || num_verts == 6))
        {
          UINT       num_vp =   1;
          D3D11_VIEWPORT vp = { };
          pDevCtx->RSGetViewports (&num_vp, &vp);

          if ( vp.TopLeftX != 0.0f ||
               vp.TopLeftY != 0.0  )
          {
          }

          else
          {
            return true;
          }
          //SK_LOGi0 (L"DrawType=%d, Verts=%d", draw_type, num_verts);
        }
      }
    }
  }

  return false;
}
#endif

WNDPROC SK_Metaphor_OrigWndProc = nullptr;
LRESULT
CALLBACK
SK_Metaphor_WindowProc ( _In_  HWND   hWnd,
                         _In_  UINT   uMsg,
                         _In_  WPARAM wParam,
                         _In_  LPARAM lParam )
{
  switch (uMsg)
  {
    case WM_CLOSE:
    {
      if (config.input.keyboard.override_alt_f4 && hWnd == game_window.hWnd)
      {
        SK_ImGui_WantExit = true;
      
        if (! config.input.keyboard.catch_alt_f4)
        {
          return IsWindowUnicode (hWnd)                ?
           DefWindowProcW (hWnd, uMsg, wParam, lParam) :
           DefWindowProcA (hWnd, uMsg, wParam, lParam);
        }
      
        return 0;
      }
    } break;
    default:
      break;
  }

  return SK_Metaphor_OrigWndProc (hWnd, uMsg, wParam, lParam);
}


__forceinline
static void
SK_AMD_MWAITX (INT64 qpcTarget = 0)
{
  if (! config.render.framerate.use_amd_mwaitx)
    return YieldProcessor ();

  static alignas(64) uint64_t monitor = 0ULL;

  _mm_monitorx (&monitor, 0, 0);
  _mm_mwaitx   (0x2, 0, (qpcTarget > 0) ? ((DWORD)qpcTarget * SK_PerfFreqInTsc + 1) : SK_PerfFreqInTsc);
}

bool
SK_CPU_IsZen (void);

__forceinline
static void
SK_YieldProcessor (INT64 qpcTarget = 0)
{
  static const bool bIsCompatibleZen =
    SK_CPU_IsZen () && SK_CPU_HasMWAITX;

  if (bIsCompatibleZen)
    SK_AMD_MWAITX (qpcTarget);
  else YieldProcessor ();
}

SleepEx_pfn SK_Metaphor_SleepEx_Original = nullptr;

bool _SK_Metaphor_FixSleepEx = false;

DWORD
WINAPI
SK_Metaphor_SleepEx (DWORD dwMilliseconds, BOOL bAlertable)
{
  if (! _SK_Metaphor_FixSleepEx)
  {
    return
      SK_Metaphor_SleepEx_Original (dwMilliseconds, bAlertable);
  }

  if (dwMilliseconds <= 1)
  {
    static thread_local DWORD  sleep0Count      = 0;
    static thread_local DWORD  sleep1Count      = 0;
    static thread_local UINT64 lastSkippedFrame = 0;

    if (dwMilliseconds == 0)
    {
      if (sleep0Count++ > 65536 && lastSkippedFrame != SK_GetFramesDrawn ())
      {
        sleep0Count = 0;
        SwitchToThread ();
      }

      YieldProcessor ();
      return 0;
    }

    dwMilliseconds = 0;

    // Prefer to keep this thread busy, but prevent a complete runaway...
    if (sleep1Count++ > 0 && lastSkippedFrame == SK_GetFramesDrawn ())
    {
      sleep1Count = 0;

      return
        SleepEx_Original (dwMilliseconds, bAlertable);
    }

    lastSkippedFrame = SK_GetFramesDrawn ();

    // Micro sleep on AMD CPUs for power efficiency on handhelds...
    SK_YieldProcessor (SK_QpcTicksPerMs/1000);

    return 0;
  }

  return
    SK_Metaphor_SleepEx_Original (dwMilliseconds, bAlertable);
}

sk::ParameterBool* _SK_Metaphor_FixSleepExCfg;

bool
SK_Metaphor_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Metaphor: ReFantazio", ImGuiTreeNodeFlags_DefaultOpen))
  {
    ImGui::TreePush ("");

    bool changed = false;

    if (ImGui::Checkbox ("Fix Thread Scheduling", &_SK_Metaphor_FixSleepEx))
    {
      changed = true;
      _SK_Metaphor_FixSleepExCfg->store (_SK_Metaphor_FixSleepEx);
    }

    if (ImGui::IsItemHovered ())
    {
      ImGui::SetTooltip ("Improve performance in CPU-limited scenarios, likely to do more harm than good on Intel CPUs because they lack a special instruction that makes this efficient.");
    }

    if (changed)
      SK_SaveConfig ();

    ImGui::TreePop ();

    return false;
  }

  return true;
}

void
SK_Metaphor_InitPlugin (void)
{
  plugin_mgr->config_fns.emplace (SK_Metaphor_PlugInCfg);

  extern LRESULT
  CALLBACK
  SK_DetourWindowProc ( _In_  HWND   hWnd,
                        _In_  UINT   uMsg,
                        _In_  WPARAM wParam,
                        _In_  LPARAM lParam );

  extern DWORD WINAPI SleepEx_Detour (DWORD, BOOL);

  bool run_once =
    SK_GetDLLConfig ()->contains_section (L"Metaphor.PlugIn");

  _SK_Metaphor_FixSleepExCfg =
    _CreateConfigParameterBool  ( L"Metaphor.PlugIn",
                                  L"FixThreadScheduling", _SK_Metaphor_FixSleepEx,
                                  L"Make Task Threads More Cooperative" );

  if (! run_once)
  {
    _SK_Metaphor_FixSleepEx =
      SK_CPU_IsZen ();
  }

  // This was auto-disabled for many users with texture mods, it should be enabled for performance reasons
  config.textures.cache.allow_staging = false;
  config.render.dxgi.low_spec_mode    = true;

  SK_SaveConfig ();

  SK_CreateFuncHook (        L"SK_DetourWindowProc",
                               SK_DetourWindowProc,
                               SK_Metaphor_WindowProc,
      static_cast_p2p <void> (&SK_Metaphor_OrigWndProc) );
  MH_QueueEnableHook   (       SK_DetourWindowProc      );

  SK_CreateFuncHook (        L"SleepEx_Detour",
                               SleepEx_Detour,
                               SK_Metaphor_SleepEx,
      static_cast_p2p <void> (&SK_Metaphor_SleepEx_Original) );
  MH_QueueEnableHook   (       SleepEx_Detour                );
  SK_ApplyQueuedHooks  ();
}

void
SK_EnderLilies_InitPlugIn (void)
{
  SK_D3D11_DeclHUDShader_Pix (0x0f1f6c61);
  SK_D3D11_DeclHUDShader_Pix (0x28d2dd57);
  SK_D3D11_DeclHUDShader_Pix (0x65ead9ff);

  static sk::ParameterInt64 fov0_addr;
  static uint8_t*           fov0            = nullptr;
  static sk::ParameterInt64 fov1_addr;
  static uint8_t*           fov1            = nullptr;
  static sk::ParameterInt64 pillarbox_addr;
  static uint8_t*           pillarbox_code  = nullptr;
  static sk::ParameterInt   widescreen_opt;
  static int                widescreen_mode = -1;

  auto dll_ini = SK_GetDLLConfig ();

  widescreen_opt.register_to_ini (
    dll_ini, L"EnderLilies.Widescreen", L"EnableWidescreen"
  );

  widescreen_opt.load  (widescreen_mode);
  widescreen_opt.store (widescreen_mode);

  static auto _ApplyPatch = [&](void)
  {
    auto createCachedAddress = [&](sk::ParameterInt64* addr, const wchar_t* name)
    {
      addr->register_to_ini (
                    dll_ini, L"EnderLilies.Widescreen", name);
    };
  
    createCachedAddress (&fov0_addr,      L"Address_Fov0");
    createCachedAddress (&fov1_addr,      L"Address_Fov1");
    createCachedAddress (&pillarbox_addr, L"Address_Pillarbox");
  
    int64_t              addr = 0;
    fov0_addr.load      (addr);
    fov0  =  (uint8_t *) addr;
    fov1_addr.load      (addr);
    fov1  =  (uint8_t *) addr;
    pillarbox_addr.load (addr); pillarbox_code
          =  (uint8_t *) addr;
  
    uint8_t* orig_fov0   = fov0;
    uint8_t* orig_fov1   = fov1;
    uint8_t* orig_pillar = fov1;
  
    pillarbox_code = (uint8_t *)
      SK_ScanAlignedEx ("\xF6\x41\x30\x01\x49", 5,
                        "\xF6\x41\x30\x01\x49", pillarbox_code > (uint8_t*)1 ? pillarbox_code - 1 : 0);
  
    DWORD dwProtect   = 0;
    bool  bFlushAddrs = false;
  
    if (pillarbox_code != nullptr)
    {
      if (VirtualProtect (
           pillarbox_code, 5, PAGE_EXECUTE_READWRITE, &dwProtect))
      {
        if (*(pillarbox_code + 3) == 1) {
            *(pillarbox_code + 3)  = 0;
              pillarbox_addr.store ((uint64_t)(intptr_t)
              pillarbox_code);
  
          bFlushAddrs |=
            (pillarbox_code != orig_pillar);
        }
        VirtualProtect (pillarbox_code, 5, dwProtect, &dwProtect);
      }
    }
  
    fov0 = (uint8_t *)
      SK_ScanAlignedEx ("\x35\xFA\x0E\x3C\x66", 5,
                        "\x35\xFA\x0E\x3C\x66", fov0 > (uint8_t*)1 ? fov0-1 : 0),
    fov1 = (uint8_t *)
      SK_ScanAlignedEx ("\xE0\x2E\xE5\x42",     4,
                        "\xE0\x2E\xE5\x42",     fov1 > (uint8_t*)1 ? fov1-1 : 0);
  
    if ( fov0 != nullptr &&
         fov1 != nullptr )
    {
      if (VirtualProtect (
           fov0, 5, PAGE_EXECUTE_READWRITE, &dwProtect))
      {
        if (*(fov0 + 2) == 0x0E) {
            *(fov0 + 2)  = 0x33;
              fov0_addr.store ((uint64_t)(intptr_t)
              fov0);
  
          bFlushAddrs |=
            (fov0 != orig_fov0);
        }
        VirtualProtect (fov0, 5, dwProtect, &dwProtect);
      }
  
      if (VirtualProtect (
           fov1, 4, PAGE_EXECUTE_READWRITE, &dwProtect))
      {
        if (*(fov1 + 3) == 0x42) {
            *(fov1 + 3)  = 0x41;
              fov1_addr.store ((uint64_t)(intptr_t)
              fov1);
  
          bFlushAddrs |=
            (fov1 != orig_fov1);
        }
        VirtualProtect (fov1, 4, dwProtect, &dwProtect);
      }
    }
  
    if (bFlushAddrs)
      config.utility.save_async ();
  };

  auto _FirstFrame = [](IUnknown* This,UINT,UINT) -> HRESULT
  {
    DXGI_SWAP_CHAIN_DESC
                swapDesc = {};

    SK_ComQIPtr <IDXGISwapChain>
                     pSwapChain (This);
    if (  nullptr != pSwapChain  )
                     pSwapChain->GetDesc (&swapDesc);
  
    if ((float)(swapDesc.BufferDesc.Width)/
        (float)(swapDesc.BufferDesc.Height) > 16.0f/9.0f)
    {
      widescreen_mode = 1;
      widescreen_opt.store (widescreen_mode);
      config.utility.save_async ();
  
      _ApplyPatch ();
    }
  
    return S_OK;
  };

  if (widescreen_mode == -1)
    plugin_mgr->first_frame_fns.insert (_FirstFrame);
  
  if (widescreen_mode == 1)
  {
    _ApplyPatch ();
  }
}



#ifdef _M_AMD64
#include <concurrent_unordered_set.h>
#include <SpecialK/nvapi.h>

bool               __SK_ACS_IsMultiFrameCapable   = false;
bool               __SK_ACS_AlwaysUseFrameGen     =  true;
bool               __SK_ACS_ShowFMVIndicator      = false;
bool               __SK_ACS_UncapFramerate        =  true;
int                __SK_ACS_DLSSG_MultiFrameCount =     1;

sk::ParameterBool*  _SK_ACS_AlwaysUseFrameGen;
sk::ParameterBool*  _SK_ACS_ShowFMVIndicator;
sk::ParameterBool*  _SK_ACS_UncapFramerate;
sk::ParameterInt*   _SK_ACS_DLSSG_MultiFrameCount;

using slGetPluginFunction_pfn = void*      (*)(const char* functionName);
using slDLSSGSetOptions_pfn   = sl::Result (*)(const sl::ViewportHandle& viewport, sl::DLSSGOptions& options);
      slDLSSGSetOptions_pfn
      slDLSSGSetOptions_ACS_Original = nullptr;

static          DWORD  LastTimeFMVChecked     = 0;
static          HANDLE LastFMVHandle          = 0;
static volatile ULONG  FrameGenDisabledForFMV = FALSE;
static bool*          pFrameGenEnabled        = nullptr;

bool
SK_ACS_ApplyFrameGenOverride (bool enable)
{
  static uintptr_t base_addr =
    (uintptr_t)SK_Debug_GetImageBaseAddr ();

  static void* FrameGenTestAddr = nullptr;

  SK_RunOnce (
    /*
    ACShadows.exe+346B5B4 - C1 E0 03              - shl eax,03 { 3 }
    ACShadows.exe+346B5B7 - 48 8B 15 0A2EC407     - mov rdx,[ACShadows.exe+B0AE3C8] { (0F507BC0) }  <---   RIP + 0x07c42e0a
    ACShadows.exe+346B5BE - 80 7A 24 00           - cmp byte ptr [rdx+24],00 { 0 }
    ACShadows.exe+346B5C2 - 48 8B 04 01           - mov rax,[rcx+rax]
    ACShadows.exe+346B5C6 - 48 89 84 24 80000000  - mov [rsp+00000080],rax
    */

    void* code_addr =
      SK_ScanAlignedExec ("\xc1\xe0\x03\x48\x8B\x15\x00\x00\x00\x00\x80\x7A\x24\x00\x48\x8B\x04\x01\x48\x89\x84\x24", 22,
                          "\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\xff\xff\xff\x00\x48\x8B\x04\x01\x48\x89\x84\x24", (void*)base_addr, 4);
  
    if (code_addr != nullptr)
    {
      uint32_t  ptr_offset =                                    0x24;
      uint32_t  rip_offset = *(uint32_t *)((uintptr_t)code_addr + 6);
      uintptr_t rip        =               (uintptr_t)code_addr + 10;
  
      pFrameGenEnabled =
        *(bool **)(rip + rip_offset) + ptr_offset;
    }

    /*
    ACShadows.exe+3397C4C - 83 FD 0A              - cmp ebp,0A { 10 }
    ACShadows.exe+3397C4F - 0F94 C0               - sete al
    ACShadows.exe+3397C52 - 44 89 66 20           - mov [rsi+20],r12d
    ACShadows.exe+3397C56 - 44 88 6E 24           - mov [rsi+24],r13b  <---  (base + 10)
    ACShadows.exe+3397C5A - 89 E9                 - mov ecx,ebp
    ACShadows.exe+3397C5C - 83 E1 FE              - and ecx,-02 { 254 }
    */

    code_addr =
      SK_ScanAlignedExec ("\x83\xFD\x0A\x0F\x94\xC0\x44\x89\x66\x20\x44\x88\x6E\x24\x89\xE9\x83\xE1\xFE", 19,
                          "\x83\xFD\x0A\x0F\x94\xC0\x44\x89\x66\x20\x44\x88\x6E\x24\x89\xE9\x83\xE1\xFE", (void*)base_addr);

    if (code_addr != nullptr)
    {
      FrameGenTestAddr =
        (void *)((uintptr_t)code_addr + 10);
    }
  );

  if (pFrameGenEnabled == nullptr || FrameGenTestAddr == nullptr)
  {
    SK_RunOnce (SK_ImGui_Warning (L"Could not find Frame Generation code?!"));
    return false;
  }

  LastTimeFMVChecked = SK::ControlPanel::current_time;

  DWORD                                                             dwOrigProt = 0x0;
  if (VirtualProtect (FrameGenTestAddr, 4, PAGE_EXECUTE_READWRITE, &dwOrigProt))
  { memcpy           (FrameGenTestAddr, (unsigned char *)(enable ? "\x90\x90\x90\x90"
                                                                 : "\x44\x88\x6E\x24"), 4);
    VirtualProtect   (FrameGenTestAddr,  4, dwOrigProt,             &dwOrigProt);

    *pFrameGenEnabled = enable;
  }

  return  
    *pFrameGenEnabled;
}

sl::Result
SK_ACS_slDLSSGSetOptions_Detour (const sl::ViewportHandle& viewport, sl::DLSSGOptions& options)
{
  SK_LOG_FIRST_CALL

  static bool enabled_once = false;

  if (enabled_once)
  {
    if (__SK_ACS_IsMultiFrameCapable &&
        __SK_ACS_DLSSG_MultiFrameCount >= 1)
    {
      options.numFramesToGenerate =
        __SK_ACS_DLSSG_MultiFrameCount;
    }

    if (__SK_ACS_AlwaysUseFrameGen && !ReadULongAcquire (&FrameGenDisabledForFMV))
    {
      options.mode                = sl::DLSSGMode::eOn;
      options.numFramesToGenerate = std::max (1u, options.numFramesToGenerate);
      SK_ACS_ApplyFrameGenOverride (true);
    }

    return
      slDLSSGSetOptions_ACS_Original (viewport, options);
  }

  auto ret =
    slDLSSGSetOptions_ACS_Original (viewport, options);

  if (ret == sl::Result::eOk)
  {
    enabled_once =
      (options.mode == sl::DLSSGMode::eOn);
  }

  return ret;
}

CloseHandle_pfn __SK_ACS_CloseHandle_Original = nullptr;

BOOL
WINAPI
SK_ACS_CloseHandle_Detour (HANDLE hObject)
{
  SK_LOG_FIRST_CALL

  BOOL bRet =
    __SK_ACS_CloseHandle_Original (hObject);

  if (bRet && hObject == LastFMVHandle)
  {
    LastFMVHandle = nullptr;
    SK_LOGi0 (L"FMV Closed");
  }

  return bRet;
}

#include <imgui/font_awesome.h>

bool
SK_ACS_PlugInCfg (void)
{
  if (ImGui::CollapsingHeader ("Assassin's Creed Shadows", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static bool restart_required = false;

    ImGui::TreePush ("");

    bool changed = false;

    bool always_use_framegen =
      __SK_ACS_AlwaysUseFrameGen;

    ImGui::BeginGroup ();

    if (ImGui::Checkbox ("Allow Cutscene Frame Generation",
                                          &__SK_ACS_AlwaysUseFrameGen))
    { if (SK_ACS_ApplyFrameGenOverride    (__SK_ACS_AlwaysUseFrameGen) != always_use_framegen)
      {  _SK_ACS_AlwaysUseFrameGen->store (__SK_ACS_AlwaysUseFrameGen);

        changed = true;
      }
    }

    if (ImGui::BeginItemTooltip ())
    {
      ImGui::TextUnformatted ("Enable Frame Generation during Cutscenes and in menus, such as the Map Screen.");
      ImGui::Separator       ();
      ImGui::BulletText      ("Cutscene Frame Generation will self-disable when FMVs begin playing (to prevent crashes).");
      ImGui::EndTooltip      ();
    }

    if (__SK_ACS_AlwaysUseFrameGen)
    {
      ImGui::SameLine ();

      if (ImGui::Checkbox ("Identify FMVs", &__SK_ACS_ShowFMVIndicator))
      {     _SK_ACS_ShowFMVIndicator->store (__SK_ACS_ShowFMVIndicator);
        changed = true;
      }

      ImGui::SetItemTooltip ("Identify when cutscenes are running at low framerate because they are pre-rendered.");
    }

    if (__SK_HasDLSSGStatusSupport)
    {
      static bool has_used_dlssg  = __SK_IsDLSSGActive;
                  has_used_dlssg |= __SK_IsDLSSGActive;

      if (has_used_dlssg)
      {
        if (__SK_ACS_IsMultiFrameCapable)
        {
          changed |= ImGui::RadioButton ("2x FrameGen", &__SK_ACS_DLSSG_MultiFrameCount, 1);
          ImGui::SetItemTooltip ("May require opening and closing game menus to take effect.");
          ImGui::SameLine    ( );
          changed |= ImGui::RadioButton ("3x FrameGen", &__SK_ACS_DLSSG_MultiFrameCount, 2);
          ImGui::SetItemTooltip ("May require opening and closing game menus to take effect; note that SK reports the pre-framegen framerate in 3x mode.");
          ImGui::SameLine    ( );
          changed |= ImGui::RadioButton ("4x FrameGen", &__SK_ACS_DLSSG_MultiFrameCount, 3);
          ImGui::SetItemTooltip ("May require opening and closing game menus to take effect; note that SK reports the pre-framegen framerate in 4x mode.");

          if (changed)
          {
            _SK_ACS_DLSSG_MultiFrameCount->store (__SK_ACS_DLSSG_MultiFrameCount);
          }
        }

        if (ImGui::Checkbox ("Allow DLSS Flip Metering", &config.nvidia.dlss.allow_flip_metering))
        {
          config.utility.save_async ();

          restart_required = true;
        }

        if (ImGui::BeginItemTooltip ())
        {
          ImGui::TextUnformatted ("Generate DLSS4 Frames Early and Use Hardware Flip Queue to Pace their Presentation");
          ImGui::Separator       (  );
          ImGui::BulletText      ("SK's overlay will appear blurred for rapidly changing text, but frame generation smoothness is improved.");
          ImGui::BulletText      ("Disabling helps software that cannot tell generated and real frames apart (i.e. RTSS), but is discouraged.");
          ImGui::BulletText      ("Use Special K's \"Native Pacing\" DLSS Frame Generation mode when Flip Metering is enabled.");
          ImGui::Separator       (  );
          ImGui::TextUnformatted ("Ignore extra frames in SK's \"Render Latency\" stat -- HW Flip Queue takes care of those.");
          ImGui::EndTooltip      (  );
        }
      }
    }

    static float last_game_fps_limit = __target_fps;
           float y_pos               = ImGui::GetCursorPosY ();

    if (ImGui::Checkbox ("Uncap Framerate", &__SK_ACS_UncapFramerate))
    {
      changed = true;

      _SK_ACS_UncapFramerate->store (__SK_ACS_UncapFramerate);

      if (! __SK_ACS_UncapFramerate)
      {
        __target_fps =
          last_game_fps_limit;
      }
    }

    if (ImGui::BeginItemTooltip ())
    {
      ImGui::TextUnformatted ("Uncap Framerate in Menus and Cutscenes");
      ImGui::Separator       ();
      ImGui::BulletText      ("ersh has a similar standalone mod that you may use.");
      ImGui::BulletText      ("Turning this back on may require opening and closing the game's menu before limits reapply.");
      ImGui::EndTooltip      ();
    }

    if (restart_required)
    {
      ImGui::PushStyleColor (ImGuiCol_Text, ImColor::HSV (.3f, .8f, .9f).Value);
      ImGui::BulletText     ("Game Restart Required");
      ImGui::PopStyleColor  ();
    }

    ImGui::EndGroup ();
    
    if (__target_fps != config.render.framerate.target_fps)
    {
      last_game_fps_limit = __target_fps;

      ImGui::SameLine        (  );
      ImGui::BeginGroup      (  );
      ImGui::SetCursorPosY   (y_pos);
      ImGui::PushStyleColor  (ImGuiCol_Text, ImColor::HSV (0.075f, 0.8f, 0.9f).Value);
      ImGui::BulletText      ("Using game-defined framerate limit:  ");
      ImGui::SameLine        (  );
      ImGui::SetCursorPosY   (y_pos);
      ImGui::TextColored     (ImColor (1.f, 1.f, 0.f).Value, "%3.0f fps", __target_fps);
      ImGui::PopStyleColor   (  );
      ImGui::EndGroup        (  );
    }

    if (changed)
    {
      SK_SaveConfig ();
    }

    ImGui::TreePop ();
  }

  return true;
}
#endif

void
SK_ACS_InitPlugin (void)
{
  // Address issues caused by Steam Input
  config.nvidia.dlss.disable_ota_updates        = false;
  config.input.gamepad.dinput.blackout_gamepads = true;

#ifdef _M_AMD64
  static HANDLE hInitThread =
  SK_Thread_CreateEx ([](LPVOID)->DWORD
  {
    static void* img_base_addr = 
      SK_Debug_GetImageBaseAddr ();
  
    _SK_ACS_AlwaysUseFrameGen =
      _CreateConfigParameterBool  ( L"AssassinsCreed.FrameRate",
                                    L"AlwaysUseFrameGen", __SK_ACS_AlwaysUseFrameGen,
                                    L"Use FrameGen in Cutscenes" );

    _SK_ACS_ShowFMVIndicator =
      _CreateConfigParameterBool ( L"AssassinsCreed.FrameRate",
                                   L"ShowFMVIndicator", __SK_ACS_ShowFMVIndicator,
                                   L"Show an indicator while FMVs are Playing" );

    _SK_ACS_UncapFramerate =
      _CreateConfigParameterBool ( L"AssassinsCreed.FrameRate",
                                   L"UncapMenusAndCutscenes", __SK_ACS_UncapFramerate,
                                   L"Uncap Framerate in Cutscenes" );

    _SK_ACS_DLSSG_MultiFrameCount =
      _CreateConfigParameterInt  ( L"AssassinsCreed.FrameRate",
                                   L"DLSSGMultiFrameCount", __SK_ACS_DLSSG_MultiFrameCount,
                                   L"Override Multi-Frame Gen" );

    plugin_mgr->config_fns.emplace (SK_ACS_PlugInCfg);

    while (SK_GetFramesDrawn () < 480)
           SK_SleepEx (150UL, FALSE);

    // Fail-safe incase any code that sets this was missed
    static float* framerate_limit = nullptr;

    static DWORD fmv_timeout_ms = 5000UL;

    static concurrency::concurrent_unordered_set <DWORD64> ContinuableCallSites;
    
    for ( auto& callsite :
            // 1.0.1
            { 0x0000000003724651, 0x0000000003725A7F, 0x00000000006A32F1, 0x00000000006A32F7,
              0x00000000006A330E, 0x00000000006A32B0, 0x00000000006A32B4, 0x00000000006A32B8,
              0x000000000372CD05, 0x000000000372CD0C } )
    {
      ContinuableCallSites.insert (callsite);
    }

    // Self-disable cutscene frame generation if it causes a crash, and then
    //   ignore the crash...
    AddVectoredExceptionHandler (1, [](_EXCEPTION_POINTERS *ExceptionInfo)->LONG
    {
      bool continuable = false;

      if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
      {
        auto Context = ExceptionInfo->ContextRecord;

        const DWORD64 addr =
             (DWORD64)Context->Rip - (DWORD64)SK_Debug_GetImageBaseAddr ();

        static concurrency::concurrent_unordered_set <DWORD64> ContinuableCallSitesFound;

        // Turn off frame generation and give a second-chance at life
        if (__SK_ACS_AlwaysUseFrameGen)
        {
          if (LastFMVHandle != 0 && ContinuableCallSites.insert (addr).second)
          {
            SK_LOGi0 (L"FMV Cutscene Exception Found: RIP=%p", addr);
          }

          if (ContinuableCallSites.count       (addr)        &&
              ContinuableCallSitesFound.insert (addr).second &&
              ContinuableCallSitesFound.size   (    )        == 10)
          {
            SK_LOGi0 (L"All 10 FMV Exception CallSites Found!");
            fmv_timeout_ms = 500UL;
          }

                            *pFrameGenEnabled = false;
          WriteULongRelease (&FrameGenDisabledForFMV, TRUE);

          continuable =
            ContinuableCallSites.count (addr) != 0;

          if (! continuable)
          {
            SK_LOGi0 (L"Non-Continuable Exception RIP=%p", addr);
          }

          else
          {
            PVOID SKX_GetNextInstruction (LPVOID addr);

            ExceptionInfo->ContextRecord->Rip =
              (DWORD64)SKX_GetNextInstruction ((void *)ExceptionInfo->ContextRecord->Rip);
          }
        }
      }

      return
        ( continuable ? EXCEPTION_CONTINUE_EXECUTION
                      : EXCEPTION_CONTINUE_SEARCH );
    });

    /*
    ACShadows.exe+F7B0C9 - 48 83 C4 20           - add rsp,20 { 32 }
    ACShadows.exe+F7B0CD - 5E                    - pop rsi
    ACShadows.exe+F7B0CE - C3                    - ret 
    ACShadows.exe+F7B0CF - 80 79 2C 00           - cmp byte ptr [rcx+2C],00 { 0 }
    ACShadows.exe+F7B0D3 - 75 CA                 - jne ACShadows.exe+F7B09F         <---  base + 10
    ACShadows.exe+F7B0D5 - EB 05                 - jmp ACShadows.exe+F7B0DC
    ACShadows.exe+F7B0D7 - 45 84 C0              - test r8b,r8b
    ACShadows.exe+F7B0DA - 75 C3                 - jne ACShadows.exe+F7B09F
    */
    
    static void* limit_check_addr =
      (void *)((uintptr_t)SK_ScanAlignedExec ("\x48\x83\xC4\x20\x5E\xC3\x80\x79\x2C\x00\x75\xCA\xEB\x05\x45\x84\xC0\x75\xC3", 19,
                                              "\x48\x83\xC4\x20\x5E\xC3\x80\x79\x2C\x00\x75\xCA\xEB\x05\x45\x84\xC0\x75\xC3", (void*)img_base_addr) + 10);
    static void* limit_store_addr = nullptr;

    if (limit_check_addr != (void *)10)
    {
      /*
      ACShadows.exe+F7B0BA - 48 8B 05 9F58D108     - mov rax,[ACShadows.exe+9C90960] { (04864E98) }
      ACShadows.exe+F7B0C1 - C5FA1180 98 000000    - vmovss [rax+00000098],xmm0
      */

      limit_store_addr =
        (void *)((uintptr_t)limit_check_addr - 18);

      uint32_t  ptr_offset =                                           0x98;
      uint32_t  rip_offset = *(uint32_t *)((uintptr_t)limit_store_addr - 4);
      uintptr_t rip        =               (uintptr_t)limit_store_addr;

      framerate_limit =
        (float *)(*(uint8_t **)((uintptr_t)rip + rip_offset) + ptr_offset);
    }

    else
    {
      SK_ImGui_Warning (L"Could not find Framerate Limiter Code?!");
    }

    config.system.silent_crash = true;
    config.utility.save_async ();

    static const bool is_fg_capable =
      StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX " ) != nullptr &&
      StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX 2") == nullptr &&
      StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX 3") == nullptr;

    // Pull out the trump card and eliminate flaky NGX feature support queries, by
    // reporting everything as supported as long as an RTX GPU not 2xxx or 3xxx is
    // installed.
    if (is_fg_capable)
    {
      __SK_ACS_IsMultiFrameCapable =
        StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX " ) != nullptr &&
        StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX 2") == nullptr &&
        StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX 3") == nullptr &&
        StrStrW (sk::NVAPI::EnumGPUs_DXGI ()[0].Description, L"RTX 4") == nullptr;

      config.nvidia.dlss.spoof_support = true;
    }

    void*                            pfnCloseHandle = nullptr;
    SK_CreateDLLHook2 (
           L"kernel32",                "CloseHandle",
                                 SK_ACS_CloseHandle_Detour,
      static_cast_p2p <void> (&__SK_ACS_CloseHandle_Original),
                                    &pfnCloseHandle );
    MH_EnableHook                  ( pfnCloseHandle );

    plugin_mgr->open_file_w_fns.insert ([](LPCWSTR lpFileName, HANDLE hFile)
    {
      if (StrStrIW (lpFileName, L"webm"))
      {
        bool file_is_exempt = false;

        for ( auto exempt_substr : { L"Epilepsy",
                                     L"UbisoftLogo",
                                     L"HUB_Bootflow_AbstergoIntro",
                                     L"ACI_Panel_Red_IMG_UI",
                                     L"ACI_Panel_Gen_IMG_UI" } )
        {
          file_is_exempt =
            StrStrIW (lpFileName, exempt_substr);

          if (file_is_exempt)
            break;
        }

        if (! file_is_exempt)
        {
          LastFMVHandle = hFile;

          WriteULongRelease            (&FrameGenDisabledForFMV, TRUE);
          SK_ACS_ApplyFrameGenOverride (false);

          if (__SK_ACS_AlwaysUseFrameGen)
          {
            SK_LOGi0 (
              L"Temporarily disabling Frame Generation because video '%ws' was opened...",
                lpFileName
            );
          }
        }
      }
    });

    // The pointer base addr is stored in the limit_load_addr instruction
    plugin_mgr->begin_frame_fns.insert ([](void)
    {
      float game_limit =
        (framerate_limit == nullptr) ? -1.0f : *framerate_limit;

      if (limit_check_addr != (void *)10)
      {
        static bool UncapFramerate = !__SK_ACS_UncapFramerate;

        static uint8_t orig_check_bytes [2] = {};
        static uint8_t orig_store_bytes [8] = {};

        SK_RunOnce (
          memcpy (orig_store_bytes, limit_store_addr, 8);
          memcpy (orig_check_bytes, limit_check_addr, 2);
        );

        DWORD dwOrigProt = 0x0;

        if (std::exchange (UncapFramerate, __SK_ACS_UncapFramerate) !=
                                           __SK_ACS_UncapFramerate)
        {
          VirtualProtect (limit_store_addr, 8, PAGE_EXECUTE_READWRITE, &dwOrigProt);
          memcpy         (limit_store_addr, UncapFramerate   ?    (unsigned char *)
                          "\x90\x90\x90\x90\x90\x90\x90\x90" : orig_store_bytes, 8);
          VirtualProtect (limit_store_addr, 8, dwOrigProt,             &dwOrigProt);
          VirtualProtect (limit_check_addr, 2, PAGE_EXECUTE_READWRITE, &dwOrigProt);
          memcpy         (limit_check_addr, UncapFramerate   ?    (unsigned char *)
                                                  "\x90\x90" : orig_check_bytes, 2);
          VirtualProtect (limit_check_addr, 2, dwOrigProt,             &dwOrigProt);
        }
      }

      // Not tested adequately in non-framegen cases
      if (__SK_ACS_AlwaysUseFrameGen)
      {
        // Replace Ubisoft's poor excuse for a framerate limiter in FMVs with SK's.
        if      (ReadULongAcquire (&FrameGenDisabledForFMV))       __target_fps = 30.0f;
        else if ((! __SK_ACS_UncapFramerate) && game_limit > 0.0f) __target_fps = game_limit;
        else                                                       __target_fps = config.render.framerate.target_fps;
      }

      else
      {
        // Replace Ubisoft's poor excuse for a framerate limiter in FMVs with SK's.
        if      (ReadULongAcquire (&FrameGenDisabledForFMV))
        {
          __target_fps = 30.0f;
        }

        else if ((! __SK_ACS_UncapFramerate) && game_limit >   0.0f &&
                                                game_limit < 500.0f)
        {
          __target_fps = game_limit;
        }

        else
        {
          __target_fps = config.render.framerate.target_fps;
        }
      }

      // 3.333 second grace period after an FMV is read to reset frame generation
      if (LastTimeFMVChecked < SK::ControlPanel::current_time - 3333UL)
      {
        if (ReadULongAcquire (&FrameGenDisabledForFMV) != 0 || (pFrameGenEnabled != nullptr &&
                                                               *pFrameGenEnabled == false))
        {
          // Video is done playing, game has unlimited framerate again.
          if (game_limit != 30.0f)
          {
            SK_ACS_ApplyFrameGenOverride (__SK_ACS_AlwaysUseFrameGen);
            WriteULongRelease            (&FrameGenDisabledForFMV, FALSE);
          }
        }

        if (__SK_ACS_UncapFramerate && framerate_limit != nullptr)
        {
          // -1.0f = Unlimited (set by game in special cases)
          *framerate_limit = -1.0f;
        }
      }

      else if (__SK_ACS_AlwaysUseFrameGen && ReadULongAcquire (&FrameGenDisabledForFMV) != 0 && ( pFrameGenEnabled != nullptr &&
                                                                                                 *pFrameGenEnabled == false ))
      {
        if (__SK_ACS_ShowFMVIndicator)
        {
          SK_ImGui_CreateNotificationEx (
            "ACShadows.FMV", SK_ImGui_Toast::Other, nullptr, nullptr,
                   INFINITE, SK_ImGui_Toast::UseDuration  |
                             SK_ImGui_Toast::ShowNewest   | 
                             SK_ImGui_Toast::DoNotSaveINI |
                             SK_ImGui_Toast::Unsilencable,
            [](void*)->bool
            {
              ImColor fmv_color (
                1.0f, 0.941177f, 0.f,
                  static_cast <float> (
                    0.75 + 0.2 * std::cos (3.14159265359 *
                      (static_cast <double> (SK::ControlPanel::current_time % 2500) / 1750.0))
                  )
              );

              ImGui::BeginGroup  ();
              ImGui::TextColored (fmv_color, ICON_FA_FILM "  30 fps ");
              ImGui::EndGroup    ();

              return false;
            }
          );
        }
      }

      else
      {
        if (__SK_ACS_UncapFramerate && framerate_limit != nullptr)
        {
          // -1.0f = Unlimited (set by game in special cases)
          *framerate_limit = -1.0f;
        }
      }

      if (                                        __SK_ACS_AlwaysUseFrameGen)
        SK_RunOnce (SK_ACS_ApplyFrameGenOverride (__SK_ACS_AlwaysUseFrameGen));

      if (__SK_IsDLSSGActive)
      {
        static HMODULE
            hModSLDLSSG  = (HMODULE)-1;
        if (hModSLDLSSG == (HMODULE)-1)GetModuleHandleExW (GET_MODULE_HANDLE_EX_FLAG_PIN, L"sl.dlss_g.dll",
           &hModSLDLSSG);

        if (hModSLDLSSG != nullptr)
        {
          SK_RunOnce (
            slGetPluginFunction_pfn
            slGetPluginFunction =
           (slGetPluginFunction_pfn)SK_GetProcAddress (hModSLDLSSG,
           "slGetPluginFunction");

            slDLSSGSetOptions_pfn                       slDLSSGSetOptions =
           (slDLSSGSetOptions_pfn)slGetPluginFunction ("slDLSSGSetOptions");

            SK_CreateFuncHook   (     L"slDLSSGSetOptions",
                                        slDLSSGSetOptions,
                                 SK_ACS_slDLSSGSetOptions_Detour,
               static_cast_p2p <void> (&slDLSSGSetOptions_ACS_Original) );
            MH_EnableHook       (       slDLSSGSetOptions               );
          );
        }
      }
    });

    SK_Thread_CloseSelf ();

    return 0;
  });
#endif
}