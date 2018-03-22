#include <stdarg.h>
#include "printk.h"

/*----------------------------------------------------------------------------*
 * Configuration
 *----------------------------------------------------------------------------*/

typedef signed int			int_t;
typedef unsigned int		uint_t;

typedef signed char			int8_t;
typedef unsigned char		uint8_t;

typedef signed short		int16_t;
typedef unsigned short		uint16_t;

typedef signed int			int32_t;
typedef unsigned int		uint32_t;

typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;

#define FALSE	0
#define TRUE	1

#define OVF_CHECK

#define IMP_DOUBLE
#define IMP_MULT
#define IMP_DIV

//#include <stdio.h>
#include "serial.h"

static inline void _putchar(char c)
{
	if (c == '\n') {
		serial_put('\n');
		serial_put('\r');
	} else {
		serial_put(c);
	}
}

/*----------------------------------------------------------------------------*
 * Function prototype
 *----------------------------------------------------------------------------*/
static inline uint_t __alt_mult_uint(
		uint_t multiplicand, uint_t multiplier);
static inline uint_t __alt_div_uint(
		uint_t dividend, uint_t divisor);
static inline uint_t __alt_rem_uint(
		uint_t dividend, uint_t divisor);
static inline uint_t __pow_uint(
		uint_t base, uint_t exponent);
static inline uint_t __log_uint(
		uint_t base, uint_t antilogarithm);

#ifdef IMP_DOUBLE
static inline uint64_t __alt_mult_uint64(
		uint64_t multiplicand, uint64_t multiplier);
static inline uint64_t __alt_div_uint64(
		uint64_t dividend, uint64_t divisor);
static inline uint64_t __alt_rem_uint64(
		uint64_t dividend, uint64_t divisor);
static inline uint64_t __pow_uint64(
		uint64_t base, uint64_t exponent);
static inline uint64_t __log_uint64(
		uint64_t base, uint64_t antilogarithm);
#endif

void _print_dec(
		uint_t val, uint_t zpad_flag, uint_t length, uint_t sign_flag);
void _print_hex(
		uint_t uval, uint_t zpad_flag, uint_t length);

#ifdef IMP_DOUBLE
void _print_dec64(
		uint64_t val, uint_t zpad_flag, uint_t length, uint_t sign_flag);
void _print_hex64(
		uint64_t uval, uint_t zpad_flag, uint_t length);
#endif

void _print_char(char c);
void _print_str(char *str);

/*----------------------------------------------------------------------------*
 * Local function : Math
 *----------------------------------------------------------------------------*/
/******** Multiply ********/
static inline uint_t __alt_mult_uint(
		uint_t multiplicand, uint_t multiplier)
{
	uint_t result;

#ifdef IMP_MULT

	result = multiplicand * multiplier;
#ifdef OVF_CHECK
	if (multiplicand != __alt_div_uint(result, multiplier)) {
		result = (uint_t)(-1);
	}
#endif

#else

	uint_t temp;
	int i;

	result = 0;
	temp = 0;
	for (i = 0; i < (int)multiplier; i++) {
		temp = result + multiplicand;
#ifdef OVF_CHECK
		if (temp < result) {
			result = (uint_t)(-1);
			break;
		}
#endif
		result = temp;
	}

#endif

	return result;
}

/******** Divide ********/
static inline uint_t __alt_div_uint(
		uint_t dividend, uint_t divisor)
{
	uint_t result;

#ifdef IMP_DIV

	result = dividend / divisor;

#else

	uint_t tmp;

	result = 0;
	tmp = dividend;
	while (tmp >= divisor) {
		result++;
		tmp = tmp - divisor;
	}

#endif

	return result;
}

/******** Remind ********/
static inline uint_t __alt_rem_uint(
		uint_t dividend, uint_t divisor)
{
	uint_t result;

#ifdef IMP_DIV

	result = dividend % divisor;

#else

	result = dividend;
	while (result >= divisor) {
		result = result - divisor;
	}

#endif

	return result;
}

/******** Power ********/
static inline uint_t __pow_uint(
		uint_t base, uint_t exponent)
{
	uint_t result;
	int i;

	result = 1;
	for (i = 0; i < (int)exponent; i++) {
		result = __alt_mult_uint(result, base);
#ifdef OVF_CHECK
		if (result == (uint_t)(-1)) {
			break;
		}
#endif
	}

	return result;
}

/******** Logalism ********/
static inline uint_t __log_uint(
		uint_t base, uint_t antilogarithm)
{
	uint_t result;
	uint_t pow;
	
	result = 0;
	pow = __pow_uint(base, result);
	while (pow <= antilogarithm) {
		result++;
		pow = __pow_uint(base, result);
#ifdef OVF_CHECK
		if (pow == (uint_t)(-1)) {
			break;
		}
#endif
	}
	result--;

	return result;
}

#ifdef IMP_DOUBLE

/******** Multiply ********/
static inline uint64_t __alt_mult_uint64(
		uint64_t multiplicand, uint64_t multiplier)
{
	uint64_t result;

#ifdef IMP_MULT

	result = multiplicand * multiplier;
#ifdef OVF_CHECK
	if (multiplicand != __alt_div_uint64(result, multiplier)) {
		result = (uint64_t)(-1);
	}
#endif

#else

	uint64_t temp;
	int i;

	result = 0;
	temp = 0;
	for (i = 0; i < (int)multiplier; i++) {
		temp = result + multiplicand;
#ifdef OVF_CHECK
		if (temp < result) {
			result = (uint64_t)(-1);
			break;
		}
#endif
		result = temp;
	}

#endif

	return result;

}

/******** Divide ********/
static inline uint64_t __alt_div_uint64(
		uint64_t dividend, uint64_t divisor)
{
	uint64_t result;

#ifdef IMP_DIV

	result = dividend / divisor;

#else

	uint64_t tmp;

	result = 0;
	tmp = dividend;
	while (tmp >= divisor) {
		result++;
		tmp = tmp - divisor;
	}

#endif

	return result;
}

/******** Remind ********/
static inline uint64_t __alt_rem_uint64(
		uint64_t dividend, uint64_t divisor)
{
	uint64_t result;

#ifdef IMP_DIV

	result = dividend % divisor;

#else

	result = dividend;
	while (result >= divisor) {
		result = result - divisor;
	}

#endif

	return result;
}

/******** Power ********/
static inline uint64_t __pow_uint64(
		uint64_t base, uint64_t exponent)
{
	uint64_t result;
	int i;

	result = 1;
	for (i = 0; i < (int)exponent; i++) {
		result = __alt_mult_uint64(result, base);
#ifdef OVF_CHECK
		if (result == (uint64_t)(-1)) {
			break;
		}
#endif
	}

	return result;
}

/******** Logalism ********/
static inline uint64_t __log_uint64(
		uint64_t base, uint64_t antilogarithm)
{
	uint64_t result;
	uint64_t pow;
	
	result = 0;
	pow = __pow_uint64(base, result);
	while (pow <= antilogarithm) {
		result++;
		pow = __pow_uint64(base, result);
#ifdef OVF_CHECK
		if (pow == (uint64_t)(-1)) {
			break;
		}
#endif
	}
	result--;

	return result;
}

#endif

/*----------------------------------------------------------------------------*
 * Local function : Print
 *----------------------------------------------------------------------------*/
/******** Singed Decimal Integer ********/
void _print_dec(
		uint_t val, uint_t zpad_flag, uint_t length,
		uint_t sign_flag)
{
	uint_t minus_flag;		// Plus : FALSE, Minus : TRUE
	uint_t uval;			// unsigned val
	uint_t digit_length;	// Digit length
	int_t pad_length;		// Padding length
	uint_t digit;			// Digit i
	int i;

	// Check Sign
	if ((sign_flag == TRUE) && ((int_t)val < 0)) {
		minus_flag = TRUE;
		uval = (uint_t)(-((int_t)val));
	} else {
		minus_flag = FALSE;
		uval = val;
	}

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
		digit_length = (uint_t)(__log_uint(10, uval) + 1);
	}

	// Print Pad and Sign
	if (zpad_flag == TRUE) {
		if (minus_flag == TRUE) {
			_putchar('-');
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		}
	} else {
		if (minus_flag == TRUE) {
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
			_putchar('-');
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
#ifdef IMP_DIV
		digit = (uint_t)((uval / __pow_uint(10, (uint_t)i)) % 10);
#else
		digit = (uint_t)__alt_rem_uint(
				__alt_div_uint(uval, __pow_uint(10, (uint_t)i)), 10);
#endif
		_putchar((char)((uint_t)'0' + digit));
	}
}

/******** Unsinged Hexadecimal Integer ********/
void _print_hex(
		uint_t uval, uint_t zpad_flag, uint_t length)
{
	uint_t digit_length;	// Digit length
	uint_t digit;			// Digit i
	int_t pad_length;		// Zero padding length
	uint_t temp_sa;			// shift amount
	int i;

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
//		digit_length = (uint_t)(__log_uint(16, uval) + 1);

		digit_length = 1;
		temp_sa = digit_length << 2;
		while ((temp_sa < (sizeof(uint_t) << 3)) &&
				((uval >> temp_sa) != 0)) {
			digit_length++;
			temp_sa = digit_length << 2;
		}
	}

	// Print Pad
	if (zpad_flag == TRUE) {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar('0');
		}
	} else {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar(' ');
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
//		digit = __alt_rem_uint(
//				__alt_div_uint(uval, __pow_uint(16, (uint_t)i)), 16);
		temp_sa = (uint_t)i << 2;
		digit = (uint_t)((uval >> temp_sa) & 0xf);

		if (digit >= 10) {
			_putchar((char)((uint_t)'a' + (digit - 10)));
		} else {
			_putchar((char)((uint_t)'0' + digit));
		}
	}
}

#ifdef IMP_DOUBLE

/******** Singed Decimal Integer ********/
void _print_dec64(
		uint64_t val, uint_t zpad_flag, uint_t length,
		uint_t sign_flag)
{
	uint_t minus_flag;		// Plus : FALSE, Minus : TRUE
	uint64_t uval;			// unsigned val
	uint_t digit_length;	// Digit length
	int_t pad_length;		// Padding length
	uint_t digit;			// Digit i
	int i;

	// Check Sign
	if ((sign_flag == TRUE) && ((int64_t)val < 0)) {
		minus_flag = TRUE;
		uval = (uint64_t)(-((int64_t)val));
	} else {
		minus_flag = FALSE;
		uval = val;
	}

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
		digit_length = (uint_t)(__log_uint64(10, uval) + 1);
	}

	// Print Pad and Sign
	if (zpad_flag == TRUE) {
		if (minus_flag == TRUE) {
			_putchar('-');
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		}
	} else {
		if (minus_flag == TRUE) {
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
			_putchar('-');
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
#ifdef IMP_DIV
		digit = (uint_t)((uval / __pow_uint64(10, (uint64_t)i)) % 10);
#else
		digit = (uint_t)__alt_rem_uint64(
				__alt_div_uint64(uval, __pow_uint64(10, (uint64_t)i)), 10);
#endif
		_putchar((char)((uint_t)'0' + digit));
	}
}

/******** Unsinged Hexadecimal Integer ********/
void _print_hex64(
		uint64_t uval, uint_t zpad_flag, uint_t length)
{
	uint_t digit_length;	// Digit length
	uint_t digit;			// Digit i
	int_t pad_length;		// Zero padding length
	uint_t temp_sa;			// shift amount
	int i;

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
//		digit_length = (uint_t)(__log_uint64(16, uval) + 1);

		digit_length = 1;
		temp_sa = digit_length << 2;
		while ((temp_sa < (sizeof(uint64_t) << 3)) &&
				((uval >> temp_sa) != 0)) {
			digit_length++;
			temp_sa = digit_length << 2;
		}
	}

	// Print Pad
	if (zpad_flag == TRUE) {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar('0');
		}
	} else {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar(' ');
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
//		digit = __alt_rem_uint64(
//				__alt_div_uint64(uval, __pow_uint64(16, (uint_t)i)), 16);
		temp_sa = (uint_t)i << 2;
		digit = (uint_t)((uval >> temp_sa) & 0xf);

		if (digit >= 10) {
			_putchar((char)((uint_t)'a' + (digit - 10)));
		} else {
			_putchar((char)((uint_t)'0' + digit));
		}
	}
}

#endif

/******** Character ********/
void _print_char(char c)
{
	_putchar(c);
}

/******** String of Characters ********/
void _print_str(char *str)
{
	while (*str != '\0') {
		_putchar(*str);
		str++;
	}
}

/*----------------------------------------------------------------------------*
 * Global function
 *----------------------------------------------------------------------------*/
/******** Simple Print ********/
void prints(const char *str)
{
	while (*str != '\0') {
		_putchar(*str);
		str++;
	}
}

/******** Kernel Print ********/
void printk(const char *fmt, ...)
{
	uint_t zpad_flag;	// ZERO Padding Flag
	uint_t length;		// Padding Length

	va_list ap;

	va_start(ap, fmt);

	while (*fmt != '\0') {
		if (*fmt == '%') {

			// Initialize Variable
			zpad_flag = FALSE;
			length = 0;

			fmt++;

			// Check Zero Padding
			if (*fmt == '0') {
				zpad_flag = TRUE;
				fmt++;
			}

			// Check Padding Length
			while ((*fmt >= '0') && (*fmt <= '9')) {
				length = length * 10 + (uint_t)(*fmt - '0');
				fmt++;
			}

			// Check Format Specifiers
			switch (*fmt) {
				case 'd':
					_print_dec((uint_t)va_arg(ap, int_t),
							zpad_flag, length, TRUE);
					fmt++;
					break;
				case 'u':
					_print_dec((uint_t)va_arg(ap, int_t),
							zpad_flag, length, FALSE);
					fmt++;
					break;
				case 'x':
					_print_hex((uint_t)va_arg(ap, int_t),
							zpad_flag, length);
					fmt++;
					break;
				case 'c':
					_print_char((char)va_arg(ap, int_t));
					fmt++;
					break;
				case 's':
					_print_str((char *)va_arg(ap, char *));
					fmt++;
					break;
#ifdef IMP_DOUBLE
				case 'l':
					fmt++;
					switch (*fmt) {
						case 'd':
							_print_dec64((uint64_t)va_arg(ap, uint64_t),
									zpad_flag, length, TRUE);
							fmt++;
							break;
						case 'u':
							_print_dec64((uint64_t)va_arg(ap, uint64_t),
									zpad_flag, length, FALSE);
							fmt++;
							break;
						case 'x':
							_print_hex64((uint64_t)va_arg(ap, uint64_t),
									zpad_flag, length);
							fmt++;
							break;
						default:
							fmt++;
							break;
					}
					break;
#endif
				default:
					fmt++;
					break;
			}
		} else {
			_putchar(*fmt);
			fmt++;
		}
	}

	va_end(ap);
}

