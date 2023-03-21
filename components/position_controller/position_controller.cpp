
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
  direction_ =0;
  slow_start_ = 0;
  last_update_ = 0;
  slow_start_update_ = 0;
}

void PositionController::loop() {
    double adjustment_interval = 1000.0 / accel_update_freq_ / 2;
    double timer_val = adjustment_interval * 3;

    const uint32_t now = millis();
    const float slow_start_beginning = 0.4;
    const float slow_start_update_interval = 10; // ms
    const float slow_start_rise = 4; // seconds


    // every 1ms, increase slow start PWM
    // 3 seconds until reaching full speed
    if ( slow_start_update_ + slow_start_update_interval <= now ) {
        slow_start_ += ( ( 1 - slow_start_beginning ) * slow_start_update_interval/1000.0 ) / 3.0;
        if ( slow_start_ >= 1.0) {
            slow_start_ = 1.0;
        }
        slow_start_update_ = now;
        set_motor(direction_ * slow_start_ );
    }

    //set_motor(direction_  );

    if ( last_update_ + timer_val < now ) {
        //set_motor(0);
        direction_ = 0;
        return;
    }
    last_update_ = now;
    adjust();
    cycle_ = (cycle_ + 1) % 30;

    double target = -target_position_->get_state();
    double actual = actual_position_->get_state();

//    ESP_LOGD("MOTOR", "target = %f, actual = %f", target, actual);

    target += rotation_offset_;

    if ( target_position_offset_ != nullptr && target_position_offset_->has_state()) {
      target += target_position_offset_->get_state();
    }

    double err = target - actual;

    if ( abs(err) < adjustment_thr_ ) {
        //set_motor(0);
        direction_ = 0;
        return;
    }


    if ( cycle_ < 5 || ( abs(err) > 3*adjustment_thr_ ) ) {
        if ( err > 0 ) {
            //set_motor(1);
            if ( direction_ != 1) {
                slow_start_ = slow_start_beginning;
            }
            direction_ = 1;
        } else {
            //set_motor(-1);
            if ( direction_ != -1) {
                slow_start_ = slow_start_beginning;
            }
            direction_ = -1;
        }
    } else {
        //set_motor(0);
        direction_ = 0;
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
  //ESP_LOGD("MOTOR!!", "direction = %f", direction);

  if ( direction == 0 || !active_ ) {
      pin_a_->set_level(0);
      pin_b_->set_level(0);
  } else if ( direction > 0 ) {
      pin_a_->set_level(fabs(direction) );
      pin_b_->set_level(0);
  } else {
      pin_a_->set_level(0);
      pin_b_->set_level(fabs(direction) );
  }

}




}  // namespace position_controller
}  // namespace esphome

