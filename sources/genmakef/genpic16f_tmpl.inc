const 
  pic16f_src_zeilanz = 15;
  
var
  pic16f_templ    : array [0..pic16f_src_zeilanz] of string =  
  (
    '/* ---------------------------------------------------------- ',
    '   ---------------------------------------------------------- */',
    '',
    '#define EXTRN_XTAL       1',
    '',
    '#include "pic16f_init.h"',
    '',
    '/* --------------------------------------------------------------------------- ',
    '                                    M A I N',
    '   --------------------------------------------------------------------------- */',
    'int main(void)',
    '{',
    '  // Mainprogram starts here',
    '',
    '  while(1);',
    '}'
  );  
