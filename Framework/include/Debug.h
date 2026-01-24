#pragma once

#include <Arduino.h>

/**
 * @file Debug.h
 * @brief Logging and debug macro definitions for CANTramCore.
 * @details Defines macros for debug, info, error, warning, developer error and chip-level logging.
 *          Macros prepend a level prefix and optionally include file/line information when DEBUG_SHOW_LOCATION is defined.
 *          Also provides a NULL pointer check macro that prints a critical error and halts execution when a null pointer is detected.
 */

/**
 * @brief Disable all output when defined.
 * @details Uncomment to completely disable all logging macros. If not defined, individual level macros control output.
 */
// #define NO_OUTPUT // Uncomment to disable all output
#ifndef NO_OUTPUT
#define DEBUG_OUTPUT          // Comment out to disable debug output
#define INFO_OUTPUT           // Comment out to disable info output
#define ERROR_OUTPUT          // Comment out to disable error output
#define WARNING_OUTPUT        // Comment out to disable warning output
#define CRITICAL_ERROR_OUTPUT // Comment out to disable critical error output
#define DEV_ERROR_OUTPUT
#define INSTRUCTION_OUTPUT    // Comment out to disable instruction output
#define CHIP_OUTPUT           // Comment out to disable chip output
#define MEASUREMENT_OUTPUT    // Comment out to disable measurement output
#endif

/**
 * @def DEBUG_SHOW_LOCATION
 * @brief When defined, logging macros append file and line information.
 * @details Define this macro to enable inclusion of the file and line number in each logging message. The MACRO_LOCATION macro expands
 *          to a newline followed by the file and line when enabled, otherwise it is empty.
 */
#ifdef DEBUG_SHOW_LOCATION
    #define MACRO_LOCATION ("\n" + String(__FILE__) + ":" + String(__LINE__))
#else
    #define MACRO_LOCATION ""
#endif

/**
 * @def DEBUG_PRINT(x)
 * @brief Print a debug message without newline.
 * @details Expands to Serial.print with "DEBUG: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef DEBUG_OUTPUT
#define DEBUG_PRINT(x) Serial.print((String) "DEBUG: " + x + MACRO_LOCATION)
#else
#define DEBUG_PRINT(x)
#endif



/**
 * @def DEBUG_PRINTLN(x)
 * @brief Print a debug message with newline.
 * @details Expands to Serial.println with "DEBUG: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef DEBUG_OUTPUT
#define DEBUG_PRINTLN(x) Serial.println((String) "DEBUG: " + x + MACRO_LOCATION)
#else
#define DEBUG_PRINTLN(x)
#endif

/**
 * @def MEASUREMENT_PRINT(x)
 * @brief Print a measurement  message without newline.
 * @details Expands to Serial.print with "MEASUREMENT: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef MEASUREMENT_OUTPUT
#define MEASUREMENT_PRINT(x) Serial.print((String) "MEASUREMENT: " + x + MACRO_LOCATION)
#else
#define MEASUREMENT_PRINT(x)
#endif



/**
 * @def MEASUREMENT_PRINTLN(x)
 * @brief Print a measurement message with newline.
 * @details Expands to Serial.println with "MEASUREMENT: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef MEASUREMENT_OUTPUT
#define MEASUREMENT_PRINTLN(x) Serial.println((String) "MEASUREMENT: " + x + MACRO_LOCATION)
#else
#define MEASUREMENT_PRINTLN(x)
#endif

/**
 * @def INSTRUCTION_PRINT(x)
 * @brief Print a INSTRUCTION message without newline.
 * @details Expands to Serial.print with "INSTRUCTION: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef INSTRUCTION_OUTPUT
#define INSTRUCTION_PRINT(x) Serial.print((String) "INSTRUCTION: " + x + MACRO_LOCATION)
#else
#define INSTRUCTION_PRINT(x)
#endif

/**
 * @def INSTRUCTION_PRINTLN(x)
 * @brief Print a INSTRUCTION message without newline.
 * @details Expands to Serial.println with "INSTRUCTION: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef INSTRUCTION_OUTPUT
#define INSTRUCTION_PRINTLN(x) Serial.println((String) "INSTRUCTION: " + x + MACRO_LOCATION)
#else
#define INSTRUCTION_PRINTLN(x)
#endif

/**
 * @def INFO_PRINT(x)
 * @brief Print an info-level message without newline.
 * @details Expands to Serial.print with "INFO: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef INFO_OUTPUT
#define INFO_PRINT(x) Serial.print((String) "INFO: " + x +  MACRO_LOCATION)
#else
#define INFO_PRINT(x)
#endif

/**
 * @def INFO_PRINTLN(x)
 * @brief Print an info-level message with newline.
 * @details Expands to Serial.println with "INFO: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef INFO_OUTPUT
#define INFO_PRINTLN(x) Serial.println((String) "INFO: " + x +  MACRO_LOCATION)
#else
#define INFO_PRINTLN(x)
#endif

/**
 * @def ERROR_PRINT(x)
 * @brief Print an error-level message without newline.
 * @details Expands to Serial.print with "ERROR: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef ERROR_OUTPUT
#define ERROR_PRINT(x) Serial.print((String) "ERROR: " + x +  MACRO_LOCATION)
#else
#define ERROR_PRINT(x)
#endif

/**
 * @def ERROR_PRINTLN(x)
 * @brief Print an error-level message with newline.
 * @details Expands to Serial.println with "ERROR: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef ERROR_OUTPUT
#define ERROR_PRINTLN(x) Serial.println((String) "ERROR: " + x +  MACRO_LOCATION)
#else
#define ERROR_PRINTLN(x)
#endif

/**
 * @def WARNING_PRINT(x)
 * @brief Print a warning-level message without newline.
 * @details Expands to Serial.print with "WARNING: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef WARNING_OUTPUT
#define WARNING_PRINT(x) Serial.print((String) "WARNING: " + x +  MACRO_LOCATION)
#else
#define WARNING_PRINT(x)
#endif

/**
 * @def WARNING_PRINTLN(x)
 * @brief Print a warning-level message with newline.
 * @details Expands to Serial.println with "WARNING: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef WARNING_OUTPUT
#define WARNING_PRINTLN(x) Serial.println((String) "WARNING: " + x +  MACRO_LOCATION)
#else
#define WARNING_PRINTLN(x)
#endif

/**
 * @def DEV_ERROR_PRINT(x)
 * @brief Print a developer-error-level message without newline.
 * @details For development-only diagnostics. Expands to Serial.print with "DEV_ERROR: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef DEV_ERROR_OUTPUT
#define DEV_ERROR_PRINT(x) Serial.print((String) "DEV_ERROR: " + x +  MACRO_LOCATION)
#else
#define DEV_ERROR_PRINT(x)
#endif

/**
 * @def DEV_ERROR_PRINTLN(x)
 * @brief Print a developer-error-level message with newline.
 * @details For development-only diagnostics. Expands to Serial.println with "DEV_ERROR: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef DEV_ERROR_OUTPUT
#define DEV_ERROR_PRINTLN(x) Serial.println((String) "DEV_ERROR: " + x +  MACRO_LOCATION)
#else
#define DEV_ERROR_PRINTLN(x)
#endif

/**
 * @def CHIP_PRINT(x)
 * @brief Print a chip-level message without newline.
 * @details Used for chip-related informational messages. Expands to Serial.print with "CHIP: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef CHIP_OUTPUT
#define CHIP_PRINT(x) Serial.print((String) "CHIP: " + x +  MACRO_LOCATION)
#else
#define CHIP_PRINT(x)
#endif

/**
 * @def CHIP_PRINTLN(x)
 * @brief Print a chip-level message with newline.
 * @details Used for chip-related informational messages. Expands to Serial.println with "CHIP: " prefix and optional file/line info.
 * @param x Expression or string to print (will be converted to String).
 */
#ifdef CHIP_OUTPUT
#define CHIP_PRINTLN(x) Serial.println((String) "CHIP: " + x +  MACRO_LOCATION)
#else
#define CHIP_PRINTLN(x)
#endif

/**
 * @def CRITICAL_ERROR_PRINT(x)
 * @brief Print a critical error message and halt execution.
 * @details Prints "CRITICAL ERROR: " prefix with the provided message and optional file/line info, then enters an infinite loop to stop execution.
 * @param x Expression or string to print (will be converted to String).
 * @note This macro does not return.
 */
#ifdef CRITICAL_ERROR_OUTPUT
#define CRITICAL_ERROR_PRINT(x)                                            \
    Serial.print((String) "CRITICAL ERROR: " + x +  MACRO_LOCATION); \
    while (true)
#else
#define CRITICAL_ERROR_PRINT(x)
#endif

/**
 * @def CRITICAL_ERROR_PRINTLN(x)
 * @brief Print a critical error message with newline and halt execution.
 * @details Prints "CRITICAL ERROR: " prefix with the provided message and optional file/line info, then enters an infinite loop to stop execution.
 * @param x Expression or string to print (will be converted to String).
 * @note This macro does not return.
 */
#ifdef CRITICAL_ERROR_OUTPUT
#define CRITICAL_ERROR_PRINTLN(x)                                            \
    Serial.println((String) "CRITICAL ERROR: " + x +  MACRO_LOCATION); \
    while (true)
#else
#define CRITICAL_ERROR_PRINTLN(x)
#endif

/**
 * @def NULL_PTR_CHECK(ptr)
 * @brief Check a pointer for null and report a critical error if null.
 * @details Evaluates the provided pointer expression and if it is nullptr, prints a critical error with the pointer name and halts execution.
 * @param ptr Pointer expression to check.
 * @return This macro does not return on failure; execution halts in an infinite loop after reporting the error.
 */
#define NULL_PTR_CHECK(ptr)                                      \
    if (ptr == nullptr) {                                     \
        CRITICAL_ERROR_PRINTLN("Null pointer exception: " + #ptr); \
    }

