/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: sw=2 ts=8 et :
 */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma GCC visibility push(default)
#include "sync/sync.h"
#pragma GCC visibility pop

#include "mozilla/unused.h"
#include "nsXULAppAPI.h"

#include "FenceUtilsGonk.h"

using namespace mozilla::layers;

using base::FileDescriptor;

namespace IPC {

void
ParamTraits<FenceHandle>::Write(Message* aMsg,
                                const paramType& aParam)
{
  FenceHandle handle = aParam;
  if (handle.IsValid()) {
    int fd = handle.GetAndResetFd();
    aMsg->WriteFileDescriptor(FileDescriptor(fd, true));
  }
}

bool
ParamTraits<FenceHandle>::Read(const Message* aMsg,
                               void** aIter, paramType* aResult)
{
  FileDescriptor fd;
  if (aMsg->ReadFileDescriptor(aIter, &fd)) {
    aResult->Merge(FenceHandle(fd.fd));
  }
  return true;
}

} // namespace IPC

namespace mozilla {
namespace layers {

FenceHandle::FenceHandle()
  : mFence(new FdObj())
{
}

FenceHandle::FenceHandle(int aFenceFd)
  : mFence(new FdObj(aFenceFd))
{
}

void
FenceHandle::Merge(const FenceHandle& aFenceHandle)
{
  if (!aFenceHandle.IsValid()) {
    return;
  }

  if (!IsValid()) {
    mFence = aFenceHandle.mFence;
  } else {
    int result = sync_merge("FenceHandle", mFence->mFd, aFenceHandle.mFence->mFd);
    if (result == -1) {
      mFence = aFenceHandle.mFence;
    } else {
      mFence = new FdObj(result);
    }
  }
}

int
FenceHandle::GetAndResetFd()
{
  int fd = mFence->mFd;
  mFence->mFd = -1;
  return fd;
}

int
FenceHandle::GetDupFd()
{
  if (IsValid()) {
    return dup(mFence->mFd);
  }
  return -1;
}

} // namespace layers
} // namespace mozilla
