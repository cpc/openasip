volatile unsigned int i1 = 1;
volatile unsigned int i2 = 2;

volatile int i3 = 3;
volatile int i4 = 4;

volatile int i_5 = -5;
volatile int i_6 = -6;

volatile float f7 = 7.0;
volatile float f8 = 8.0;

#define MIN(I,J) (I<J?I:J)
#define MAX(I,J) (I>J?I:J)

void main(void) {
    unsigned int ii1 = i1;
    unsigned int ii2 = i2;
    int ii3 = i3;
    int ii4 = i4;
    int ii_5 = i_5;
    int ii_6 = i_6;
    float ff7 = f7;
    float ff8 = f8;
    _TCE_STDOUT(MIN(ii1,ii2)+48);
    _TCE_STDOUT(MAX(ii1,ii2)+48);

    _TCE_STDOUT(48 - (MIN(ii_5,ii_6)));
    _TCE_STDOUT(48 - (MAX(ii_5,ii_6)));

    _TCE_STDOUT(48 - (MIN(ii3, ii_5)));
    _TCE_STDOUT(48 + (MAX(ii4, ii_6)));

    _TCE_STDOUT((int)(MIN(ff7,ff8)) + 48);
    _TCE_STDOUT((int)(MAX(ff7,ff8)) + 48);

    _TCE_STDOUT(48 - (int)(MIN(ff7,-ff8)));
    _TCE_STDOUT(48 + (int)(MAX(ff7,-ff8)));
}
