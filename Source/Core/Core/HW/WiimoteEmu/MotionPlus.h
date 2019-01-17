// Copyright 2019 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <array>

#include "Common/CommonTypes.h"
#include "Core/HW/WiimoteEmu/Dynamics.h"
#include "Core/HW/WiimoteEmu/ExtensionPort.h"
#include "Core/HW/WiimoteEmu/I2CBus.h"

namespace WiimoteEmu
{
struct AngularVelocity;

struct MotionPlus : public Extension
{
public:
  MotionPlus();

  void Update() override;
  void Reset() override;
  void DoState(PointerWrap& p) override;

  ExtensionPort& GetExtPort();

  // Vec3 is interpreted as radians/s about the x,y,z axes following the "right-hand rule".
  void PrepareInput(const Common::Vec3& angular_velocity);

private:
#pragma pack(push, 1)
  struct DataFormat
  {
    // yaw1, roll1, pitch1: Bits 0-7
    // yaw2, roll2, pitch2: Bits 8-13

    u8 yaw1;
    u8 roll1;
    u8 pitch1;

    u8 pitch_slow : 1;
    u8 yaw_slow : 1;
    u8 yaw2 : 6;

    u8 extension_connected : 1;
    u8 roll_slow : 1;
    u8 roll2 : 6;

    u8 zero : 1;
    u8 is_mp_data : 1;
    u8 pitch2 : 6;
  };

  struct Register
  {
    std::array<u8, 21> controller_data;
    u8 unknown_0x15[11];

    // address 0x20
    std::array<u8, 0x20> calibration_data;

    // address 0x40
    // Data is read from the extension on the passthrough port.
    std::array<u8, 0x10> passthrough_ext_calib;

    // address 0x50
    std::array<u8, 0x40> init_data;

    u8 unknown_0x90[0x60];

    // address 0xF0
    u8 initialized;

    // address 0xF1
    u8 init_stage;

    // address 0xF2
    // Games write 0x00 here twice to start and stop calibration.
    u8 calibration_trigger;

    // address 0xF3
    u8 unknown_0xf3[3];

    // address 0xF6
    // Value is taken from the extension on the passthrough port.
    u8 passthrough_ext_id_4;

    // address 0xF7
    // Games read this value to know when the data at 0x50 is ready.
    // Value is 0x02 upon activation.
    // Real M+ changes this value from 0x4, 0x8, 0xc, and finally 0xe.
    // Games then trigger a 2nd stage via a write to 0xf1.
    // Real M+ changes this value to 0x14, 0x18, and finally 0x1a.

    // Note: The speed of this value progression seems to be
    // greatly increased by the reading of regular controller data.

    // Note: We don't progress like this. We jump to the final value as soon as possible.
    u8 init_progress;

    // address 0xF8
    // Values are taken from the extension on the passthrough port.
    u8 passthrough_ext_id_0;
    u8 passthrough_ext_id_5;

    // address 0xFA
    std::array<u8, 6> ext_identifier;
  };
#pragma pack(pop)

  static_assert(sizeof(DataFormat) == 6, "Wrong size");
  static_assert(0x100 == sizeof(Register), "Wrong size");

  static constexpr u8 INACTIVE_DEVICE_ADDR = 0x53;
  static constexpr u8 ACTIVE_DEVICE_ADDR = 0x52;

  static constexpr u8 PASSTHROUGH_MODE_OFFSET = 0xfe;

  enum class PassthroughMode : u8
  {
    Disabled = 0x04,
    Nunchuk = 0x05,
    Classic = 0x07,
  };

  enum class ActivationStatus
  {
    Inactive,
    Activating,
    Deactivating,
    Active,
  };

  void Activate();
  void Deactivate();
  void OnPassthroughModeWrite();

  ActivationStatus GetActivationStatus() const;
  PassthroughMode GetPassthroughMode() const;

  int BusRead(u8 slave_addr, u8 addr, int count, u8* data_out) override;
  int BusWrite(u8 slave_addr, u8 addr, int count, const u8* data_in) override;

  bool ReadDeviceDetectPin() const override;
  bool IsButtonPressed() const override;

  Register m_reg_data = {};

  u8 m_activation_progress = {};

  // The port on the end of the motion plus:
  I2CBus m_i2c_bus;
  ExtensionPort m_extension_port{&m_i2c_bus};
};
}  // namespace WiimoteEmu
