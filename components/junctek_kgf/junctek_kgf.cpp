#include "junctek_kgf.h"

#include "esphome/core/log.h"

#include <string>
#include <string.h>

static const char *const TAG = "JunkTek KG-F";


// Get a value where it's expected to be "<number>[,.], incrementing the cursor past the end"
int getval(const char*& cursor)
{
  const char* pos = cursor;
  char* end = nullptr;
  const long val = strtoll(pos, &end, 10);
//  ESP_LOGE("JunkTekKGF", "val = %f", val);
  if (end == pos || end == nullptr)
  {
    return -1;
  }
  if (*end != ',' && *end != '.')
  {
    return -2;
  }
  cursor = end + 1;
  return val;
}

JuncTekKGF::JuncTekKGF(unsigned address)
  : address_(address)
{
}

void JuncTekKGF::dump_config()
{
  ESP_LOGCONFIG(TAG, "junctek_kgf:");
  ESP_LOGCONFIG(TAG, "  address: %d", this->address_);
}

// void JuncTekKGF::handle_settings(const char* buffer)
// {
//   const char* cursor = buffer;
//   const int address = getval(cursor);
//   if (address != this->address_)
//     return;
//   const int checksum = getval(cursor);
//   if (! verify_checksum(checksum, cursor))
//     return;

//   const float overVoltage = getval(cursor) / 100.0;
//   const float underVoltage = getval(cursor) / 100.0;
//   const float positiveOverCurrent = getval(cursor) / 1000.0;
//   const float negativeOverCurrent = getval(cursor) / 100.00;
//   const float overPowerProtection = getval(cursor) / 100.00;
//   const float overTemperature = getval(cursor) - 100.0;
//   const int protectionRecoverySeconds = getval(cursor);
//   const int delayTime = getval(cursor);
//   const float batteryAmpHourCapacity = getval(cursor) / 10.0;
//   const int voltageCalibration = getval(cursor);
//   const int currentCalibration = getval(cursor);
//   const float temperatureCalibration = getval(cursor) - 100.0;
//   const int reserved = getval(cursor);
//   const int relayNormallyOpen = getval(cursor);
//   const int currentratio = getval(cursor);
//   // NOTE these are in the docs, but I don't seem to get them
//   //const int voltageCurveScale = getval(cursor);
//   //const int currentCurveScale = getval(cursor);

//   // Save the capacity for calculating the %
//   this->battery_capacity_ = batteryAmpHourCapacity;

//   this->last_settings_ = millis();
// }

void JuncTekKGF::handle_status(const char* buffer)
{
  const char* cursor = buffer;
//  ESP_LOGE("JunkTekKGF", "buffer = %s", buffer);
  const int address = getval(cursor);
  ESP_LOGE("JunkTekKGF", "address = %d", address);
   if (address != this->address_)
     return;

   const int checksum = getval(cursor);
   ESP_LOGE("JunkTekKGF", "checksum = %d", checksum);
//    if (! verify_checksum(checksum, cursor))
//    {
//     return;
//    } else 
//    {
//        ESP_LOGE("JunkTekKGF", "verify failed");
//    }

  const float voltage = getval(cursor) / 100.0;
  ESP_LOGE("JunkTekKGF", "voltage = %f", voltage);
  const float amps = getval(cursor) / 100.0;
  ESP_LOGE("JunkTekKGF", "amperage = %f", amps);
  const float ampHourRemaining = getval(cursor) / 1000.0;
  ESP_LOGE("JunkTekKGF", "amps left = %f", ampoHourRemaining);
  const float ampHourTotalUsed = getval(cursor) / 100.00;
  ESP_LOGE("JunkTekKGF", "amps used = %f", ampHourTotalUsed);
  const float wattHourRemaining = getval(cursor) / 100.0;
  ESP_LOGE("JunkTekKGF", "watts left = %f", wattHourRemaining);
  const float runtimeSeconds = getval(cursor);
  ESP_LOGE("JunkTekKGF", "runtime = %f", runtimeSeconds);
  const float temperature = getval(cursor) - 100.0;
  ESP_LOGE("JunkTekKGF", "temp = %f", temperature);
  const float powerInWatts = getval(cursor) / 100.0;
  ESP_LOGE("JunkTekKGF", "power = %f", powerInWatts);
  const int relayStatus = getval(cursor);
  ESP_LOGE("JunkTekKGF", "relays = %d", relayStatus);
  const int direction = getval(cursor);
  ESP_LOGE("JunkTekKGF", "direction = %d", direction);
  const int batteryLifeMinutes = getval(cursor);
  ESP_LOGE("JunkTekKGF", "batteryLifeMinutes = %f", batteryLifeMinutes);
//   const float batteryInternalOhms = getval(cursor) / 100.0;
//   ESP_LOGV("JunkTekKGF", "Recv %f %f %d %f %f %f", voltage, ampHourRemaining, direction, powerInWatts, amps, temperature);
//   if (voltage_sensor_)
//     this->voltage_sensor_->publish_state(voltage);
//   if (battery_level_sensor_ && this->battery_capacity_)
//     this->battery_level_sensor_->publish_state(ampHourRemaining * 100.0 / *this->battery_capacity_);
//   if (current_sensor_)
//     current_sensor_->publish_state(direction == 0 ? amps : - amps);
//   if (temperature_)
//     this->temperature_->publish_state(temperature);

   this->last_stats_ = millis();
}

void JuncTekKGF::handle_line()
{
  const char* buffer = &this->line_buffer_[0];
// ESP_LOGE("JunkTekKGF", "buffer = %s", buffer);
  if (buffer[0] != ':' || buffer[1] != 'r')
    return;
  if (strncmp(&buffer[2], "50=", 3) == 0)
    handle_status(&buffer[5]);
//  else if (strncmp(&buffer[2], "51=", 3) == 0)
//    handle_settings(&buffer[5]);

  return;
}

bool JuncTekKGF::readline()
{
  while (available()) {
    const char readch = read();
    if (readch > 0) {
      switch (readch) {
        case '\n': // Ignore new-lines
          break;
        case '\r': // Return on CR
          this->line_pos_ = 0;  // Reset position index ready for next time
          return true;
        default:
          if (this->line_pos_ < MAX_LINE_LEN - 1)
          {
            this->line_buffer_[this->line_pos_++] = readch;
            this->line_buffer_[this->line_pos_] = 0;
          }
      }
    }
  }
  return false;
}

bool JuncTekKGF::verify_checksum(int checksum, const char* buffer)
{
  long total = 0;

    while (true)
    {
      const int val = getval(buffer);
      ESP_LOGE("JunkTekKGF", "val = %d", val);
      total += val;
    }
  const bool checksum_valid = (total % 255) + 1 == checksum;
  ESP_LOGD("JunkTekKGF", "Recv checksum %d total %ld valid %d", checksum, total, checksum_valid);
  return checksum_valid;
//  return true;
}

void JuncTekKGF::loop()
{
  const unsigned long start_time = millis();

  if (!this->last_settings_ || (*this->last_settings_ + (30 * 1000)) < start_time)
  {
    this->last_settings_ = start_time;
    char buffer[20];
    sprintf(buffer, ":R51=%d,2,1,\r\n", this->address_);
    write_str(buffer);
//    ESP_LOGE("JunkTekKGF", "buffer = %s", buffer);
  }

  if (!this->last_stats_ || (*this->last_stats_ + (10 * 1000)) < start_time)
  {
    this->last_stats_ = start_time;
    char buffer[20];
    sprintf(buffer, ":R50=%d,2,1,\r\n", this->address_);
    write_str(buffer);
//    ESP_LOGE("JunkTekKGF", "buffer = %s", buffer);
  }


  if (readline())
//   {
//     ESP_LOGE("JunkTekKGF", "readline successful");
//   }
//   else
//   {
//       ESP_LOGE("JunkTekKGF", "readline fail");
//   }
  {
    handle_line();
  }
}

float JuncTekKGF::get_setup_priority() const
{
  return setup_priority::DATA;
}
