: $Id: boltz_cvode.inc,v 1.1.1.1 2005/12/15 15:16:39 hines Exp $
TITLE Boltzmann eqn definition channel

COMMENT
Each channel has activation and inactivation particles as in the original
Hodgkin Huxley formulation.  The activation particle mm and inactivation
particle hh go from on to off states according to kinetic variables alpha
and beta which are voltage dependent.  The form of the alpha and beta
functions were dissimilar in the HH study.  The formulae are:
	Inf = 1./(1.+exp((v+vhalf)/kconst))
Tau must be set separately by the function tauset()

	chanexp : number for exponentiating the state variable; e.g.
		  original HH Na channel use m^3, note that chanexp = 0
		  will turn off this state variable
	erev : reversal potential for the channel
	celsius : temperature at which experiment was done (Tau will
		      will be adjusted using a q10 of 3.0)
	vhalf : (a voltage) determines the voltage at which the value
		 of the sigmoid function for Inf is 1/2
	vrest : voltage shift for vhalf
	kconst: the Boltzmann K : determines steepness of the sigmoid
ENDCOMMENT

INDEPENDENT {t FROM 0 TO 1 WITH 1 (ms)}

NEURON {
	RANGE gmax, g, i
	GLOBAL erev, Inf, Tau, vrest
} : end NEURON

CONSTANT {
	  FARADAY = 96489.0	: Faraday's constant
	  R= 8.31441		: Gas constant

} : end CONSTANT

UNITS {
	(mA) = (milliamp)
	(mV) = (millivolt)
	(umho) = (micromho)
} : end UNITS

COMMENT
** parameter values should come from files specific to particular channels
ASSIGNED { ina ena } : eg for Na
PARAMETER {
  erev 		= 0    (mV)
  gmax 		= 0    (S/cm2)
  vrest           = 0    (mV)

  mvhalf 		= 0
  mkconst 	= 0
  exptemp 	= 0
  mq10		= 3
  mexp 		= 0

  hvhalf 		= 0
  hkconst 	= 0
  hq10		= 3
  hexp 		= 0
} : end PARAMETER
ENDCOMMENT

PARAMETER {
  cao                (mM)
  cai                (mM)
  celsius			   (degC)
  dt 				   (ms)
  v 			       (mV)
}

ASSIGNED {
  i (mA/cm^2)		
  g (mho/cm^2)
  Inf[2]		: 0 = m and 1 = h
  Tau[2]		: 0 = m and 1 = h
  mexp_val
  hexp_val
} : end ASSIGNED 

STATE { m h }

INITIAL { 
 	mh(v)
        m = Inf[0]  h = Inf[1]
}

BREAKPOINT {

	LOCAL hexp_val, index, mexp_val

	SOLVE states  METHOD cnexp

	hexp_val = 1
	mexp_val = 1

	: Determining h's exponent value
	if (hexp > 0) {
		FROM index=1 TO hexp {
			hexp_val = h * hexp_val
		}
	}

	: Determining m's exponent value
	if (mexp > 0) {
		FROM index = 1 TO mexp {
			mexp_val = m * mexp_val
		}
	}

	:			       mexp			    hexp
	: Note that mexp_val is now = m      and hexp_val is now = h 
	g = gmax * mexp_val * hexp_val

	iassign()
} : end BREAKPOINT

: ASSIGNMENT PROCEDURES
: Must be overwritten by user routines in parameters.multi
: PROCEDURE iassign () { i = g*(v-erev) ina=i }
: PROCEDURE iassign () { i = g*ghkca(v) ica=i }

:-------------------------------------------------------------------
: I suppose we have 2 choices, to use the DERIVATIVE function or
: to explicitly state m+ and h+.  If you were to use the DERIVATIVE
: function, then you will do as follows:
: DERIVATIVE deriv {
:	m' = (-m + minf) / mtau
:	h' = (-h + hinf) / htau
: }
DERIVATIVE states {
	mh(v)
	m' = (-m + Inf[0]) / Tau[0] 
	h' = (-h + Inf[1]) / Tau[1]
 }

:-------------------------------------------------------------------
: NOTE : 0 = m and 1 = h
PROCEDURE mh (v) {
	LOCAL a, b, j, mqq10, hqq10, mv0, hv0

        mv0 = mvhalf + vrest
        hv0 = hvhalf + vrest

	mqq10 = mq10^((celsius-exptemp)/10.)	

	: Calculater Inf and Tau values for h and m

	Inf[0]=1./(1.+exp((v+mv0)/mkconst))	

	if (hexp == 0) { Inf[1] = 1  Tau[1]=1 } 
	else {		
	  Inf[1]=1./(1.+exp((v+hv0)/hkconst)) 
	  hqq10 = hq10^((celsius-exptemp)/10.)	
  	  Tau[1]=settau(1,v)/hqq10
	}

	Tau[0]=settau(0,v)/mqq10

} : end PROCEDURE mh (v)

:-------------------------------------------------------------------
FUNCTION FRT(temperature) {
	FRT = FARADAY * 0.001 / R / (temperature + 273.15)
} : end FUNCTION FRT (temperature)

:-------------------------------------------------------------------
 FUNCTION ghkca (v) { : Goldman-Hodgkin-Katz eqn
       LOCAL nu, efun

       nu = v*2*FRT(celsius)
       if(fabs(nu) < 1.e-6) {
               efun = 1.- nu/2.
       } else {
               efun = nu/(exp(nu)-1.) }

       ghkca = -FARADAY*2.e-3*efun*(cao - cai*exp(nu))
 } : end FUNCTION ghkca()

:________________________________________________________________
FUNCTION interp (v,ystart,yend,vmin,vmax) {
    interp = ystart + ((v-vmin)/(vmax-vmin))*(yend-ystart)
} : end FUNCTION interp()

COMMENT
: j==0 m; j==1 h
FUNCTION settau(j,v) {
  
  if (j==0) {
    if (v <= -110) {
	settau = 15 }
    else if (v < -40) {
      settau = interp(v,15., 80., -110.,-40.) }
    else if (v < 40) {
      settau = interp(v,80., 20, -40.,40.) }
    else { settau = 20 }
  } else {
    if (v <= -110) {
	settau = 200 }
    else if (v < -75) {
      settau = interp(v,200., 1000., -110.,-75.) }
    else { settau = 1000 }
 }
} : end FUNCTION settau (j)
ENDCOMMENT
