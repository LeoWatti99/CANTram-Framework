#include "ESP32_CANCore.h"
#include <driver/twai.h>
#include "Debug.h"
#include "CANCore.h"

/**
 * @brief Set the CAN transceiver pins.
 * @details configures the TX and RX pins for the CAN interface. Must be called before begin().
 *
 * @param txPin The TX pin number.
 * @param rxPin The RX pin number.
 * @return true if the pins were set successfully.
 * @return false if already initialized.
 */
bool ESP32_CANCore::setPins(int8_t txPin, int8_t rxPin) {
    // Store the pin configuration
    if(_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] Cannot change pins while initialized. Call setPins(...) before begin().");
        return false;
    }
    _txPin = txPin;
    _rxPin = rxPin;
    INFO_PRINTLN("[ESP32_CANCore] CAN pins set to TX: " + String(txPin) + ", RX: " + String(rxPin));
    return true;
}

/**
 * @brief Set the CAN bus baudrate.
 * @details Configures the CAN bus baudrate. Must be called before begin().
 *
 * @param baudrate The desired baudrate from the Baudrate enum.
 * @note If called after begin(), the baudrate will not be changed.
 */
bool ESP32_CANCore::setBaudrate(Baudrate baudrate) {
    if(_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] Cannot change baudrate while initialized. Call setBaudrate(...) before begin().");
        return false;
    }
    _baudrate = baudrate;
    INFO_PRINTLN("[ESP32_CANCore] Baudrate set to " + String(baudrate) + " bps");
    return true;
}

/**
 * @brief Initialize the CAN interface.
 * @details This function first checks if the interface is already initialized. If not, it prepares the general, timing, and filter configurations based on the stored settings. It then installs the TWAI driver, configures alerts, and starts the driver.
 *
 * @return true if initialization was successful.
 * @return false if initialization failed.
 */
bool ESP32_CANCore::begin() {
    bool success = true;

    //check if already initialized
    if(_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] CAN interface already initialized.");
        return false;
    }
    
    //Prepare CAN general configuration
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(gpio_num_t(_txPin), gpio_num_t(_rxPin), TWAI_MODE_NORMAL);
    
    //Prepare CAN timing configuration based on selected baudrate
    twai_timing_config_t t_config;
    switch(_baudrate) {
        case BR_125K:
            t_config = TWAI_TIMING_CONFIG_125KBITS();
            break;
        case BR_250K:
            t_config = TWAI_TIMING_CONFIG_250KBITS();
            break;
        case BR_500K:
            t_config = TWAI_TIMING_CONFIG_500KBITS();
            break;
        case BR_1M:
            t_config = TWAI_TIMING_CONFIG_1MBITS();
            break;
        default:
            ERROR_PRINTLN("[ESP32_CANCore] Invalid or unset baudrate. Cannot initialize CAN interface.");
            return false;
    }
    INFO_PRINTLN("[ESP32_CANCore] CAN timing configuration set for baudrate: " + String(_baudrate) + " bps");

    //Prepare CAN filter configuration to accept all messages
    twai_filter_config_t f_config;
    switch(_filterType){
        case FILTER_ACCEPT_ALL:
            f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
            INFO_PRINTLN("[ESP32_CANCore] CAN filter configuration set to accept all messages.");
            break;
        case FILTER_MATCH_ID:
            f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
            DEV_ERROR_PRINTLN("[ESP32_CANCore] FILTER_MATCH_ID not implemented yet.");
            break;
        case FILTER_MASKED:
            f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
            DEV_ERROR_PRINTLN("[ESP32_CANCore] FILTER_MASKED not implemented yet.");
            break;
        default:
            f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
            ERROR_PRINTLN("[ESP32_CANCore] Invalid filter type. Cannot initialize CAN interface.");
            success &= false;
    }
    

    
  //Treiber installieren
  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    ERROR_PRINTLN("[ESP32_CANCore] Failed to install TWAI driver!");
    return false;
  }

  // CAN Status Meldungen konfigurieren
  uint32_t alerts_to_enable=0;
  alerts_to_enable |= TWAI_ALERT_RX_DATA;           //set alert for received data
  alerts_to_enable |= TWAI_ALERT_ERR_PASS;          //set alert for error passive
  alerts_to_enable |= TWAI_ALERT_BUS_ERROR;        //set alert for bus error
  alerts_to_enable |= TWAI_ALERT_RX_QUEUE_FULL;    //set alert for RX queue full
  alerts_to_enable |= TWAI_ALERT_TX_FAILED;        //set alert for TX failed
  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
    INFO_PRINTLN("[ESP32_CANCore] CAN Alerts reconfigured");
  } else {
    ERROR_PRINTLN("[ESP32_CANCore] Failed to reconfigure alerts");
    return false;
  }

  //Treiber starten
  if (twai_start() != ESP_OK) {
    Serial.println("Failed to start TWAI driver!");
    return false;
  }
  _isInitialized = true;
  INFO_PRINTLN("[ESP32_CANCore] CAN interface initialized.");
  return true;
}

bool ESP32_CANCore::end() {
    if(!_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] CAN interface not initialized.");
        return false;
    }
    if(twai_stop() != ESP_OK) {
        ERROR_PRINTLN("[ESP32_CANCore] Failed to stop TWAI driver.");
        return false;
    }
    if(twai_driver_uninstall() != ESP_OK) {
        ERROR_PRINTLN("[ESP32_CANCore] Failed to uninstall TWAI driver.");
        return false;
    }
    _isInitialized = false;
    INFO_PRINTLN("[ESP32_CANCore] CAN interface deinitialized.");
    return true;
}

bool ESP32_CANCore::sendMessage(const CANMessage& message) {
    if(!_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] CAN interface not initialized. Cannot send message.");
        return false;
    }

    //Convert CANCore::CANMessage to twai_message_t
    twai_message_t twai_msg;
    twai_msg.identifier = message.id;
    twai_msg.data_length_code = message.length;
    memcpy(twai_msg.data, message.data, message.length);    //Copy message to twai_msg data array
    twai_msg.flags = 0;
    if(message.isExtended) {
        twai_msg.flags |= TWAI_MSG_FLAG_EXTD;
    }
    if(message.isRemote) {
        twai_msg.flags |= TWAI_MSG_FLAG_RTR;
    }

    if(twai_transmit(&twai_msg, pdMS_TO_TICKS(1000)) != ESP_OK) {
        ERROR_PRINTLN("[ESP32_CANCore] Failed to transmit CAN message.");
        return false;
    }
    DEBUG_PRINTLN("[ESP32_CANCore] CAN message transmitted. ID: " + String(message.id) + ", Length: " + String(message.length));
    return true;
}

/**
 * @brief Read a CAN message from the interface.
 *
 * @param message Reference to a CANCore::CANMessage object to store the received message.
 * @return True if a message was successfully read, false otherwise.
 */
bool ESP32_CANCore::readMessage(CANMessage& message) {
    CANMessage* canMsg = &message;
    if(!_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] CAN interface not initialized. Cannot read message.");
        canMsg->error = true;
        return false;
    }

    //read ESP32 specific twai_message_t
    if(available() == 0) {
        ERROR_PRINTLN("[ESP32_CANCore] No CAN message available to read.");
        canMsg->error = true;
        return false;
    }

    twai_message_t twai_msg;
    if(twai_receive(&twai_msg, pdMS_TO_TICKS(1000)) != ESP_OK) {
        ERROR_PRINTLN("[ESP32_CANCore] No CAN message available to read.");
        canMsg->error = true;
        return false;
    }

    //Convert twai_message_t to CANCore::CANMessage
    canMsg->id = twai_msg.identifier;
    canMsg->length = twai_msg.data_length_code;
    memcpy(canMsg->data, twai_msg.data, twai_msg.data_length_code); //Copy data from twai_msg to canMsg
    canMsg->isExtended = (twai_msg.flags & TWAI_MSG_FLAG_EXTD) != 0;
    canMsg->isRemote = (twai_msg.flags & TWAI_MSG_FLAG_RTR) != 0;

    DEBUG_PRINTLN("[ESP32_CANCore] CAN message received. ID: " + String(canMsg->id) + ", Length: " + String(canMsg->length));
    return true;
}

/** 
 * @brief Check how many CAN messages are available to read.
 * @details Queries the TWAI status information to determine how many messages are currently in the RX queue.
 *
 * @return uint8_t Number of available messages to read.
 */
uint8_t ESP32_CANCore::available() {
    if(!_isInitialized) {
        ERROR_PRINTLN("[ESP32_CANCore] CAN interface not initialized. Cannot check available messages.");
        return 0;
    }

    twai_status_info_t status_info;
    twai_get_status_info(&status_info);
    return status_info.msgs_to_rx;
}

bool ESP32_CANCore::setupFilter(uint32_t id, uint32_t mask) {
    DEV_ERROR_PRINTLN("[ESP32_CANCore] setupFilter not implemented yet.");
    return false;
}

bool ESP32_CANCore::reset() {
    INFO_PRINTLN("[ESP32_CANCore] Resetting ESP32 CAN core.");
    return CANCore::reset();
}

