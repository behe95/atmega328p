import os
from enum import Enum

from pathlib import Path
import serial
import threading

PROJECT_DIR = Path.cwd()

class STATE(Enum):
#	SHOW_MENU=1
	SHOW_HELP=2
	CMD_MODE=3
	EXIT=4
	CLR_SCRN=5
	CMD_PWD=6
	CMD_LS=7
	CMD_CD=8
	CMD_SET_BAUD=9
	CMD_SET_SERIAL_LINE=10
	CMD_SEND_DATA=11
	CMD_RECEIVE_DATA=12
	CMD_SHOW_UART_CONFIG=13

def main():


	CURRENT_STATE = STATE.SHOW_HELP

	is_running = True




	while is_running:
#		if CURRENT_STATE == STATE.SHOW_MENU:
#			CURRENT_STATE = display_menu()
		if CURRENT_STATE == STATE.SHOW_HELP:
			CURRENT_STATE = display_help()
		elif CURRENT_STATE == STATE.CMD_MODE:
			CURRENT_STATE = cmd_mode()
		elif CURRENT_STATE == STATE.EXIT:
			is_running = False
		elif CURRENT_STATE == STATE.CLR_SCRN:
			clear_scr()
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_PWD:
			cmd_pwd()
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_LS:
			cmd_ls()
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_CD:
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_SET_BAUD:
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_SET_SERIAL_LINE:
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_SEND_DATA:
			CURRENT_STATE = STATE.CMD_MODE
		elif CURRENT_STATE == STATE.CMD_RECEIVE_DATA:
			CURRENT_STATE = STATE.CMD_MODE

		elif CURRENT_STATE == STATE.CMD_SHOW_UART_CONFIG:
			CURRENT_STATE = STATE.CMD_MODE

def clear_scr():
	os.system("cls" if os.name == "nt" else "clear")


"""
def display_menu():
	while True:
		clear_scr()

		print("\n|================ UART FILE SHELL ======================|")
		print(  "| Select a number from 1 to 5 or type / to enter        |")
		print(  "| command mode.                                         |")
		print(  "|                                                       |")
		print(  "| 1. Navigate Files                                     |")
		print(  "| 2. Send Files                                         |")
		print(  "| 3. Receive Files                                      |")
		print(  "| 4. UART Config                                        |")
		print(  "| 5. Show available commands                            |")
		print(  "|_______________________________________________________|")


		is_valid = False
		
		while not is_valid:
			is_valid = True
			selection = input(":")
			if selection == "/":
				return STATE.CMD_MODE
			elif selection == "5":
				return STATE.SHOW_HELP
			else:
				print("Invlid input")
				is_valid = False

"""

def display_help():

#	while True:
#		clear_scr()
	print("\n|============================== Available Commands ===============================|")
	print(  "|    Command                          Description                                 |")
	print(  "|    -------                          -----------                                 |")
	print(  "|    exit                             Exit shell                                  |")
#	print(  "|    /                                Enter command shell                         |")
#	print(  "|    menu                             Display menu                                |")
	print(  "|    help                             Display help                                |")
	print(  "|    clear                            Clear Screen                                |")
	print(  "|    pwd                              Show current directory                      |")
	print(  "|    ls                               List files in the current directory         |")
	print(  "|    cd <dir name>                    Change directory                            |")
	print(  "|    set_baud <BAUD rate>             Set UART BAUD rate                          |")
	print(  "|    set_serln <Serial Line>          Set UART Serial Line                        |")
	print(  "|    send_dt <filename>               Send data                                   |")
	print(  "|    rcv_dt <filename>                Receive data                                |")
	print(  "|    show_uart_config                 Show UART Config                            |")
	print(  "|_________________________________________________________________________________|")

	return STATE.CMD_MODE

"""
		is_valid = False

		while not is_valid:
			is_valid = True
			selection = input(":")
			if selection == "/":
				return STATE.CMD_MODE
			else:
				print("Invlid input")
				is_valid = False
"""

def cmd_mode():

	while True:
		selection = input(":").strip()
		if selection == "exit":
			return STATE.EXIT
#		elif selection == "menu":
#			return STATE.SHOW_MENU
		elif selection == "help":
			return STATE.SHOW_HELP
		elif selection == "clear":
			return STATE.CLR_SCRN
		elif selection == "pwd":
			return STATE.CMD_PWD
		elif selection == "ls":
			return STATE.CMD_LS
		elif selection.startswith("cd "):
			path = selection[3:].strip()
			cmd_cd(path)
			return STATE.CMD_CD
		elif selection.startswith("set_baud "):
					
			try:
				rate = int(selection[9:].strip())
				update_config("BAUD_RATE",rate)
			except Exception as ex:
				print(f"Error: {ex}")
			finally:
				return STATE.CMD_SET_BAUD
		elif selection.startswith("set_serln "):
			update_config("SERIAL_LINE", selection[10:].strip())
			return STATE.CMD_SET_SERIAL_LINE

		elif selection.startswith("send_dt "):
			send_data(selection[8:].strip())
			return STATE.CMD_SEND_DATA
		elif selection.startswith("rcv_dt "):
			receive_data(selection[7:].strip())
			return STATE.CMD_RECEIVE_DATA
		elif selection == "show_uart_config":
			try:
				config = get_uart_config()
				for i, (k, v) in enumerate(config.items()):
					print(f"{k}:{v}")
			except Exception as ex:
				print(f"Error: {ex}")
			return STATE.CMD_SHOW_UART_CONFIG
				

		else:
			print("Invalid command")


def cmd_pwd():
	print(Path.cwd())

def cmd_ls():
	for item in Path.cwd().iterdir():
		print(item.name)

def cmd_cd(path):
	try:
		os.chdir(path)
		print(os.getcwd())
	except Exception as ex:
		print(f"Error: {ex}")

def update_config(key,val):
	config_path = Path(PROJECT_DIR) / "sys.config"
	new_line = f"{key}:{val}\n"
	found = False

	lines = []
	try:
		if config_path.exists():
			with config_path.open("r") as f:
				lines = f.readlines()
	except Exception as ex:
		lines = []
		print(f"Error: {ex}")
		return

	for idx, line in enumerate(lines):
		if line.startswith(f"{key}:"):
			lines[idx] = new_line
			found = True
			break

	if not found:
		lines.append(new_line)


	with config_path.open("w") as f:
		f.writelines(lines)
		print(f"{key} set successfully")
	


"""
	@return object{port, baud}
"""
def get_uart_config():
	config_path = Path(PROJECT_DIR) / "sys.config"
	lines = []

	if config_path.exists():
		with config_path.open("r") as f:
			lines = f.readlines()
	else:
		raise Exception("BAUD rate and Serial Line has not been setup!")
		return

	baud = None
	port = None

	found = 0

	for idx, line in enumerate(lines):
		if line.startswith("BAUD_RATE"):
			found += 1
			baud = line.split(":")[1].strip()
		elif line.startswith("SERIAL_LINE"):
			found += 1
			port = line.split(":")[1].strip()
		
		if found >= 2:
			break

	if found <= 1:
		raise Exception("BAUD rate and Serial Line has not been setup!")
		return

	return {"baud": baud, "port": port}


def send_data(filename):
	try:
		config = get_uart_config()
		baud = config["baud"]
		port = config["port"]

		ser = serial.Serial(port, baud, timeout=1)
		fsize = os.path.getsize(filename)
		sent = 0
		with open(filename, "rb") as f:
			while True:
				data = f.read(64)
				if not data:
					break
				ser.write(data)
				sent += len(data)
				print("Sent: ", sent, "/", fsize)

		ser.close()
		print("File sent successfully!")

	except Exception as ex:
		print(f"Error: {ex}")


def receive_data(filename):
	
	running = [True]
	def check_keyboard():
		try:
			input("")
			running[0] = False
		except Exception as ex:
			print(f"Keyboard Thread Error: {ex}")
	threading.Thread(target=check_keyboard, daemon=True).start()


	try:
		config = get_uart_config()
		baud = config["baud"]
		port = config["port"]

		ser = serial.Serial(port, baud, timeout=1)

		print("Receiving data....hit <Enter> to stop")

		with open(filename, "wb") as f:
			while running[0]:
				data = ser.read(64)
				if data:
					f.write(data)
					print("received: ", len(data), "bytes")


		ser.close()
		print("File received successfully!")

	except Exception as ex:
		print(f"Error: {ex}")


if __name__ == "__main__":
	main()




