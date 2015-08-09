#!/usr/bin/python -tt

""" Simple script to run the experiments of Cache Simulator """

import os
import sys
import subprocess
import argparse

import Tkinter as tk
import ttk

USER_INPUT_ARGS = [
	{
		"name": "traces_directory",
		"type": "Text",
		"help": "INPUT: Directory where traces are located",
		"default": "./traces",
		"choices": [],
		"action": "",
	},
	{
		"name": "results_directory",
		"type": "Text",
		"help": "OUTPUT: Directory where result is to be stored",
		"default": "./results",
		"choices": [],
		"action": "",
	},
	{
		"name": "traces",
		"type": "List",
		"help": "TRACES",
		"default": "ALL",
		"choices": ["ASTAR", "BZIP2", "MCF", "PERLBENCH", "4X_ASTAR", "4X_BZIP2", "4X_MCF", "4X_PERLBENCH", "MIX_AAPA", "MIX_ABMP", "ALL_4X", "ALL"],
		"action": "",
	},
	{
		"name": "cleanup",
		"type": "Bool",
		"help": "CLEAN: Clear all the old experiment results",
		"default": "False",
		"choices": [],
		"action": "store_true",
	},
]

def run_experiments(traces_directory, results_directory, traces_list):
	"""
		Simple function that compiles the 'cachesim' program and generates
		output for all the traces in the traces directory
	"""
	# First compile the source code
	subprocess.call("make", shell = True)
	
	# Running experiments for each trace in the traces directory
	for trace in traces_list:
		print "Generating output for trace: " + trace
		trace_location = os.path.join(traces_directory, trace)
		if not os.path.isfile(trace_location):
			print "Error: Input traces does not exist. Aborting."
			sys.exit(1)
		result_location = os.path.join(results_directory, trace)

		subprocess.call("./cachesim < " + trace_location + " > " + result_location + "_results.txt", shell = True)
	
def get_traces(traces):
	"""
		Function to determine which traces to use for experimentation
	"""
	traces_list = []

	if traces == "ALL":
		traces_list.extend(['astar.trace', 'bzip2.trace', 'mcf.trace', 'perlbench.trace', '4x_astar.trace', '4x_bzip2.trace', '4x_mcf.trace', '4x_perlbench.trace', 'mix_aapa.trace', 'mix_abmp.trace'])
	elif traces == "ASTAR":
		traces_list.append('astar.trace')
	elif traces == "BZIP2":
		traces_list.append('bzip2.trace')
	elif traces == "MCF":
		traces_list.append('mcf.trace')
	elif traces == "PERLBENCH":
		traces_list.append('perlbench.trace')
	elif traces == "4X_ASTAR":
		traces_list.append('4x_astar.trace')
	elif traces == "4X_BZIP2":
		traces_list.append('4x_bzip2.trace')
	elif traces == "4X_MCF":
		traces_list.append('4x_mcf.trace')
	elif traces == "4X_PERLBENCH":
		traces_list.append('4x_perlbench.trace')
	elif traces == "MIX_AAPA":
		traces_list.append('mix_aapa.trace')
	elif traces == "MIX_ABMP":
		traces_list.append('mix_abmp.trace')
	elif traces == "ALL_4X":
		traces_list.extend(['4x_astar.trace', '4x_bzip2.trace', '4x_mcf.trace', '4x_perlbench.trace'])

	return traces_list

def clean_results(results_directory):
	"""
		Clear the contents of the result directory
	"""
	for file in os.listdir(results_directory):
		file_to_remove = os.path.join(results_directory, file)
		print "Deleting " + file_to_remove
		os.remove(file_to_remove)

def get_gui_inputs(args):
	"""
		Display Tkinter GUI for better user experience
	"""
	bindings = {}
	avars = vars(args)

	# Build the GUI
	root = tk.Tk()
	root.wm_title("Run Experiments")
	s = ttk.Style()
	s.theme_use('classic')
	s.configure('.', font=("Consolas", 10))

	Row = 0
	Column = 0

	for entry in USER_INPUT_ARGS:
		type = entry['type']
		name = entry['name']

		if type == "Text":
			t = ttk.Label(root, text=entry['help'])
			t.grid(row=Row, column=Column, sticky=tk.W)

			var = tk.StringVar()
			var.set(avars[name])
			bindings[name] = var

			e = ttk.Entry(root, textvariable=var, width=40, font=("Consolas", 10))
			e.grid(row=Row, column=Column+1, sticky=tk.W)

		elif type == "List":
			t = ttk.Label(root, text=entry['help'])
			t.grid(row=Row, column=Column, sticky=tk.W)

			var = tk.StringVar()
			var.set(avars[name])
			bindings[name] = var

			e = tk.OptionMenu(root, var, *entry['choices'])
			e.grid(row=Row, column=Column+1, sticky=tk.W)

		elif type == "Bool":
			var = tk.BooleanVar()
			var.set(avars[name])
			bindings[name] = var

			e = ttk.Checkbutton(root, text=entry['help'], variable=var)
			e.grid(row=Row, column=Column, columnspan=2, sticky=tk.W)		


		Row = Row + 1

	x = ttk.Button(root, text="START EXPERIMENTS", command=root.destroy)
	x.grid(row=Row, column=Column, sticky=tk.W)

	# Display the GUI
	root.mainloop()

	# Process the user inputs
	for entry in USER_INPUT_ARGS:
		type = entry['type']
		name = entry['name']

		if type == "Bool":
			if bindings[name].get() == 0:
				avars[name] = False
			else:
				avars[name] = True
		else:
			avars[name] = bindings[name].get()

	return args

def main():
	"""
		Main function which starts running the experiments
		The traces directory name is given as a command line argument
	"""

	parser = argparse.ArgumentParser(description="Script to find the minimum Average Access Time (AAT) for a \
												  given trace. User has to choose between default and manual \
												  configuration. In default configuration, upper and lower \
												  bounds for C, B, S, V, K are chosen by default. In manual \
												  configuration, user has to enter the upper and lower bounds.")
	
	for entry in USER_INPUT_ARGS:
		if entry['type'] == "Text":
			parser.add_argument("--"+entry['name'], help=entry['help'], default=entry['default'])
		elif entry['type'] == "List":
			parser.add_argument("--"+entry['name'], help=entry['help'], default=entry['default'], choices=entry['choices'])		
		elif entry['type'] == "Bool":
			parser.add_argument("--"+entry['name'], help=entry['help'], action=entry['action'])

	args = parser.parse_args()
	args = get_gui_inputs(args)

	# Validating input directory
	if not os.path.isdir(args.traces_directory):
		print "Error: input directory does not exist. Aborting"
		sys.exit(1)

	# Validating output directory
	if not os.path.isdir(args.results_directory):
		print "Results directory does not exist, creating it."
		os.makedirs(args.results_directory)

	# Cleaning up all the previous results
	if args.cleanup:
		print "Cleaning up all the previous results"
		clean_results(args.results_directory)

	# Determining which traces to include in the experiments
	traces_list = get_traces(args.traces)
	
	run_experiments(args.traces_directory, args.results_directory, traces_list)
	
# Standard boiler plate to run the main() function
if __name__ == '__main__':
	main()