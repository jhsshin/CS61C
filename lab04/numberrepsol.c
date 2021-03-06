#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "numberrep.h"

#define MAX_BASE 36

// A number can be interpreted as base N if it only contains characters chosen
// from the first N elements of this array.  
//
// Note that this implies that we support numbers up to base 36.
const char allowed_chars[MAX_BASE] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Validates a number given a base. If the number is invalid, this exits the
// program with a relevant error message.
//
// Params
// ------
// char *number :
//     The number we are validating.
// uint32_t base :
//     The base for which we are making sure number is valid. 
//     Must be less than MAX_BASE
//
// Examples
// --------
// validate_number("FFF", 16) // Valid, so nothing happens.
// validate_number( "10",  2) // Valid, so nothing happens.
// validate_number("FFF", 10) // Invalid, so the program exits.
// validate_number("100", 37) // Base is too high, so program exits.
void validate_number(char *number, uint32_t base) {
    if (base > MAX_BASE) {
        fprintf(stderr, "Base %d is too large", base);
        exit(1);
    }
    if (number == NULL) {
        fprintf(stderr, "Number is null");
        exit(1);
    }
    size_t length = strlen(number);
    for (size_t i = 0; i < length; i++) {
        int valid = 0;
        for (int j = 0; j < base; j++) {
            valid = valid || number[i] == allowed_chars[j];
        }
        if (!valid) {
            fprintf(stderr, "Number %s is invalid in base %d", number, base);
            exit(1);
        }
    }
}

// Converts a number to its corresponding unsigned representation in decimal.
//
// Params
// ------
// char *number :
//     The number we are converting to decimal.
//
// Examples
// --------
// convert_to_decimal("11Z", 36) // 1367
// convert_to_decimal("101",  2) // 5
// convert_to_decimal("ABC", 16) // 2748
uint64_t convert_to_decimal(char *number, uint32_t base) {
    validate_number(number, base);

    size_t length = strlen(number);
    uint64_t result = 0;
    uint64_t power_of_base = 1;
    for (size_t i = length; i > 0; i--) {
        size_t decimal_value = -1;
        for (size_t j = 0; j < MAX_BASE; j++) {
            if (allowed_chars[j] == number[i - 1]) {
                decimal_value = j;
                break;
            }
        }
        result += decimal_value * power_of_base;
        power_of_base *= base;
    }

    return result;
}

// Converts an unsigned decimal to the given base.
//
// Params
// ------
// uint64_t number :
//     The unsigned decimal number we are converting.
// uint32_t base :
//     The base we are converting to.
//
// Examples
// --------
// decimal_to_base(1367, 36) // "11Z"
// decimal_to_base(   5,  2) // "101"
// decimal_to_base(2748, 16) // "ABC"
char *decimal_to_base(uint64_t number, uint32_t base) {
    size_t allocated_space = 100;
    char *reversed = malloc(sizeof(char) * allocated_space);
    size_t length = 0;
    while (number) {
        // number = q * base + r
        uint64_t q = number / base;
        uint64_t r = number % base;
        reversed[length] = allowed_chars[r];
        number = q;
        length++;

        if (length >= allocated_space) {
            reversed = realloc(reversed, sizeof(char) * 2 * allocated_space);
            allocated_space *= 2;
        }
    }
    char *converted = malloc(sizeof(char) * (length + 1));
    for (int i = 0; i < length; i++) {
        converted[i] = reversed[length - i - 1];
    }
    converted[length] = '\0';
    free(reversed);
    return converted;
}

// Converts an unsigned number from one base to another.
// 
// Params
// ------
// char *number :
//     The number we are converting.
// uint32_t orig_base :
//     The original base for the number.
// uint32_t new_base :
//     The new base for the number.
//
// Examples
// --------
// convert_base("11Z", 36,  2) // "10101010111"
// convert_base("ABC", 16, 36) // "24C"
char *convert_base(char *number, uint32_t orig_base, uint32_t new_base) {
    validate_number(number, orig_base);
    return decimal_to_base(convert_to_decimal(number, orig_base), new_base);
}
