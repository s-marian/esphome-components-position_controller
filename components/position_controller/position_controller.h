#pragma once


#include <memory>
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/output/float_output.h"


namespace esphome {

namespace position_controller {

class PositionController : public Component {
 public:
  PositionController();
  void setup() override;
  void dump_config() override;
  void loop() override ;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_actual_position_sensor ( sensor::Sensor *sensor) { actual_position_ = sensor; }
  void set_target_position_sensor ( sensor::Sensor *sensor) { target_position_ = sensor; }
  void set_target_position_offset_sensor ( sensor::Sensor *sensor) { target_position_offset_ = sensor; }
  void set_accel_update_freq(double freq) { accel_update_freq_ = freq; }
  void set_adjustment_threshold(double thr) { adjustment_thr_ = thr; }
  void set_rotation_offset(double offset) { rotation_offset_ = offset; }
  void set_disabled(bool val) { active_ = !val; }

  void set_pin_a(output::FloatOutput *pin_a) { pin_a_ = pin_a; }
  void set_pin_b(output::FloatOutput *pin_b) { pin_b_ = pin_b; }

 protected:
  bool initialized_{false};
  bool active_{true};
  double accel_update_freq_;
  double adjustment_thr_;
  int cycle_;
  uint32_t last_update_;
  double rotation_offset_ {0};
  
  output::FloatOutput *pin_a_;
  output::FloatOutput *pin_b_;

  sensor::Sensor *actual_position_, *target_position_;
  sensor::Sensor *target_position_offset_{nullptr};


  void set_motor(float direction);
  void adjust();


};


}  // namespace position_controller
}  // namespace esphome

