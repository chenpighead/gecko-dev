/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: sw=2 ts=8 et :
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_layers_FenceUtilsGonk_h
#define mozilla_layers_FenceUtilsGonk_h

#include <unistd.h>
#include "mozilla/RefPtr.h"    // for RefPtr
#include "ipc/IPCMessageUtils.h"

namespace mozilla {
namespace layers {

class FdObj {
  NS_INLINE_DECL_THREADSAFE_REFCOUNTING(FdObj)
  friend class FenceHandle;
public:
  FdObj()
    : mFd(-1) {}
  FdObj(int aFd)
    : mFd(aFd) {}
  virtual ~FdObj() {
    if (mFd != -1) {
      close(mFd);
    }
  }

private:
  int mFd;
};

struct FenceHandle {
public:
  FenceHandle();

  FenceHandle(int aFenceFd);

  bool operator==(const FenceHandle& aOther) const {
    return mFence.get() == aOther.mFence.get();
  }

  bool IsValid() const
  {
    return (mFence->mFd != -1);
  }

  void Merge(const FenceHandle& aFenceHandle);

  int GetAndResetFd();

  int GetDupFd();

private:
  RefPtr<FdObj> mFence;
};

} // namespace layers
} // namespace mozilla

namespace IPC {

template <>
struct ParamTraits<mozilla::layers::FenceHandle> {
  typedef mozilla::layers::FenceHandle paramType;

  static void Write(Message* aMsg, const paramType& aParam);
  static bool Read(const Message* aMsg, void** aIter, paramType* aResult);
};

} // namespace IPC

#endif  // mozilla_layers_FenceUtilsGonk_h
