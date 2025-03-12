import argparse
import json
import subprocess
import time
import os
import glob
import time
import select

HOST = "127.0.0.1"
PORT = "6667"
TEST_DIR = "./cases"
LOG_DIR = "./log"
TIMEOUT_AFTER_LAST_RESPONSE = 5

class IRCTester:
	def __init__(self, host: str = HOST, port: str = PORT, test_dir: str = TEST_DIR, log_dir: str = LOG_DIR) -> None:
		self.host: str = host
		self.port: str = port
		self.log_dir: str = self.__setup_log_dir(log_dir)
		self.test_files : list[str] = self.__setup_test_files(test_dir)

	def __setup_log_dir(self, log_dir: str) -> str:
		os.makedirs(log_dir, exist_ok=True)
		return log_dir

	def __setup_test_files(self, test_dir: str) -> str:
		return glob.glob(os.path.join(test_dir, "**", "*.json"), recursive=True)

	def __subprocess_loop(self, child_stdout, log_fd):
		last_response_time = time.time()
		while True:
			ready, _, _ = select.select([child_stdout], [], [], TIMEOUT_AFTER_LAST_RESPONSE)
			if ready:
				line: str = child_stdout.readline().strip()
				if line:
					print(f"Response: {line}", file=log_fd)
					print(f"Response: {line}")
					last_response_time = time.time()
			else:
				if time.time() - last_response_time >= TIMEOUT_AFTER_LAST_RESPONSE:
					print(f"{time.strftime('%Y-%m-%d %H:%M:%S')} timeout (no response for {TIMEOUT_AFTER_LAST_RESPONSE} sec)", file=log_fd)
					print(f"{time.strftime('%Y-%m-%d %H:%M:%S')} timeout (no response for {TIMEOUT_AFTER_LAST_RESPONSE} sec)")
					break

	def __run_subprocess(self, log_fd, command_str: str):
		print("=" * 67 , file=log_fd)
		process = subprocess.Popen(
			["nc", self.host, self.port],
			stdin=subprocess.PIPE,
			stdout=subprocess.PIPE,
			stderr=subprocess.PIPE,
			text=True
		)
		try:
			process.stdin.write(command_str)
			process.stdin.flush()
			self.__subprocess_loop(process.stdout, log_fd)
		except Exception as e:
			print(f"Error: {str(e)}", file=log_fd)
			print(f"Error: {str(e)}")
		finally:
			process.kill()
			print("=" * 67 , file=log_fd)

	def __send_commands(self, commands, log_fd) -> None:
		command_str: str = "\r\n".join(commands) + "\r\n"
		self.__run_subprocess(log_fd, command_str)

	def __exec_one_file(self, log_fd, commands_data) -> None:
		for node in commands_data:
			print(f"Executing: {node.get('title', 'Untitled')}", file=log_fd)
			print(f"Executing: {node.get('title', 'Untitled')}")
			self.__send_commands(node.get("cmds", []), log_fd)
		
	def run(self) -> None:
		for test_file in self.test_files:
			log_file: str = os.path.join(self.log_dir, f"log_{os.path.basename(test_file).replace('.json', '')}.txt")
			with open(test_file, "r", encoding="utf-8") as file:
				commands_data = json.load(file)
			with open(log_file, "a", encoding="utf-8") as log_fd:
				self.__exec_one_file(log_fd, commands_data)

		

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=str)
    parser.add_argument("-c", "--host", type=str)
    parser.add_argument("-t", "--test_dir", type=str)
    parser.add_argument("-l", "--log_dir", type=str)
    args = parser.parse_args()

    host: str = args.host or HOST
    port: str = args.port or PORT
    test_dir: str = args.test_dir or TEST_DIR
    log_dir: str = args.log_dir or LOG_DIR
    tester = IRCTester(host=host, port=port, test_dir=test_dir, log_dir=log_dir)
    tester.run()

if __name__ == "__main__":
    main()
