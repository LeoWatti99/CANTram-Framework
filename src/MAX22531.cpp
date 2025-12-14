#include "MAX22531.h"
#include "Debug.h"
#include "TestingUtility.h"

/**
 * @brief Constructs a MAX22531 object with specified chip select and interrupt pins.
 * @param csPin Chip select pin for SPI communication.
 * @param intPin Interrupt pin (optional, set to -1 if unused).
 */
MAX22531::MAX22531(uint8_t csPin, uint8_t intPin) : SPIChip(csPin), _intPin(intPin)
{}

/**
 * @brief Initializes the MAX22531 chip and confirms communication.
 * @return true if initialization and communication are successful, false otherwise.
 */
bool MAX22531::begin()
{
    if (!SPI.bus()) {
        CHIP_PRINTLN("[MAX22531] SPI wasn't initialized. Please call SPI.begin() first.");
        return false;
    }
    if(_intPin != -1){
        useInt = true;
        CHIP_PRINTLN("[MAX22531] Using interrupt pin: " + String(_intPin));
    }
    SPIChip::deselectChip();

    //Attempt to clear POR flag
    CHIP_PRINTLN("[MAX22531] Attempting to initialize MAX22531...");
    for(int i=0; i<MAX_START_ATTEMPTS; i++){
        //try to clear POR flag
        uint8_t result = writeRegister(MAX22531_REG_CONTROL, (1 << MAX22531_BIT_CLEAR_POR_FLAG)); // Clear POR bit
        if(result != STATUS_OK) {
            CHIP_PRINTLN("[MAX22531] Failed to write register. Error Code: " + String(result));
            return false;
        }

        //read back ID register to confirm communication
        uint16_t readData;
        result = readRegister(MAX22531_REG_ID, &readData);
        if(result != STATUS_OK) {
            CHIP_PRINTLN("[MAX22531] Failed to read register. Error Code: " + String(result));
            return false;
        }

        //check if read data matches expected ID
        result = (readData >> MAX22531_BIT_POR_FLAG) & 0x01; // Check if POR flag is zero
        if(result != 0){
            CHIP_PRINTLN("[MAX22531] POR flag still set. Attempt " + String(i+1) + " of " + String(MAX_START_ATTEMPTS) + ".");
            delay(START_RETRY_DELAY_MS);
            continue;
        }
        CHIP_PRINTLN("[MAX22531] Communication confirmed.");
        break;
    }
    return true;
}

/**
 * @brief Writes a 16-bit value to a register on the MAX22531.
 * @param address Register address to write to.
 * @param data 16-bit data to write.
 * @return STATUS_OK on success, INVALID_REGISTER if address is invalid.
 */
uint8_t MAX22531::writeRegister(uint8_t address, uint16_t data)
{
    if(address >MAX22531_REG_CONTROL) return INVALID_REGISTER; // Invalid register address
    uint8_t highByte = (data >> 8) & 0xFF;
    uint8_t lowByte = data & 0xFF;
    CHIP_PRINTLN("[MAX22531] Writing to register: 0b" + addLeadingZeros(address, 8) + " or 0x" + String(address, HEX));
    address = (address << MAX22531_ADDRESS_OFFSET) | (1 << MAX22531_WRITE_BIT); // Shift address and set write bit
    CHIP_PRINTLN("[MAX22531] Transformed address with write bit: 0b" + addLeadingZeros(address, 8) +" or 0x" + String(address, HEX));
    SPI.beginTransaction(_spiSettings);

    //handle chip select
    SPIChip::selectChip();
    SPI.transfer(address); // Write bit and address    
    SPI.transfer(highByte);
    SPI.transfer(lowByte);
    //SPI.transfer(crc); // Send CRC if needed
    
    //handle chip select
    SPIChip::deselectChip();
    SPI.endTransaction();
    CHIP_PRINTLN("[MAX22531] Data written: 0b" + addLeadingZeros(highByte, 8) + " " + addLeadingZeros(lowByte, 8));
    return STATUS_OK;
}

/**
 * @brief Reads a 16-bit value from a register on the MAX22531.
 * @param address Register address to read from.
 * @param data Pointer to store the read 16-bit value. Must not be null.
 * @return STATUS_OK on success, INVALID_REGISTER if address is invalid, NULL_POINTER if data is null.
 */
uint16_t MAX22531::readRegister(uint8_t address, uint16_t* data){

    if(address >MAX22531_REG_CONTROL) return INVALID_REGISTER; // Invalid register address
    CHIP_PRINTLN("[MAX22531] Reading register: 0b" + addLeadingZeros(address, 8) + " or 0x" + String(address, HEX));
    address = (address << MAX22531_ADDRESS_OFFSET) | (0 << MAX22531_WRITE_BIT); // Shift address and set read bit
    CHIP_PRINTLN("[MAX22531] Transformed address with read bit: 0b" + addLeadingZeros(address, 8) +" or 0x" + String(address, HEX));
    
    uint16_t returnData;
    uint8_t highByte, lowByte;
    SPI.beginTransaction(_spiSettings);
    
    //handle chip select
    SPIChip::selectChip();

    //transfer address and data
    uint8_t dummy = SPI.transfer(address); 
    highByte = SPI.transfer(0x00);
    lowByte = SPI.transfer(0x00);
    //handle chip select
    SPIChip::deselectChip();

    SPI.endTransaction();
    returnData = (highByte << 8) | lowByte;
    CHIP_PRINTLN("[MAX22531] Raw data read: 0b" + addLeadingZeros(highByte, 8) + " " + addLeadingZeros(lowByte, 8));
    if(data==nullptr){
        CHIP_PRINTLN("[MAX22531] Null pointer provided for data storage.");
        return NULL_POINTER;
    } 
    CHIP_PRINTLN("[MAX22531] Processed data read: 0b" + addLeadingZeros(returnData >> 8, 8) + " " + addLeadingZeros(returnData & 0xFF, 8));
    *data = returnData;
    return STATUS_OK;
}

/**
 * @brief Performs a burst read of ADC channels and interrupt status from the MAX22531.
 * @param filtered If true, reads filtered ADC values; if false, reads unfiltered values.
 * @return BurstResponse struct containing ADC and interrupt data.
 */
MAX22531::BurstResponse MAX22531::burstRead(bool filtered)
{
    BurstResponse response;
    response.filtered = filtered;
    CHIP_PRINTLN("[MAX22531] Performing burst read. Filtered: " + String(filtered ? "Yes" : "No"));
    
    //determine starting address based on filtered or unfiltered
    uint8_t address = filtered ? MAX22531_FILTERED : MAX22531_UNFILTERED;

    //shift address and set burst bit
    address = (address << MAX22531_ADDRESS_OFFSET) | (0 << MAX22531_WRITE_BIT) | (1 << MAX22531_BURST_BIT);

    SPI.beginTransaction(_spiSettings);
    SPIChip::selectChip();
    SPI.transfer(address); // Burst read 
    response.adc1 = SPI.transfer16(0x0000);
    response.adc2 = SPI.transfer16(0x0000);
    response.adc3 = SPI.transfer16(0x0000);
    response.adc4 = SPI.transfer16(0x0000);
    response.interrupt = SPI.transfer16(0x0000);
    SPIChip::deselectChip();
    SPI.endTransaction();
    response.status = STATUS_OK;
    
    return response;
}

/**
 * @brief Reads a specific ADC channel from the MAX22531 and returns the 12-bit ADC value.
 * @param channel The ADC channel to read (0-3).
 * @param data Pointer to store the read ADC value. Must not be null.
 * @param filtered If true, reads from the filtered ADC register; if false, reads from the unfiltered ADC register.
 * @return STATUS_OK on success, INVALID_CHANNEL if the channel is out of range, NULL_POINTER if data is null.
 */
uint16_t MAX22531::readChannel(uint8_t channel, uint16_t* data,bool filtered)
{
    if(channel > 3) return INVALID_CHANNEL; // Invalid channel

    //Determine which register to read based on filtered or unfiltered
    uint8_t address = filtered ? MAX22531_REG_FADC_1_STATUS + channel : MAX22531_REG_ADC_1_STATUS + channel; 
    uint8_t result = readRegister(address, data);

    //Mask to 12 bits and print debug info
    CHIP_PRINTLN("[MAX22531] Channel " + String(channel+1) + " raw data read: " + String(*data));
    *data &= MAX22531_MASK_12_BIT; // Mask to 12 bits
    CHIP_PRINTLN("[MAX22531] Channel " + String(channel+1) + " processed data read (12-bit): " + String(*data));

    return result;  
}

/**
 * @brief Reads all filtered ADC channels and interrupt status from the MAX22531.
 * @return BurstResponse struct containing ADC and interrupt data.
 */
MAX22531::BurstResponse MAX22531::readAllChannels()
{
    return burstRead();
}

/**
 * @brief Performs a hardware reset of the MAX22531 via the CONTROL register.
 * @return STATUS_OK on success, error code otherwise.
 */
uint8_t MAX22531::HWreset(){
    
    CHIP_PRINTLN("[MAX22531] Performing hardware reset via CONTROL register.");
    bool result = writeRegister(MAX22531_REG_CONTROL, (1 << MAX22531_BIT_HW_RESET)); // Set POR bit
    delay(500);
    return result;
}