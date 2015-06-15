#include <pal.h>

unsigned int normalizeRadiansToPlusMinusM_PI(float *radians) {
	unsigned int *radiansBits = (unsigned int *) radians;
	unsigned int signBit = *radiansBits & 0x80000000u;
	*radiansBits = *radiansBits ^ signBit; //remove any negative bit

	int revolutions = (int) (*radians * M_1_PI) + 1;
	revolutions = revolutions >> 1; // div 2

	*radians = *radians - revolutions * (2 * M_PI); //subtract whole revolutions, now in range [-pi..pi]

	*radiansBits = *radiansBits ^ signBit; //if was negative, flip negative bit
	return signBit;
}

unsigned int radiansToPlusMinusM_PI_2(float *radians) {
	unsigned int flip = 0;
	normalizeRadiansToPlusMinusM_PI(radians);

	if (*radians < -M_PI_2 || *radians > M_PI_2) {
		if (*radians < 0) {
			*radians += M_PI;
		} else {
			*radians -= M_PI;
		}
		flip ^= 0x80000000u; //flip the sign for second or third quadrant
	}
	return flip;
}

float myCosUnrolled(float x) {
	unsigned int flip = radiansToPlusMinusM_PI_2(&x);
	float x2 = x * x;
	float temp = x2;

	float res = 1;
	unsigned int *resBits = (unsigned int *) &res;

	res -= temp * (1.0f / 2); // 1/2!
	temp = temp * x2;
	res += temp * (1.0f / 24); // 1/4!
	temp = temp * x2;
	res -= temp * (1.0f / 720); // 1/6!
	temp = temp * x2;
	res += temp * (1.0f / 40320); // 1/8!

	*resBits = *resBits ^ flip; //flip sign if required

	return res;
}

/**
 *
 * Compute the cosine of the vector 'a'. Angles are specified in radians.
 * The radian number must be in the range 0 to 2pi,
 *
 * @param a     Pointer to input vector
 *
 * @param c     Pointer to output vector
 *
 * @param n     Size of 'a' and 'c' vector.
 *
 * @return      None
 *
 */
void p_cos_f32(const float *a, float *c, int n)
{

    int i;
    for (i = 0; i < n; i++) {
        const float *pa = (a+i);
        float *pc = (c+i);
        *pc = myCosUnrolled(*pa);
    }
}
