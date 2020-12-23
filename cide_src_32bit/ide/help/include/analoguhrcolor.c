/* --------------------- ANALOGUHRCOLOR.C -------------------

   Include - Datei zur Darstellung einer Analogen Uhr und
   deren Zeiger auf einem Farbdisplay

   22.08.2014 R. Seelig
   ---------------------------------------------------------- */
   
// ----------- Analoguhr -------------

uint8_t UHR_X = 64;                            // Mittelpunkt der Zeiger
uint8_t UHR_Y = 110;
#define	STD_ZEIGER_SIZE  21
#define MIN_ZEIGER_SIZE  27
#define UHR_RI           29                    // innere Skalierung
#define UHR_RA           32                    // aeussere Skalierung


#define BOGEN_0		0
#define COS_0		0
#define SIN_0		1
#define BOGEN_6		0.10471975511966
#define COS_6 		0.104528463267653
#define SIN_6 		0.994521895368273
#define BOGEN_12	0.20943951023932
#define COS_12		0.207911690817759
#define SIN_12		0.978147600733806
#define BOGEN_18	0.314159265358979
#define COS_18		0.309016994374947
#define SIN_18		0.951056516295153
#define BOGEN_24	0.418879020478639
#define COS_24		0.4067366430758
#define SIN_24		0.913545457642601
#define BOGEN_30 	0.523598775598299
#define COS_30		0.5
#define SIN_30		0.866025403784439
#define BOGEN_36	0.628318530717959
#define COS_36		0.587785252292473
#define SIN_36		0.809016994374947
#define BOGEN_42	0.733038285837618
#define COS_42		0.669130606358858
#define SIN_42		0.743144825477394
#define BOGEN_48	0.837758040957278
#define COS_48		0.743144825477394
#define SIN_48		0.669130606358858
#define BOGEN_54	0.942477796076938
#define COS_54		0.809016994374947
#define SIN_54		0.587785252292473
#define BOGEN_60	1.0471975511966,0
#define COS_60		0.866025403784439
#define SIN_60		0.5
#define BOGEN_66	1.15191730631626
#define COS_66		0.913545457642601
#define SIN_66		0.4067366430758
#define BOGEN_72	1.25663706143592
#define COS_72		0.951056516295153
#define SIN_72		0.309016994374947
#define BOGEN_78	1.36135681655558
#define COS_78		0.978147600733806
#define SIN_78		0.207911690817759
#define BOGEN_84	1.46607657167524
#define COS_84		0.994521895368273
#define SIN_84		0.104528463267653
#define BOGEN_90	1.5707963267949
#define COS_90		1
#define SIN_90		0


typedef const struct{
  int8_t x1;
  int8_t y1;
  int8_t x2;
  int8_t y2;
} line_typ;


const line_typ PROGMEM uhr[] = {
  {+UHR_RI *  COS_0, -UHR_RI *  SIN_0, +UHR_RA *  COS_0, -UHR_RA *  SIN_0},
  {+UHR_RI * COS_30, -UHR_RI * SIN_30, +UHR_RA * COS_30, -UHR_RA * SIN_30},
  {+UHR_RI * COS_60, -UHR_RI * SIN_60, +UHR_RA * COS_60, -UHR_RA * SIN_60},
  {+UHR_RI * COS_90, -UHR_RI * SIN_90, +UHR_RA * COS_90, -UHR_RA * SIN_90},

  {+UHR_RI * COS_60, +UHR_RI * SIN_60, +UHR_RA * COS_60, +UHR_RA * SIN_60},
  {+UHR_RI * COS_30, +UHR_RI * SIN_30, +UHR_RA * COS_30, +UHR_RA * SIN_30},
  {+UHR_RI *  COS_0, +UHR_RI *  SIN_0, +UHR_RA *  COS_0, +UHR_RA *  SIN_0},

  {-UHR_RA * COS_30, -UHR_RA * SIN_30, -UHR_RI * COS_30, -UHR_RI * SIN_30},
  {-UHR_RA * COS_60, -UHR_RA * SIN_60, -UHR_RI * COS_60, -UHR_RI * SIN_60},
  {-UHR_RA * COS_90, -UHR_RA * SIN_90, -UHR_RI * COS_90, -UHR_RI * SIN_90},

  {-UHR_RA * COS_60, +UHR_RA * SIN_60, -UHR_RI * COS_60, +UHR_RI * SIN_60},
  {-UHR_RA * COS_30, +UHR_RA * SIN_30, -UHR_RI * COS_30, +UHR_RI * SIN_30}
};

const uint8_t PROGMEM std_zeiger[] = {
  STD_ZEIGER_SIZE * COS_0,
  STD_ZEIGER_SIZE * COS_6,
  STD_ZEIGER_SIZE * COS_12,
  STD_ZEIGER_SIZE * COS_18,
  STD_ZEIGER_SIZE * COS_24,
  STD_ZEIGER_SIZE * COS_30,
  STD_ZEIGER_SIZE * COS_36,
  STD_ZEIGER_SIZE * COS_42,
  STD_ZEIGER_SIZE * COS_48,
  STD_ZEIGER_SIZE * COS_54,
  STD_ZEIGER_SIZE * COS_60,
  STD_ZEIGER_SIZE * COS_66,
  STD_ZEIGER_SIZE * COS_72,
  STD_ZEIGER_SIZE * COS_78,
  STD_ZEIGER_SIZE * COS_84,
  STD_ZEIGER_SIZE * COS_90
};

const uint8_t PROGMEM min_zeiger[] = {
  MIN_ZEIGER_SIZE * COS_0, 	// 0
  MIN_ZEIGER_SIZE * COS_6,	// 1
  MIN_ZEIGER_SIZE * COS_12,	// 2
  MIN_ZEIGER_SIZE * COS_18,	// 3
  MIN_ZEIGER_SIZE * COS_24,	// 4
  MIN_ZEIGER_SIZE * COS_30,	// 5
  MIN_ZEIGER_SIZE * COS_36,	// 6
  MIN_ZEIGER_SIZE * COS_42,	// 7
  MIN_ZEIGER_SIZE * COS_48,	// 8
  MIN_ZEIGER_SIZE * COS_54,	// 9
  MIN_ZEIGER_SIZE * COS_60,	// 10
  MIN_ZEIGER_SIZE * COS_66,	// 11
  MIN_ZEIGER_SIZE * COS_72,	// 12
  MIN_ZEIGER_SIZE * COS_78,	// 13
  MIN_ZEIGER_SIZE * COS_84,	// 14
  MIN_ZEIGER_SIZE * COS_90	// 15
};

void get_zeiger_value(uint8_t w, const uint8_t *tab_ptr, int8_t *x, int8_t *y){

  if (w < 15){
    *x = +pgm_read_byte(&tab_ptr[w]);
    *y = -pgm_read_byte(&tab_ptr[15-w]);
  }
  else{
    if (w < 30){
      *x = +pgm_read_byte(&tab_ptr[30-w]);
      *y = +pgm_read_byte(&tab_ptr[w-15]);
    }
    else{
      if (w < 45){
        *x = -pgm_read_byte(&tab_ptr[w-30]);
        *y = +pgm_read_byte(&tab_ptr[45-w]);
      }
      else{
        *x = -pgm_read_byte(&tab_ptr[60-w]);
        *y = -pgm_read_byte(&tab_ptr[w-45]);
      }
    }
  }
}

void analoguhr_skala(uint16_t color)
{
  uint8_t i;

  for(i=0; i<12; i++)
  {
    line( UHR_X + pgm_read_byte(&uhr[i].x1),
           UHR_Y + pgm_read_byte(&uhr[i].y1),
           UHR_X + pgm_read_byte(&uhr[i].x2),
           UHR_Y + pgm_read_byte(&uhr[i].y2),color);
  }
}

void analoguhr_zeiger(uint8_t std, uint8_t min, uint8_t sek, uint16_t color)
{
  uint8_t i;
  int8_t x, y;


  get_zeiger_value(min, min_zeiger, &x, &y);
  line(UHR_X, UHR_Y, UHR_X+x, UHR_Y+y,color);
  if (abs(x) > abs(y)){
    line(UHR_X, UHR_Y+1, UHR_X+x, UHR_Y+y+1,color);
  }
  else{
    line(UHR_X+1, UHR_Y, UHR_X+x+1, UHR_Y+y,color);
  }
  std = std * 5;
  while(min >= 12){
    min -= 12;
    std++;
  }
  get_zeiger_value(std, std_zeiger, &x, &y);
  line(UHR_X, UHR_Y, UHR_X+x, UHR_Y+y,color);
  if (abs(x) > abs(y)){
    line(UHR_X, UHR_Y+1, UHR_X+x, UHR_Y+y+1,color);
  }
  else{
    line(UHR_X+1, UHR_Y, UHR_X+x+1, UHR_Y+y,color);
  }
  get_zeiger_value(sek, min_zeiger, &x, &y);
  line(UHR_X, UHR_Y, UHR_X+x, UHR_Y+y,color);
}

// ------------------------------------------------------------------------------------------------------------------
