#pragma once

class ConsoleCommands {
public:
	ConsoleCommands();
	void handle_command(std::string);
	void spawn(DatumIndex object_datum, int count, float x, float y, float z, float randomMultiplier);
	void output(std::wstring result);
	void display(std::string output);
	std::vector<std::string> prevCommands;
	std::vector<std::string> prevOutput;
	BOOL handleInput(WPARAM wp);
	BOOL consoleOpen() { return this->console; };

	std::string command;
	int caretPos;
	bool handled;
	bool console = false;
	int previous_command_index = 0;
private:
	void writePreviousCommand(std::string& msg);
	void writePreviousOutput(std::string& msg);
	bool isNum(const char *s);
	void checkForIds();
	bool checked_for_ids = false;
	std::unordered_map<std::string, unsigned int> object_ids;
	DWORD sleepTime;
};
