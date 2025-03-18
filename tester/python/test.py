import argparse
import json
import subprocess
import time
import os
import glob
import select
import multiprocessing

from tqdm import tqdm

HOST = "127.0.0.1"
PORT = "6667"
TEST_DIR = "./cases"
LOG_DIR = "./log"
TIMEOUT_AFTER_LAST_RESPONSE = 5

class IRCTester:
	def __init__(self, host: str = HOST, port: str = PORT, test_dir: str = TEST_DIR, log_dir: str = LOG_DIR) -> None:
		self.host: str = host
		self.port: str = port
		self.log_dir: str = IRCTester.__setup_log_dir(log_dir)
		self.test_files: list[str] = IRCTester.__setup_test_files(test_dir)
		self.manager = multiprocessing.Manager()
		self.result_dict: multiprocessing.DictProxy[str, multiprocessing.DictProxy[int, str]] = self.manager.dict()
		self.lock = multiprocessing.Lock()

	@staticmethod
	def __setup_log_dir(log_dir: str) -> str:
		os.makedirs(log_dir, exist_ok=True)
		return log_dir

	@staticmethod
	def __setup_test_files(test_dir: str) -> list[str]:
		return glob.glob(os.path.join(test_dir, "**", "*.json"), recursive=True)

	def __subprocess_loop(self, child_stdout):
		last_response_time = time.time()
		output_lines = []
		while True:
			ready, _, _ = select.select([child_stdout], [], [], TIMEOUT_AFTER_LAST_RESPONSE)
			if ready:
				line: str = child_stdout.readline().strip()
				if line:
					output_lines.append(f"Response: {line}")
					last_response_time = time.time()
			else:
				if time.time() - last_response_time >= TIMEOUT_AFTER_LAST_RESPONSE:
					timeout_msg = f"{time.strftime('%Y-%m-%d %H:%M:%S')} timeout (no response for {TIMEOUT_AFTER_LAST_RESPONSE} sec)"
					output_lines.append(timeout_msg)
					break
		return output_lines

	def __run_subprocess(self, command_str: str) -> list[str]:
		""" subprocess を実行し、結果をリストで返す """
		output_log = ["=" * 67]
		process = subprocess.Popen(
			["nc", self.host, self.port],
			stdin=subprocess.PIPE,
			stdout=subprocess.PIPE,
			stderr=subprocess.PIPE,
			encoding="utf-8"
		)
		process.stdin.write(command_str)
		process.stdin.flush()
		output_log.extend(self.__subprocess_loop(process.stdout))
		process.terminate()
		output_log.append("=" * 67)
		return output_log

	def __exec_one_node(self, test_file: str, node: dict, index: int, progress_bar):
		""" 各 `node` を並列に実行 """
		log_output = [f"Executing: {node.get('title', 'Untitled')}"]
		command_str = "\r\n".join(node.get("cmds", [])) + "\r\n"
		log_output.extend(self.__run_subprocess(command_str))
		with self.lock:
			if self.result_dict.get(test_file) is None:
				self.result_dict[test_file] = self.manager.dict()
			self.result_dict[test_file][index] = log_output 
		progress_bar.update(1)

	def __exec_one_file(self, test_file: str):
		""" 各ファイルの `node` を並列処理 """
		with open(test_file, "r", encoding="utf-8") as file:
			content = file.read().strip()
			if not content:
				print(f"Warning: {test_file} is empty or invalid JSON.")
				return
			try:
				commands_data = json.loads(content)
			except json.JSONDecodeError as e:
				print(f"Error: Failed to parse JSON in {test_file}: {e}")
				return
		with tqdm(total=len(commands_data), desc=f"Processing {os.path.basename(test_file)}", position=1, leave=False) as progress_bar:
			processes = []
			for index, node in enumerate(commands_data):
				p = multiprocessing.Process(target=self.__exec_one_node, args=(test_file, node, index, progress_bar))
				p.daemon = False
				processes.append(p)
				p.start()
			for p in processes:
				p.join()

	def run(self) -> None:
		""" 全テストファイルの処理を実行 """
		with tqdm(total=len(self.test_files), desc="Overall Progress", position=0) as overall_progress:
			processes = []
			for test_file in self.test_files:
				p = multiprocessing.Process(target=self.__exec_one_file, args=(test_file,))
				p.daemon = False
				processes.append(p)
				p.start()
				overall_progress.update(1)

			for p in processes:
				p.join()

		for key, sub_dict in self.result_dict.items():
			key = os.path.basename(key).replace(".json", "")
			log_file = os.path.join(self.log_dir, f"out_{key}.log")
			with open(log_file, "a", encoding="utf-8") as log_fd:
				for index in sorted(sub_dict.keys()):
					log_fd.write("\n".join(sub_dict[index]) + "\n")

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
