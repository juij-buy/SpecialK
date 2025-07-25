﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/**
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

#include <SpecialK/stdafx.h>

#ifdef  __SK_SUBSYSTEM__
#undef  __SK_SUBSYSTEM__
#endif
#define __SK_SUBSYSTEM__ L" DStorage "

#include <SpecialK/render/dstorage/dstorage.h>
#include <SpecialK/render/dstorage/dstorage_queue.h>
#include <SpecialK/render/dstorage/dstorage_factory.h>

DSTORAGE_COMPRESSION_SUPPORT SK_DStorage_GDeflateSupport = { };

HRESULT
STDMETHODCALLTYPE
SK_IWrapDStorageFactory::CreateQueue (const DSTORAGE_QUEUE_DESC *desc, REFIID riid, _COM_Outptr_ void **ppv)
{
  if (ppv == nullptr || desc == nullptr)
    return E_POINTER;

  *ppv = nullptr;

  DSTORAGE_QUEUE_DESC override_desc = *desc;
  
  SK_LOGi0 (
    L"SK_IWrapDStorageFactory::CreateQueue (Priority=%ws, Name=%hs, Capacity=%d)",
      SK_DStorage_PriorityToStr (desc->Priority),
                                 desc->Name != nullptr ?
                                 desc->Name : "Unnamed",
                                 desc->Capacity);

  if (desc->Device == nullptr && config.render.dstorage.use_dummy_d3d12_dev)
  {
    // Singleton device
    static SK_ComPtr <ID3D12Device> pDevice;

    if ( D3D12CreateDevice_Import != nullptr &&
// Requires Windows SDK 22000
#if defined (NTDDI_WIN10_CO) && (WDK_NTDDI_VERSION >= NTDDI_WIN10_CO)
        (SUCCEEDED (
           D3D12CreateDevice_Import (
             nullptr,     D3D_FEATURE_LEVEL_12_2,
               __uuidof (ID3D12Device), (void **)&pDevice.p )
         ) ||
#endif
         SUCCEEDED (
           D3D12CreateDevice_Import (
             nullptr,     D3D_FEATURE_LEVEL_12_1,
               __uuidof (ID3D12Device), (void **)&pDevice.p )
         ) ||
         SUCCEEDED (
           D3D12CreateDevice_Import (
             nullptr,     D3D_FEATURE_LEVEL_12_0,
               __uuidof (ID3D12Device), (void **)&pDevice.p )
         ) ||
        SUCCEEDED (
           D3D12CreateDevice_Import (
             nullptr,     D3D_FEATURE_LEVEL_11_1,
               __uuidof (ID3D12Device), (void **)&pDevice.p )
         ) ||
         SUCCEEDED (
           D3D12CreateDevice_Import (
             nullptr,     D3D_FEATURE_LEVEL_11_0,
               __uuidof (ID3D12Device), (void **)&pDevice.p )
         )
        )
       )
    {
      SK_LOGi0 (L"Adding D3D12 Device to DirectStorage queue desc.");
      override_desc.Device = pDevice.Detach ();
    }
  }
  
  if (SK_GetCurrentGameID () == SK_GAME_ID::RatchetAndClank_RiftApart)
  {
    if (desc->Name != nullptr)
    {
      auto &dstorage =
        SK_GetDLLConfig ()->get_section (L"RatchetAndClank.DStorage");
  
      if (0 == _stricmp (desc->Name, "Bulk"))
      {
        if (! dstorage.contains_key  (L"BulkPriority"))
              dstorage.add_key_value (L"BulkPriority", L"Normal");
  
        override_desc.Priority =
          SK_DStorage_PriorityFromStr (dstorage.get_value (L"BulkPriority").c_str ());
      }
  
      else if (0 == _stricmp (desc->Name, "Loose reads"))
      {
        if (! dstorage.contains_key  (L"LooseReadPriority"))
              dstorage.add_key_value (L"LooseReadPriority", L"High");
  
        override_desc.Priority =
          SK_DStorage_PriorityFromStr (dstorage.get_value (L"LooseReadPriority").c_str ());
      }
  
      else if (0 == _stricmp (desc->Name, "Texture"))
      {
        if (! dstorage.contains_key  (L"TexturePriority"))
              dstorage.add_key_value (L"TexturePriority", L"Low");
  
        override_desc.Priority =
          SK_DStorage_PriorityFromStr (dstorage.get_value (L"TexturePriority").c_str ());
      }
  
      else if (0 == _stricmp (desc->Name, "NxStorage Index"))
      {
        if (! dstorage.contains_key  (L"NxStorageIndexPriority"))
              dstorage.add_key_value (L"NxStorageIndexPriority", L"Realtime");
  
        override_desc.Priority =
          SK_DStorage_PriorityFromStr (dstorage.get_value (L"NxStorageIndexPriority").c_str ());
      }
    }
  }

  if (SK_GetCurrentGameID () == SK_GAME_ID::FinalFantasyXVI)
  {
    if (override_desc.Priority == DSTORAGE_PRIORITY_REALTIME)
    {
      static int realtime_count = 0;

      if (realtime_count++ < 7)
      {
        override_desc.Capacity =
          (uint16_t)((float)override_desc.Capacity);
      }

      else if (realtime_count++ < 12)
      {
        override_desc.Capacity =
          (uint16_t)((float)override_desc.Capacity * 0.5f);
      }

      else
      {
        realtime_count++;
        override_desc.Capacity =
          (uint16_t)((float)override_desc.Capacity * 0.25f);
      }
    }

    else if (override_desc.Priority == DSTORAGE_PRIORITY_HIGH)
    {
      override_desc.Capacity =
        (uint16_t)((float)override_desc.Capacity * 0.5f);
    }

    else if (override_desc.Priority == DSTORAGE_PRIORITY_NORMAL)
    {
      static int normal_count = 0;
      if (normal_count++ < 14)
      {
        override_desc.Capacity =
          (uint16_t)((float)override_desc.Capacity);
      }

      else
        override_desc.Capacity =
          (uint16_t)((float)override_desc.Capacity * 0.5f);
    }
  }

  IDStorageQueue *pQueue = nullptr;

  HRESULT hr = E_NOTIMPL;

  IID  iid_queue2 = __uuidof (IDStorageQueue2);
  if (riid ==       __uuidof (IDStorageQueue)||
      riid ==       __uuidof (IDStorageQueue1))
  {
    hr = 
      pReal->CreateQueue (&override_desc, iid_queue2, (void **)&pQueue);
  }

  auto& _riid = iid_queue2;

  if (hr != S_OK)
  {
    hr =
      pReal->CreateQueue (&override_desc, riid, (void **)&pQueue);

    _riid = riid;
  }
  
  if (SUCCEEDED (hr))
  {
    if (SK_DStorage_IsWrappableQueueType (_riid))
    {
      *ppv =
        new SK_IWrapDStorageQueue (pQueue);

      SK_RunOnce (
      {
        SK_ComQIPtr <IDStorageQueue2>
            queue2 (pQueue);
        if (queue2 != nullptr)
        {
          SK_DStorage_GDeflateSupport =
            queue2->GetCompressionSupport (DSTORAGE_COMPRESSION_FORMAT_GDEFLATE);
        }
      });
    }

    else
    {
      *ppv = pQueue;

      wchar_t                wszGUID [41] = { };
      StringFromGUID2 (riid, wszGUID, 40);

      SK_LOGi0 (L"Unsupported Queue IID: %ws", wszGUID);
    }
  }

  return hr;
}

DSTORAGE_COMPRESSION_SUPPORT
SK_DStorage_GetGDeflateSupport (void)
{
  return
    SK_DStorage_GDeflateSupport;
}