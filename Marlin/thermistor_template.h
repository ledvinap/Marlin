// adaptation of createTemperatureLookupMarlin.py for cpp

/*
# Creates a C code lookup table for doing ADC to temperature conversion
# on a microcontroller
# based on: http://hydraraptor.blogspot.com/2007/10/measuring-temperature-easy-way.html
"""Thermistor Value Lookup Table Generator

Generates lookup to temperature values for use in a microcontroller in C format based on:
http://hydraraptor.blogspot.com/2007/10/measuring-temperature-easy-way.html

The main use is for Arduino programs that read data from the circuit board described here:
http://make.rrrf.org/ts-1.0

Usage: python createTemperatureLookup.py [options]

Options:
-h, --help        show this help
--rp=...          pull-up resistor
--t0=ttt:rrr      low temperature temperature:resistance point (around 25C)
--t1=ttt:rrr      middle temperature temperature:resistance point (around 150C)
--t2=ttt:rrr      high temperature temperature:resistance point (around 250C)
--num-temps=...   the number of temperature points to calculate (default: 20)
"""
*/


//class Thermistor:
//"Class to do the thermistor maths"
//def __init__(self, rp, t1, r1, t2, r2, t3, r3):

/* 
rp = 4700;
t1 = 25;
r1 = 100000;
t2 = 150;
r2 = 1641.9;
t3 = 250;
r3 = 226.15;
num_temps = int(36);
*/
#define TT_RP 4700
#define TT_T1 25
#define TT_R1 100000
#define TT_T2 150
#define TT_R2 1641.9
#define TT_T3 250
#define TT_R3 226.15
#define TT_TABLENAME temptable_1

#if defined(TT_T1)
# define __t1 ((TT_T1)+273.15)
#else
# error "Unknown temperature T1"
#endif
#if defined(TT_T2)
#define __t2 ((TT_T2)+273.15)
#else
# error "Unknown temperature T2"
#endif
#if defined(TT_T3)
#define __t3 ((TT_T3)+273.15)
#else
# error "Unknown temperature T3"
#endif

#if defined(TT_R1)
# define __r1 (TT_R1)
#elif defined(TT_R1_ADC)
# define __r1 __temp_r(TT_R1_ADC)
#else
# error "Unknown resistance R1"
#endif
#if defined(TT_R2)
# define __r2 (TT_R2)
#elif defined(TT_R2_ADC)
# define __r2 __temp_r(TT_R2_ADC)
#else
# error "Unknown resistance R2"
#endif
#if defined(TT_R3)
# define __r3 (TT_R3)
#elif defined(TT_R3_ADC)
# define __r3 __temp_r(TT_R3_ADC)
#else
# error "Unknown resistance R3"
#endif

#if defined(TT_RP)
# define __rp (TT_RP)
#else
# error "Unknown pullup resistance RP"
#endif

#define __vadc 5.0
#define __vcc 5.0

#define __a1 log(__r1)
#define __a2 log(__r2)
#define __a3 log(__r3)
#define __z (__a1 - __a2)
#define __y (__a1 - __a3)
#define __x (1/__t1 - 1/__t2)
#define __w (1/__t1 - 1/__t3)
#define __v (pow(__a1,3) - pow(__a2,3))
#define __u (pow(__a1,3) - pow(__a3,3))
#define __c3 ((__x-__z*__w/__y)/(__v-__z*__u/__y))
#define __c2 ((__x-__c3*__v)/__z)
#define __c1 (1/__t1-__c3*pow(__a1,3)-__c2*__a1)

//def temp(self,adc):
//"Convert ADC reading into a temperature in Celcius"
// v = adc * self.vadc / (1024 * 16)   # convert the 10 bit ADC value to a voltage
#define __temp_v(adc) ((adc)*__vadc/(1024*16))
// r = self.rp * v / (self.vcc - v)    # resistance of thermistor
#define __temp_r(adc) (__rp*__temp_v(adc)/(__vcc-__temp_v(adc)))
//lnr = log(r)
//Tinv = self.c1 + (self.c2*lnr) + (self.c3*pow(lnr,3))
//return (1/Tinv) - 273.15        # temperature
#define __temp(adc) (1/(__c1 + (__c2*log(__temp_r(adc))) + (__c3*pow(log(__temp_r(adc)), 3))) - 273.15)

//def adc(self,temp):
//"Convert temperature into a ADC reading"
//y = (self.c1 - (1/(temp+273.15))) / (2*self.c3)
#define __adc_y(temp) ((__c1 - (1/(temp+273.15))) / (2*__c3))
//x = sqrt(pow(self.c2 / (3*self.c3),3) + pow(y,2))
#define __adc_x(temp) (sqrt(pow(__c2 / (3*__c3),3) + pow(__adc_y(temp),2)))
//r = exp(pow(x-y,1.0/3) - pow(x+y,1.0/3)) # resistance of thermistor
#define __adc_r(temp) (exp(pow(__adc_x(temp)-__adc_y(temp),1.0/3) - pow(__adc_x(temp)+__adc_y(temp),1.0/3)))
//return (r / (self.rp + r)) * (1024*16)
#define __adc(temp) ((__adc_r(temp) / (__rp + __adc_r(temp))) * (1024*16))

//def main(argv):

/*
rp = 4700;
t1 = 25;
r1 = 100000;
t2 = 150;
r2 = 1641.9;
t3 = 250;
r3 = 226.15;
num_temps = int(36);

try:
opts, args = getopt.getopt(argv, "h", ["help", "rp=", "t1=", "t2=", "t3=", "num-temps="])
except getopt.GetoptError:
usage()
sys.exit(2)

for opt, arg in opts:
if opt in ("-h", "--help"):
usage()
sys.exit()
elif opt == "--rp":
rp = int(arg)
elif opt == "--t1":
arg =  arg.split(':')
t1 = float( arg[0])
r1 = float( arg[1])
elif opt == "--t2":
arg =  arg.split(':')
t2 = float( arg[0])
r2 = float( arg[1])
elif opt == "--t3":
arg =  arg.split(':')
t3 = float( arg[0])
r3 = float( arg[1])
elif opt == "--num-temps":
num_temps =  int(arg)
*/
/*
max_adc = (1024 * 16) - 1
min_temp = 0
max_temp = 350
increment = int(max_adc/(num_temps-1));

t = Thermistor(rp, t1, r1, t2, r2, t3, r3)
tmp = (min_temp - max_temp) / (num_temps-1)
print tmp
temps = range(max_temp, min_temp + tmp, tmp);

print "// Thermistor lookup table for Marlin"
print "// ./createTemperatureLookup.py --rp=%s --t1=%s:%s --t2=%s:%s --t3=%s:%s --num-temps=%s" % (rp, t1, r1, t2, r2, t3, r3, num_temps)
print "#define NUMTEMPS %s" % (len(temps))
print "short temptable[NUMTEMPS][2] = {"

counter = 0
for temp in temps:
counter = counter +1
if counter == len(temps):
            print "   {%s, %s}" % (int(t.adc(temp)), temp)
        else:
            print "   {%s, %s}," % (int(t.adc(temp)), temp)
    print "};"
    
def usage():
    print __doc__

if __name__ == "__main__":
    main(sys.argv[1:])
*/
#define __entry(t) {__adc(t), t}
  
const short TT_TABLENAME[][2] PROGMEM = {
// include another file to suppress warnings
#include "thermistor_template2.h"
};

// clean-up defined macros

#undef TT_RP
#undef TT_T1
#undef TT_T2
#undef TT_T3
#if defined(TT_R1)
# undef TT_R1
#endif
#if defined(TT_R2)
# undef TT_R2
#endif
#if defined(TT_R3)
# undef TT_R3
#endif
#if defined(TT_R1_ADC)
# undef TT_R1_ADC
#endif
#if defined(TT_R2_ADC)
# undef TT_R2_ADC
#endif
#if defined(TT_R3_ADC)
# undef TT_R3_ADC
#endif
#undef TT_TABLENAME

#undef __r1
#undef __r2
#undef __r3
#undef __rp
#undef __vadc
#undef __vcc
#undef __a1
#undef __a2
#undef __a3
#undef __z
#undef __y
#undef __x
#undef __w
#undef __v
#undef __u
#undef __c3
#undef __c2
#undef __c1
#undef __temp_v
#undef __temp_r
#undef __temp
#undef __adc_y
#undef __adc_x
#undef __adc_r
#undef __adc
#undef __entry