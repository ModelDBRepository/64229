NEURON { SUFFIX cah }
NEURON {  USEION ca READ cao,cai WRITE ica }
ASSIGNED { ica }
PARAMETER {
        erev
	gmax 		= 0.001   (umho)
        vrest           = 0    (mV)

	mvalence 	= 6
	mgamma 		= 0.7
	mbaserate 	= 1
	mvhalf 		= -15.
	mbasetau 	= 0.1
	mtemp 		= 37
        mq10            =  3
	mexp 		= 1

	hvalence 	= 0
	hgamma		= 0
	hbaserate 	= 0
	hvhalf 		= 0
	hbasetau 	= 0
	htemp 		= 0
        hq10            = 3
	hexp 		= 0

	cao             = 2      (mM)
	cai             = 5e-5   (mM)

	celsius		= 37	 (degC)
	dt 				   (ms)
	v 			       (mV)

}

INCLUDE "bg_cvode.inc"
PROCEDURE iassign() { i = g * ghkca(v) ica=i }

:** kdr
