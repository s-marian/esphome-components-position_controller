
#include <memory>
#include "position_controller.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include "esphome/core/hal.h"


namespace esphome {

namespace position_controller {


PositionController::PositionController() {
}

void PositionController::setup() {
  set_motor(0);
  cycle_ = 0;
  last_update_ = 0;
}

void PositionController::loop() {
    double adjustment_interval = 1000.0 / accel_update_freq_ / 2;
    double timer_val = adjustment_interval * 3;

    const uint32_t now = millis();

    if ( last_update_ + timer_val < now ) {
        set_motor(0);
        return;
    }
    last_update_ = now;
    adjust();
    cycle_ = (cycle_ + 1) % 30;

    double target = -target_position_->get_state();
    double actual = actual_position_->get_state();


    target += rotation_offset_;

    if ( target_position_offset_ != nullptr && target_position_offset_->has_state()) {
      target += target_position_offset_->get_state();
    }

    double err = target - actual;

    if ( abs(err) < adjustment_thr_ ) {
        set_motor(0);
        return;
    }


    if ( cycle_ < 5 || ( abs(err) > 3*adjustment_thr_ ) ) {
        if ( err > 0 ) {
            set_motor(1);
        } else {
            set_motor(-1);
        }
    } else {
        set_motor(0);
    }

}

void PositionController::dump_config() {
}


void PositionController::adjust() {
    if ( ! ( actual_position_->has_state() && target_position_->has_state()) ) {
        ESP_LOGD("MOTOR", "Cannot find position states");
        return;
    }
}


void PositionController::set_motor(float direction) {

  if ( direction == 0 ) {
      pin_a_->set_level(0);
      pin_b_->set_level(0);
  } else if ( direction > 0 ) {
      pin_a_->set_level(abs(direction));
      pin_b_->set_level(0);
  } else {
      pin_a_->set_level(0);
      pin_b_->set_level(abs(direction));
  }

}




}  // namespace position_controller
}  // namespace esphome

