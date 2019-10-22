#include "stdafx.h"
#include <fstream>
#include "Globals.h"
#include "Blam/BlamLibrary.h"
#include "H2MOD\Modules\Startup\Startup.h"
#include "H2MOD\Modules\Tweaks\Tweaks.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\CustomMenu\Credits.h"
#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD\Modules\Networking\NetworkStats\NetworkStats.h"
#include "H2MOD\Modules\Networking\CustomPackets\CustomPackets.h"
#include "H2MOD\Modules\Networking\NetworkSession\NetworkSession.h"
#include "H2MOD\Modules\ServerConsole\ServerConsole.h"
#include "H2MOD\Variants\GunGame\GunGame.h"

#include "Util\ClipboardAPI.h"

std::wstring ERROR_OPENING_CLIPBOARD(L"Error opening clipboard");

ConsoleCommands::ConsoleCommands() {
	command = "";
	caretPos = 0;
}

void ConsoleCommands::writePreviousCommand(std::string& msg) {
	this->prevCommands.insert(this->prevCommands.begin(), msg);
	if (this->prevCommands.size() > 6) 
		this->prevCommands.pop_back();
}

void ConsoleCommands::writePreviousOutput(std::string& msg) {
	this->prevOutput.insert(this->prevOutput.begin(), msg);
	if (this->prevOutput.size() > 18) 
		this->prevOutput.pop_back();
}

time_t start = time(0);
BOOL ConsoleCommands::handleInput(WPARAM wp) {
	if (H2hWnd != GetForegroundWindow()) {
		//halo2 is not in focus
		return false;
	}
	double seconds_since_start = difftime(time(0), start);
	if (wp == H2Config_hotkeyIdConsole) {
		if (seconds_since_start > 0.5) {
			this->console = !this->console;
			*h2mod->GetAddress<bool*>(0x479F51) = !this->console;
			start = time(0);
		}
		return true;
	}
	switch (wp) {
		case '\b':   // backspace
		{
			if (this->console) {
				if (this->caretPos > 0)
				{
					this->command.erase(this->caretPos - 1, 1);
					this->caretPos -= 1;
				}
				return true;
			}
		}
		break;

		case '\r':    // return/enter
		{
			if (console) {
				writePreviousOutput(this->command);
				writePreviousCommand(this->command);
				std::string fullCommand("$");
				fullCommand += this->command;
				this->handle_command(fullCommand);

				command = "";
				caretPos = 0;
				previous_command_index = 0;
				return true;
			}
		}
		break;

		// copy/paste functionality
		case 0x56:
		{
			//read 'V' before 'CTRL'
			if (console && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {

				std::string clipboardContent;
				if (!ClipboardAPI::read(clipboardContent)) {
					output(ERROR_OPENING_CLIPBOARD);
					return false;
				}
				this->command += clipboardContent;
				this->caretPos = this->command.length();
				break;
			}
		}

	default:

		if (console) {

			if (wp == VK_UP)
			{
				if (prevCommands.size() > 0 && previous_command_index < prevCommands.size())
				{
					command = prevCommands[previous_command_index];
					caretPos = command.length();
					previous_command_index++;
				}
				return true;
			}

			if (wp == VK_DOWN)
			{
				if (prevCommands.size() > 0 && previous_command_index > 0)
				{
					previous_command_index--;
					command = prevCommands[previous_command_index];
					caretPos = command.length();
				}
				return true;
			}

			if (wp == VK_END)
			{
				caretPos = command.length();
				return true;
			}

			if (wp == VK_HOME)
			{
				caretPos = 0;
				return true;
			}

			if (wp == VK_LEFT)
			{
				if (caretPos)
					caretPos--;

				return true;
			}

			if (wp == VK_RIGHT)
			{
				if (caretPos != command.length())
					caretPos++;

				return true;
			}


			if (((wp >= 0x30 && wp <= 0x5A) || wp == 0x20 || wp == VK_OEM_MINUS || wp == VK_OEM_COMMA || wp == VK_OEM_PERIOD || wp == VK_OEM_MINUS || wp == VK_OEM_PLUS ||
				wp == VK_OEM_1 || wp == VK_OEM_2 || wp == VK_OEM_3 || wp == VK_OEM_4 || wp == VK_OEM_5 || wp == VK_OEM_6 || wp == VK_OEM_7)) {

				switch (wp)
				{
				case VK_OEM_COMMA:
					wp = ',';
					break;

				case VK_OEM_PERIOD:
					wp = '.';
					break;

				case VK_OEM_MINUS:
					wp = '-';
					break;

				case VK_OEM_1:
					wp = ';';
					break;

				case VK_OEM_2:
					wp = '/';
					break;

				case VK_OEM_4:
					wp = '[';
					break;

				case VK_OEM_5:
					wp = '\\';
					break;

				case VK_OEM_6:
					wp = ']';
					break;

				case VK_OEM_7:
					wp = '\'';
					break;

				case VK_OEM_PLUS:
					wp = '=';
					break;
				}



				if (GetAsyncKeyState(0x10) & 0x8000 || GetAsyncKeyState(0xA0) & 0x8000 || GetAsyncKeyState(0xA1) & 0x8000) {

					switch (wp)
					{
					case '1':
						wp = '!';
						break;

					case '-':
						wp = '_';
						break;

					case '2':
						wp = '@';
						break;

					case '3':
						wp = '#';
						break;

					case '4':
						wp = '$';
						break;

					case '5':
						wp = '%';
						break;

					case '6':
						wp = '^';
						break;

					case '7':
						wp = '&';
						break;

					case '8':
						wp = '*';
						break;

					case '9':
						wp = '(';
						break;

					case '0':
						wp = ')';
						break;

					case '=':
						wp = '+';
						break;

					case '[':
						wp = '{';
						break;

					case ']':
						wp = '}';
						break;

					case '\'':
						wp = '"';
						break;

					case ';':
						wp = ':';
						break;

					case ',':
						wp = '<';
						break;

					case '.':
						wp = '>';
						break;

					case '/':
						wp = '?';
						break;

					case '\\':
						wp = '|';
						break;



					default:
						wp = toupper(wp);
						break;
					}

				}
				else {
					wp = tolower(wp);
				}
				command.insert(caretPos, 1, (char)wp);
				caretPos += 1;
				return true;
			}
		}
		break;
	}
	return false;
}

void ConsoleCommands::checkForIds() {
	if (!checked_for_ids) {
		//only check once per game
		checked_for_ids = true;
		std::ifstream infile("commands.txt");
		if (infile.good()) {
			std::string line;
			while (std::getline(infile, line)) {
				std::vector<std::string> command = split(line, ',');
				if (command.size() != 2) {
					LOG_TRACE_GAME("Found line not comma separated right, {}", line);
				}
				else {
					object_ids[command[0]] = strtoul(command[1].c_str(), NULL, 0);
				}
			}
		}
	}
}

void ConsoleCommands::spawn(DatumIndex object_datum, int count, float x, float y, float z, float randomMultiplier) {

	for (int i = 0; i < count; i++) {
		try {
			ObjectPlacementData nObject;

			if (!object_datum.IsNull()) {
				DatumIndex player_datum = h2mod->get_unit_datum_from_player_index(0);
				call_object_placement_data_new(&nObject, object_datum, player_datum, 0);
				Real::Point3D* player_position = h2mod->get_player_coords(0);
				
				if (player_position != nullptr) {
					nObject.Placement.X = player_position->X * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
					nObject.Placement.Y = player_position->Y * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
					nObject.Placement.Z = (player_position->Z + 5.0f) * static_cast <float> (rand()) / static_cast<float>(RAND_MAX);
				}
				
				LOG_TRACE_GAME("object_datum = {0:#x}, x={1:f}, y={2:f}, z={3:f}", object_datum.ToInt(), nObject.Placement.X, nObject.Placement.Y, nObject.Placement.Z);
				unsigned int object_gamestate_datum = call_object_new(&nObject);
				call_add_object_to_sync(object_gamestate_datum);
			}
		}
		catch (...) {
			LOG_TRACE_GAME("Error running spawn command");
		}
	}
}

void ConsoleCommands::output(std::wstring result) {
	if (h2mod->Server) {
		result = result + L"\n";
		ServerConsole::logToDedicatedServerConsole(result.c_str());
	}
	else {
		std::string str(result.begin(), result.end());
		writePreviousOutput(str);
	}
}

void ConsoleCommands::display(std::string output)
{
	writePreviousOutput(output);
}

bool ConsoleCommands::isNum(const char *s) {
	int i = 0;
	while (s[i]) {
		//if there is a letter in a string then string is not a number
		if (isalpha(s[i])) {
			return false;
		}
		i++;
	}
	return true;
}

int __cdecl call_get_object_via_datum(DatumIndex object_datum_index, int object_type)
{
	//LOG_TRACE_GAME("call_get_object( object_datum_index: %08X, object_type: %08X )", object_datum_index, object_type);

	typedef int(__cdecl *get_object)(DatumIndex object_datum_index, int object_type);
	get_object pget_object = (get_object)(h2mod->GetAddress(0x1304E3, 0x11F3A6));

	return pget_object(object_datum_index, object_type);
}

int __cdecl call_entity_datum_to_gamestate_datum(int entity_datum)
{
	typedef int(__cdecl *entity_datum_to_gamestate_datum)(int entity_datum);
	entity_datum_to_gamestate_datum pentity_datum_to_gamestate_datum = (entity_datum_to_gamestate_datum)h2mod->GetAddress(0x1F2211);

	return pentity_datum_to_gamestate_datum(entity_datum);
}


/*
* Handles the given string command
* Returns a bool indicating whether the command is a valid command or not
*/
void ConsoleCommands::handle_command(std::string command) {
	//split by a space
	std::vector<std::string> splitCommands = split(command, ' ');

	if (splitCommands.size() != 0) {
		std::string firstCommand = splitCommands[0];
		std::transform(firstCommand.begin(), firstCommand.end(), firstCommand.begin(), ::tolower);
		if (firstCommand == "$reloadmaps") {
			if (splitCommands.size() != 1) {
				output(L"Invalid command, usage - $reloadMaps");
				return;
			}
			mapManager->reloadMaps();
		}
		else if (firstCommand == "$help") {
			output(L"reloadMaps");
			output(L"kick");
			output(L"logPlayers");
			output(L"maxPlayers");
			output(L"resetSpawnCommandList");
			output(L"isHost");
			output(L"downloadMap");
			output(L"spawn");
			output(L"controller_sens");
			output(L"mouse_sens");
		}
		else if (firstCommand == "$mapfilename")
		{
			std::wstring map_file_name;
			mapManager->getMapFilename(map_file_name);
			output(map_file_name);
		}
		else if (firstCommand == "$downloadmap") {
			if (splitCommands.size() != 2) {
				output(L"Invalid download map command, usage - $downloadMap MAP_NAME");
				return;
			}
			std::string firstArg = splitCommands[1];
			mapManager->downloadFromRepo(firstArg);
		}
		else if (firstCommand == "$kick") {
			if (h2mod->Server) {
				output(L"Don't use this on dedis");
				return;
			}
			if (splitCommands.size() != 2) {
				output(L"Invalid kick command, usage - $kick PEER_INDEX");
				return;
			}
			if (!NetworkSession::localPeerIsSessionHost()) {
				output(L"Only the server can kick players");
				return;
			}

			std::string firstArg = splitCommands[1];

			if (isNum(firstArg.c_str())) {
				int peerIndex = atoi(firstArg.c_str());
				if (peerIndex == NetworkSession::getCurrentNetworkSession()->session_host_peer_index) {
					output(L"Don't kick yourself");
					return;
				}
				if (peerIndex >= NetworkSession::getCurrentNetworkSession()->membership.total_peers) {
					output(L"Peer at the specified index doesn't exist");
					return;
				}
				NetworkSession::kickPeer(peerIndex);
			}
		}
		else if (firstCommand == "$logplayers") {
			if (!NetworkSession::localPeerIsSessionHost()) {
				output(L"Only host can log out information about players");
				return;
			}
			networkPlayers->logAllPlayersToConsole();
		}
		else if (firstCommand == "$logpeers")
		{
			if (!NetworkSession::localPeerIsSessionHost()) {
				output(L"Only host can log out information about players");
				return;
			}
			networkPlayers->logAllPeersToConsole();
		}
		else if (firstCommand == "$maxplayers") {
			if (splitCommands.size() != 2) {
				output(L"Usage: $maxplayers value (betwen 1 and 16).");
				return;
			}
			if (!NetworkSession::localPeerIsSessionHost()) {
				output(L"Can be only used while hosting.");
				return;
			}

			std::string secondArg = splitCommands[1];
			network_session* session = NetworkSession::getCurrentNetworkSession();

			if (isNum(secondArg.c_str())) {

				int maxPlayersToSet = atoi(secondArg.c_str());
				if (maxPlayersToSet < 1 || maxPlayersToSet > 16) {
					output(L"The value needs to be between 1 and 16.");
					return;
				}
				if (maxPlayersToSet < session->membership.total_players) {
					output(L"You can't set a value of max players smaller than the actual number of players on the server.");
					return;
				}
				else {
					session->parameters.max_party_players = maxPlayersToSet;
					output(L"Maximum players set.");
					return;
				}
			}
		}
		else if (firstCommand == "$resetspawncommandlist") {
			//reset checked_for_ids, so you can reload new object_datums at runtime
			this->checked_for_ids = false;
		}
		else if (firstCommand == "$spawnnear") {
			if (splitCommands.size() < 3 || splitCommands.size() > 4) {
				output(L"Invalid command, usage $spawn command_name count");
				return;
			}

			if (!NetworkSession::localPeerIsSessionHost())
			{
				output(L"Can only be used by the session host!");
				return;
			}

			if (isLobby) {
				//TODO: need a nicer way to detect this for dedis
				output(L"Can only be used ingame");
				return;
			}

			//lookup a commands.txt file that contain string->object_datums
			checkForIds();

			std::string secondArg = splitCommands[1];
			std::string thirdArg = splitCommands[2];
			DatumIndex object_datum;
			if (object_ids.find(secondArg) == object_ids.end()) {
				//read from chatbox line
				std::string secondArg = splitCommands[1];
				object_datum = strtoul(secondArg.c_str(), NULL, 0);
			}
			else {
				//read from object_id map
				object_datum = object_ids[secondArg];
			}

			float randomMultiplier = 1.0f;
			if (splitCommands.size() == 4) {
				//optional multiplier provided
				std::string fourthArg = splitCommands[3];
				randomMultiplier = stof(fourthArg);
			}

			int count = 1;
			try {
				count = stoi(thirdArg);
			}
			catch (...) {
				LOG_TRACE_GAME("Error converting string to int");
			}
			
			Real::Point3D* playerPosition = h2mod->get_player_coords(0);
			this->spawn(object_datum, count, playerPosition->X + 0.5f, playerPosition->Y + 0.5f, playerPosition->Z + 0.5f, randomMultiplier);
		}
		else if (firstCommand == "$ishost") {
			network_session* session = NetworkSession::getCurrentNetworkSession();
			std::wstring isHostStr = L"isHost=";
			DWORD isHostByteValue = session->local_session_state;
			std::wostringstream ws;
			ws << isHostByteValue;
			const std::wstring s(ws.str());
			isHostStr += (NetworkSession::localPeerIsSessionHost() ? L"yes" : L"no");
			isHostStr += L", value=";
			isHostStr += s;
			output(isHostStr);
		}
		else if (firstCommand == "$leavegame") {
			h2mod->exit_game();
		}
		else if (firstCommand == "$xyz") {
			if (!NetworkSession::localPeerIsSessionHost()) {
				output(L"Only host can see xyz for now...");
				return;
			}
			displayXyz = !displayXyz;
		}
		else if (firstCommand == "$downloadmap") {
			if (splitCommands.size() != 2 && !splitCommands[1].empty()) {
				output(L"Invalid command, usage downloadMap filename");
				return;
			}
			std::string secondArg = splitCommands[1];
			secondArg += ".map";
			std::thread(&MapManager::downloadFromRepo, mapManager, secondArg).detach();
		}
		else if (firstCommand == "$spawn") {
			if (splitCommands.size() != 6) {
				output(L"Invalid command, usage $spawn command_name count x y z");
				return;
			}

			if (!NetworkSession::localPeerIsSessionHost()) {
				output(L"Can only be used by the session host!");
				return;
			}

			if (isLobby) {
				//TODO: need a nicer way to detect this for dedis
				output(L"Can only be used ingame");
				return;
			}

			//lookup a commands.txt file that contain string->object_datums
			checkForIds();

			std::string secondArg = splitCommands[1];
			std::string thirdArg = splitCommands[2];
			unsigned int object_datum;
			if (object_ids.find(secondArg) == object_ids.end()) {
				//read from chatbox line
				std::string secondArg = splitCommands[1];
				object_datum = strtoul(secondArg.c_str(), NULL, 0);
			}
			else {
				//read from object_id map
				object_datum = object_ids[secondArg];
			}

			int count = stoi(thirdArg);
			float x = stof(splitCommands[3]);
			float y = stof(splitCommands[4]);
			float z = stof(splitCommands[5]);

			this->spawn(object_datum, count, x, y, z, 1.0f);
		}
		else if (firstCommand == "$controller_sens") {
			if (splitCommands.size() != 2) {
				output(L"Invalid usage, usage $controller_sens value");
				return;
			}
			std::string sensVal = splitCommands[1];

			if (isNum(sensVal.c_str())) {
				H2Tweaks::setSens(CONTROLLER, stoi(sensVal));
				H2Config_controller_sens = stoi(sensVal);
			}
			else {
				output(L"Wrong input! Use a number.");
			}

		}
		else if (firstCommand == "$mouse_sens") {
			if (splitCommands.size() != 2) {
				output(L"Invalid usage, usage $mouse_sens value");
				return;
			}
			std::string sensVal = splitCommands[1];

			if (isNum(sensVal.c_str())) {
				H2Tweaks::setSens(MOUSE, stoi(sensVal));
				H2Config_mouse_sens = stoi(sensVal);
			}
			else {
				output(L"Wrong input! Use a number.");
			}
		}
		else if (firstCommand == "$netstats") {
			NetworkStatistics = NetworkStatistics == false ? true : false;
		}
		else if (firstCommand == "$displayinfos") {
			std::string str_index = splitCommands[1];
			int inc = 0;
			int index = stoi(str_index);
			network_session* netsession = NetworkSession::getCurrentNetworkSession();
		
			std::wstring str_to_print;
			std::wstring space = L" ";
			std::wstring xuid = L"XUID: ";
			str_to_print += netsession->parameters.scenario_path + space + netsession->membership.peer_info[index].peer_name + space + netsession->membership.peer_info[index].peer_name_2;
			str_to_print += xuid + std::to_wstring(netsession->membership.player_info[index].identifier);
			str_to_print += L"\n Netowork Session state: " + std::to_wstring(netsession->local_session_state);
			output(str_to_print);
				
		}
		else if (firstCommand == "$requestfilename") {
			CustomPackets::sendRequestMapFilename(NetworkSession::getCurrentNetworkSession());
		}
		else {
			output(L"Unknown command.");
		}
	}
}