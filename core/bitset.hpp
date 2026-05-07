#pragma once 

#ifndef CORE_BIT_SET_HPP
#define CORE_BIT_SET_HPP

/*

		here a few macros to help you do bit operations

*/

// ready to use bit-masks 
#define BIT0() 0b00000001 
#define BIT1() 0b00000010
#define BIT2() 0b00000100
#define BIT3() 0b00001000
#define BIT4() 0b00010000
#define BIT5() 0b00100000
#define BIT6() 0b01000000
#define BIT7() 0b10000000

#define BIT8()  0x00000100ULL
#define BIT16() 0x00010000ULL
#define BIT31() 0x80000000ULL 
#define BIT63() 0x8000000000000000ULL		

// create bit-mask
#define BIT_MASK(VALUE) (1ULL << (VALUE))
#define INVERSE_BIT_MASK(VALUE) ~BIT_MASK(VALUE)

// macros for bit-manipulation
#define SET_BIT_ON(VARIABLE, BIT_INDEX)   VARIABLE |= BIT_MASK(BIT_INDEX)
#define SET_BIT_OFF(VARIABLE, BIT_INDEX)  VARIABLE &= ~BIT_MASK(BIT_INDEX)
#define FLIP_BIT(VARIABLE, BIT_INDEX)     VARIABLE ^= BIT_MASK(BIT_INDEX)
#define GET_BIT(VALUE, BIT_INDEX)   (( (VALUE) & (BIT_MASK(BIT_INDEX)) ) >> BIT_INDEX)
#define IS_BIT_ON(VALUE, BIT_INDEX) (( (VALUE) & (BIT_MASK(BIT_INDEX)) ) != 0)


// macros for max value
#define _8_BIT_   0xFF
#define _16_BIT_  0xFFFF
#define _32_BIT_  0xFFFFFFFF
#define _64_BIT_  0xFFFFFFFFFFFFFFFF

#endif