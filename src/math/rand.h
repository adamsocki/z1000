
typedef struct {
    int32 seed;
    int32 state;
} LCGState;

#define LCG_MULT 1103515245
#define LCG_INC 12345
//
////LCGState defaultLCGState;

#define MY_RAND_MAX ((1 << 31) - 1)

void SeedRand(int32 seed);
int32 Randi();
int32 RandUpper(int32 upperLimit);
int32 RandiRange(int32 lowerLimit, int32 upperLimit);
real32 Randf();
real32 RandfUpper(real32 upperLimit);
real32 RandfRange(real32 lowerLimit, real32 upperLimit);