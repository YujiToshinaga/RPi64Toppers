#ifndef STDDEF_H
#define STDDEF_H

#define TRUE    1
#define FALSE   0

typedef signed char			int8_t;
typedef unsigned char		uint8_t;
typedef signed short		int16_t;
typedef unsigned short		uint16_t;
typedef signed int			int32_t;
typedef unsigned int		uint32_t;
typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;

static inline uint8_t rd_byte(const uint64_t mem)
{
	uint8_t data;
	data = *((const volatile uint8_t *)mem);
	return data;
}

static inline void wr_byte(uint64_t mem, uint8_t data)
{
	*((volatile uint8_t *)mem) = data;
}

static inline uint32_t rd_word(const uint64_t mem)
{
	uint32_t data;
	data = *((const volatile uint32_t *)mem);
	return data;
}

static inline void wr_word(uint64_t mem, uint32_t data)
{
	*((volatile uint32_t *)mem) = data;
}

#define assert(exp)		((void) 0)

#endif /* STDDEF_H */
