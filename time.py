from os import system, access, remove, W_OK
import numpy as np
import scipy.stats as st
import math
import argparse
import time
def chooseAppropriateTimeUnit(timeInMicros):
	if (timeInMicros / 1000000 > 1):
		return "s"
	elif (timeInMicros/ 1000 > 1):
		return "ms"
	else:
		return "us"
def adjustToTimeUnit(timeInMicros, timeunit):
	if timeunit == "s":
		return timeInMicros / 1000000
	elif timeunit == "ms":
		return timeInMicros / 1000
	else:
		return timeInMicros
def formatProperly(mean, r, timeunit):
	mean = adjustToTimeUnit(mean, timeunit)
	r = adjustToTimeUnit(r, timeunit)
	precision = -round(math.log10(r)) + 1
	if precision < 0:
		roundedMean = int(round(mean, precision))
		roundedRadius = int(round(r, precision))
		return "{:d} +- {:d} {}".format(roundedMean, roundedRadius, timeunit)
	else:
		return "{:.{prec}f} +- {:.{prec}f} {}".format(mean, r, timeunit, prec=precision)
def findMeanEstimate(vals, p=0.95):
	N = len(vals)
	mean = np.mean(vals)
	tinv = st.t.ppf((1 + p)/2, N - 1)
	r = tinv*st.sem(vals)/math.sqrt(N)
	return (mean, r)
if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Statistics")
	parser.add_argument("executable")
	parser.add_argument("--runsCount", type=int, required=True)
	parser.add_argument("--cleanup", action="store_true")
	args = vars(parser.parse_args())

	runsCount = args["runsCount"]
	executable = args["executable"]
	print("Performing %d runs of \"%s\"" % (runsCount, executable))
	outerTimesList = list()
	for i in range(runsCount):
		begin = time.perf_counter()
		system("%s > NUL" % (executable))
		end = time.perf_counter()
		outerTimesList.append((end - begin) * 1000000)
	(mean, r)=findMeanEstimate(outerTimesList)
	print(" Outer elapased mean time = %s (sigma = %.0f us)" % (\
		formatProperly(mean, r, "s"),\
		st.sem(outerTimesList)))
