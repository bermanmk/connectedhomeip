/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <algorithm>
#include <cstdint>

#include <core/CHIPError.h>
#include <core/Optional.h>
#include <core/PeerId.h>
#include <inet/InetLayer.h>
#include <lib/support/Span.h>

namespace chip {
namespace Mdns {

static constexpr uint16_t kMdnsPort = 5353;
// Need 8 bytes to fit a thread mac.
static constexpr size_t kMaxMacSize = 8;

enum class CommssionAdvertiseMode : uint8_t
{
    kCommissioning,
    kCommissionable,
};

template <class Derived>
class BaseAdvertisingParams
{
public:
    Derived & SetPort(uint16_t port)
    {
        mPort = port;
        return *reinterpret_cast<Derived *>(this);
    }
    uint64_t GetPort() const { return mPort; }

    Derived & EnableIpV4(bool enable)
    {
        mEnableIPv4 = enable;
        return *reinterpret_cast<Derived *>(this);
    }
    bool IsIPv4Enabled() const { return mEnableIPv4; }
    Derived & SetMac(chip::ByteSpan mac)
    {
        mMacLength = std::min(mac.size(), kMaxMacSize);
        memcpy(mMacStorage, mac.data(), mMacLength);
        return *reinterpret_cast<Derived *>(this);
    }
    const chip::ByteSpan GetMac() const { return chip::ByteSpan(mMacStorage, mMacLength); }

private:
    uint16_t mPort                   = CHIP_PORT;
    bool mEnableIPv4                 = true;
    uint8_t mMacStorage[kMaxMacSize] = {};
    size_t mMacLength                = 0;
}; // namespace Mdns

/// Defines parameters required for advertising a CHIP node
/// over mDNS as an 'operationally ready' node.
class OperationalAdvertisingParameters : public BaseAdvertisingParams<OperationalAdvertisingParameters>
{
public:
    // Amount of mDNS text entries required for this advertising type
    static constexpr uint8_t kNumAdvertisingTxtEntries = 2;
    static constexpr uint8_t kTxtMaxKeySize            = 3 + 1; // "CRI"/"CRA" as possible keys
    static constexpr uint8_t kTxtMaxValueSize          = 7 + 1; // Max for text representation of the 32-bit CRMP intervals

    OperationalAdvertisingParameters & SetPeerId(const PeerId & peerId)
    {
        mPeerId = peerId;
        return *this;
    }
    PeerId GetPeerId() const { return mPeerId; }

    OperationalAdvertisingParameters & SetCRMPRetryIntervals(uint32_t intervalIdle, uint32_t intervalActive)
    {
        mCrmpRetryIntervalIdle   = intervalIdle;
        mCrmpRetryIntervalActive = intervalActive;
        return *this;
    }
    void GetCRMPRetryIntervals(uint32_t & intervalIdle, uint32_t & intervalActive) const
    {
        intervalIdle   = mCrmpRetryIntervalIdle;
        intervalActive = mCrmpRetryIntervalActive;
    }

private:
    PeerId mPeerId;
    uint32_t mCrmpRetryIntervalIdle   = 0;
    uint32_t mCrmpRetryIntervalActive = 0;
};

class CommissionAdvertisingParameters : public BaseAdvertisingParams<CommissionAdvertisingParameters>
{
public:
    // Amount of mDNS text entries required for this advertising type
    static constexpr uint8_t kNumAdvertisingTxtEntries = 8;     // Min 1 - Max 8
    static constexpr uint8_t kTxtMaxKeySize            = 2 + 1; // "D"/"VP"/"CM"/"DT"/"DN"/"RI"/"PI"/"PH" as possible keys
    static constexpr uint8_t kTxtMaxValueSize          = 128;   // Max from PI - Pairing Instruction

    CommissionAdvertisingParameters & SetShortDiscriminator(uint8_t discriminator)
    {
        mShortDiscriminator = discriminator;
        return *this;
    }
    uint8_t GetShortDiscriminator() const { return mShortDiscriminator; }

    CommissionAdvertisingParameters & SetLongDiscrimininator(uint16_t discriminator)
    {
        mLongDiscriminator = discriminator;
        return *this;
    }
    uint16_t GetLongDiscriminator() const { return mLongDiscriminator; }

    CommissionAdvertisingParameters & SetVendorId(Optional<uint16_t> vendorId)
    {
        mVendorId = vendorId;
        return *this;
    }
    Optional<uint16_t> GetVendorId() const { return mVendorId; }

    CommissionAdvertisingParameters & SetProductId(Optional<uint16_t> productId)
    {
        mProductId = productId;
        return *this;
    }
    Optional<uint16_t> GetProductId() const { return mProductId; }

    CommissionAdvertisingParameters & SetPairingInstr(Optional<const char *> pairingInstr)
    {
        mPairingInstr = pairingInstr;
        return *this;
    }
    Optional<const char *> GetPairingInstr() const { return mPairingInstr; }

    CommissionAdvertisingParameters & SetPairingHint(Optional<uint8_t> pairingHint)
    {
        mPairingHint = pairingHint;
        return *this;
    }
    Optional<uint8_t> GetPairingHint() const { return mPairingHint; }

    CommissionAdvertisingParameters & SetCommissionAdvertiseMode(CommssionAdvertiseMode mode)
    {
        mMode = mode;
        return *this;
    }
    CommssionAdvertiseMode GetCommissionAdvertiseMode() const { return mMode; }

private:
    uint8_t mShortDiscriminator  = 0;
    uint16_t mLongDiscriminator  = 0; // 12-bit according to spec
    CommssionAdvertiseMode mMode = CommssionAdvertiseMode::kCommissioning;
    chip::Optional<uint16_t> mVendorId;
    chip::Optional<uint16_t> mProductId;
    chip::Optional<const char *> mPairingInstr;
    chip::Optional<uint8_t> mPairingHint;
};

/// Handles advertising of CHIP nodes
class ServiceAdvertiser
{
public:
    virtual ~ServiceAdvertiser() {}

    /// Starts the advertiser. Items 'Advertised' will become visible.
    /// Must be called before Advertise() calls.
    virtual CHIP_ERROR Start(chip::Inet::InetLayer * inetLayer, uint16_t port) = 0;

    /// Advertises the CHIP node as an operational node
    virtual CHIP_ERROR Advertise(const OperationalAdvertisingParameters & params) = 0;

    /// Advertises the CHIP node as a commisioning/commissionable node
    virtual CHIP_ERROR Advertise(const CommissionAdvertisingParameters & params) = 0;

    /// Provides the system-wide implementation of the service advertiser
    static ServiceAdvertiser & Instance();
};

} // namespace Mdns
} // namespace chip
