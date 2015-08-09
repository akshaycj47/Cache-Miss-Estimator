#!/usr/bin/python -tt

""" Simple script to run the experiments of Cache Simulator """

import os
import sys
import subprocess

def run_experiments(traces_directory):
	"""
		Simple function that compiles the 'cachesim' program and generates
		output for all the traces in the traces directory
	"""
	# First compile the source code
	subprocess.call("make", shell = True)
	
	# Running experiments for each trace in the traces directory
	for trace in os.listdir(traces_directory):
		print "Generating output for trace: " + trace
		subprocess.call("./cachesim < traces/" + trace + " > results/" + trace + "_results.txt", shell = True)
	
def main():
	"""
		Main function which starts running the experiments
		The traces directory name is given as a command line argument
	"""
	
	if len(sys.argv) != 2:
		print "Correct usage: ./script.py [traces_directory_name]"
		sys.exit(1)
	
	traces_directory = sys.argv[1]
	run_experiments(traces_directory)
	
# Standard boiler plate to run the main() function
if __name__ == '__main__':
	main()