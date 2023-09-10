#include "Random.h"

namespace Cookie::Random {

	f32 Float(f32 min, f32 max) {
		f32 random = (rand() / (f32)RAND_MAX); // [0, 1]
		return min + random * (max - min);
	}

	i32 Int(i32 min, i32 maxExclusive) { return (rand() % (maxExclusive - min) + min); }

} // namespace Cookie::Random