﻿/**
 * This file is part of Special K.
 *
 * Special K is free software : you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by The Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Special K is distributed in the hope that it will be useful,
 *
 * But WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Special K.
 *
 *   If not, see <http://www.gnu.org/licenses/>.
 *
**/

#ifndef __SK__GPU_MONITOR_H__
#define __SK__GPU_MONITOR_H__

#include <Windows.h>
#include <cstdint>

// 64?! Isn't that a bit ridiculous memory wise? But NvAPI wants it that big
#define MAX_GPUS 64

struct gpu_sensors_t
{
  struct
  {
    //
    // NV's driver only updates load % once per-second...
    //   (or so the API says, but that is wrong in practice).
    //
    struct
    {
      // In thousandths of a percent
      uint32_t gpu = 0; // GPU

      // NVIDIA ONLY
      struct
      {
        uint32_t fb  = 0; // Memory
        uint32_t vid = 0; // Video Encoder
        uint32_t bus = 0; // Bus
      };
    } loads_percent;

    struct
    {
      uint32_t gpu    = 0;
      uint32_t ram    = 0;
      //uint32_t shader = 0;
    } clocks_kHz;

    struct
    {
      float core      = 0.0f;
      float ov        = 0.0f;
      bool  over      = false;
      bool  supported = false;
    } volts_mV;

    struct
    {
      bool    supported = true; // Are _any_ supported?
      float   gpu       = 0.0f;
      int32_t ram       = 0;
      int32_t psu       = 0;
      int32_t pcb       = 0;
    } temps_c;

    struct
    {
      uint32_t gpu       = 0;
      bool     supported = false;
    } fans_rpm;

    //
    // NOTE: This isn't particularly accurate; it is a sum total rather than
    //         the ammount an application is using.
    //
    //         We can do better in Windows 10!  (*See DXGI 1.4 Memory Info)
    //
    struct
    {
      uint64_t local    = 0ULL;
      uint64_t nonlocal = 0ULL; // COMPUTED: (total - local)
      uint64_t total    = 0ULL;
      uint64_t capacity = 0ULL;
    } memory_B;

    struct
    {
      uint32_t mem_type           = 0UL; // 8 == GDDR5
      uint32_t mem_bus_width      = 0UL;

      uint32_t pcie_lanes         = 0UL;
      uint32_t pcie_gen           = 0UL;
      uint32_t pcie_transfer_rate = 0UL;

      double pcie_bandwidth_mb (void) noexcept
      {
        // 8b/10b
        if (pcie_gen == 2 || pcie_gen == 1 || pcie_gen == 0)
        {
          return (static_cast <double> (pcie_transfer_rate) / 8.0) * (0.8) * static_cast <double> (pcie_lanes);
        }

        //128b/130b
        else if (pcie_gen == 3 || pcie_gen == 4 || pcie_gen == 5)
        {
          return (static_cast <double> (pcie_transfer_rate) / 8.0) * (0.9846) * static_cast <double> (pcie_lanes);
        }

        // Something's rotten in GPU land.
        return 0.0;
      }
    } hwinfo;

    uint32_t nv_gpuid           = 0UL;
    uint32_t nv_perf_state      = 0UL;
    int      nv_perf_state_iter = 0;

    bool     has_nv_pstates     =  true;
    bool     queried_nv_pstates = false;

    struct {
      int64 phase0 = 0;
      int64 phase1 = 0;
    } amortization;
  } gpus [MAX_GPUS];

  int_fast32_t   num_gpus    =    0;
  ULARGE_INTEGER last_update = { 0ULL };
};

gpu_sensors_t*
  SK_GPU_CurrentSensorData (void);

void               SK_GPU_InitSensorData       (void);
uint32_t __stdcall SK_GPU_GetClockRateInkHz    (int gpu);
uint32_t __stdcall SK_GPU_GetMemClockRateInkHz (int gpu);
uint64_t __stdcall SK_GPU_GetMemoryBandwidth   (int GPU);
float    __stdcall SK_GPU_GetMemoryLoad        (int GPU);
float    __stdcall SK_GPU_GetGPULoad           (int GPU);
float    __stdcall SK_GPU_GetTempInC           (int gpu);
uint32_t __stdcall SK_GPU_GetFanSpeedRPM       (int gpu);
uint64_t __stdcall SK_GPU_GetVRAMUsed          (int gpu);
//uint64_t __stdcall SK_GPU_GetVRAMResident      (int gpu);  // TODO: Abstract DXGI 1.4 better
uint64_t __stdcall SK_GPU_GetVRAMShared        (int gpu);
uint64_t __stdcall SK_GPU_GetVRAMCapacity      (int gpu);
uint64_t __stdcall SK_GPU_GetVRAMBudget        (int gpu);

void SK_PollGPU       (void);
void SK_EndGPUPolling (void);

#endif /* __SK__GPU_MONITOR_H__ */