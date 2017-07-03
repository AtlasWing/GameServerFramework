#ifndef __TAF_ATOMIC__
#define __TAF_ATOMIC__

#include <sys/cdefs.h>
__BEGIN_DECLS

#define TAF_LOCK "lock ; "

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile int counter; } taf_atomic_t;

#define TAF_ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically reads the value of @v.
 */ 
#define taf_atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @v: pointer of type taf_atomic_t
 * @i: required value
 * 
 * Atomically sets the value of @v to @i.
 */ 
#define taf_atomic_set(v,i)		(((v)->counter) = (i))

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically adds @i to @v.
 */
static __inline__ void taf_atomic_add(int i, taf_atomic_t *v)
{
	__asm__ __volatile__(
		TAF_LOCK "addl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically subtracts @i from @v.
 */
static __inline__ void taf_atomic_sub(int i, taf_atomic_t *v)
{
	__asm__ __volatile__(
		TAF_LOCK "subl %1,%0"
		:"=m" (v->counter)
		:"ir" (i), "m" (v->counter));
}

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static __inline__ int taf_atomic_sub_and_test(int i, taf_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		TAF_LOCK "subl %2,%0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically increments @v by 1.
 */ 
static __inline__ void taf_atomic_inc(taf_atomic_t *v)
{
	__asm__ __volatile__(
		TAF_LOCK "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically decrements @v by 1.
 */ 
static __inline__ void taf_atomic_dec(taf_atomic_t *v)
{
	__asm__ __volatile__(
		TAF_LOCK "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */ 
static __inline__ int taf_atomic_dec_and_test(taf_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		TAF_LOCK "decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic_inc_and_test - increment and test 
 * @v: pointer of type taf_atomic_t
 * 
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */ 
static __inline__ int taf_atomic_inc_and_test(taf_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		TAF_LOCK "incl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

/**
 * atomic_add_negative - add and test if negative
 * @v: pointer of type taf_atomic_t
 * @i: integer value to add
 * 
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */ 
static __inline__ int taf_atomic_add_negative(int i, taf_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		TAF_LOCK "addl %2,%0; sets %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v->counter) : "memory");
	return c;
}

/**
 * atomic_add_return - add and return
 * @v: pointer of type taf_atomic_t
 * @i: integer value to add
 *
 * Atomically adds @i to @v and returns @i + @v
 */
static __inline__ int taf_atomic_add_return(int i, taf_atomic_t *v)
{
	/* Modern 486+ processor */
	int __i = i;
	__asm__ __volatile__(
		TAF_LOCK "xaddl %0, %1;"
		:"=r"(i)
		:"m"(v->counter), "0"(i));
	return i + __i;
}

static __inline__ int taf_atomic_sub_return(int i, taf_atomic_t *v)
{
	return taf_atomic_add_return(-i,v);
}

#define taf_atomic_inc_return(v)  (taf_atomic_add_return(1,v))
#define taf_atomic_dec_return(v)  (taf_atomic_sub_return(1,v))

/* These are x86-specific, used by some header files */
#define taf_atomic_clear_mask(mask, addr) \
__asm__ __volatile__(TAF_LOCK "andl %0,%1" \
: : "r" (~(mask)),"m" (*addr) : "memory")

#define taf_atomic_set_mask(mask, addr) \
__asm__ __volatile__(TAF_LOCK "orl %0,%1" \
: : "r" (mask),"m" (*(addr)) : "memory")

__END_DECLS
#endif
