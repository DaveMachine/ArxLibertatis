/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/Startup.h"

#include <stddef.h>
#include <string>
#include <vector>
#include <iostream>
#include <set>

#if ARX_COMPILER_MSVC
	#pragma warning(push)
	#pragma warning(disable:4512)
#endif
#include <boost/program_options.hpp>
#if ARX_COMPILER_MSVC
	#pragma warning(pop)
#endif

#include "core/Config.h"
#include "io/fs/FilePath.h"
#include "io/fs/Filesystem.h"
#include "io/log/Logger.h"
#include "platform/CrashHandler.h"
#include "platform/Environment.h"

#include "Configure.h"

namespace po = boost::program_options;

using std::string;

static fs::path findSubdirectory(const std::string & where, const fs::path & dir,
                                 fs::path * to_create = NULL) {
	
	string prefixes = expandEvironmentVariables(where);
	
	bool create_exists = false;
	
	size_t start = 0;
	while(true) {
		size_t end = prefixes.find(env_list_seperator, start);
		fs::path prefix = prefixes.substr(start, (end == string::npos) ? end : (end - start));
		
		fs::path subdir = prefix / dir;
		if(fs::is_directory(subdir)) {
			return subdir;
		}
		
		if(to_create) {
			if(to_create->empty() || (!create_exists && fs::is_directory(prefix))) {
				*to_create = subdir;
				create_exists = fs::is_directory(prefix);
			}
		}
		
		if(end == string::npos) {
			break;
		} else {
			start = end + 1;
		}
	}
	
	return fs::path();
}

static void findDataDirectory() {
	
	config.paths.data.clear();
	
	string temp;
	if(getSystemConfiguration("DataDir", temp)) {
		config.paths.data = temp;
		LogDebug("Got data directory from registry: " << config.paths.data);
		return;
	}
	
#ifdef DATA_DIR
	
	fs::path dir = expandEvironmentVariables(DATA_DIR);
	
#ifdef DATA_DIR_PREFIXES 
	if(dir.is_relative()) {
		config.paths.data = findSubdirectory(DATA_DIR_PREFIXES, dir);
		if(!config.paths.data.empty()) {
			LogDebug("Got data directory from DATA_DIR_PREFIXES: " << config.paths.data);
			return;
		}
	}
#endif // DATA_DIR_PREFIXES
	
	if(fs::is_directory(dir)) {
		config.paths.data = dir;
		LogDebug("Got data directory from DATA_DIR: " << config.paths.data);
		return;
	}
	
#endif // DATA_DIR
	
	LogDebug("No data directory found.");
}

static void findUserDirectory() {
	
	config.paths.user.clear();
	
	string temp;
	if(getSystemConfiguration("UserDir", temp)) {
		config.paths.user = temp;
		LogDebug("Got user directory from registry: " << config.paths.user);
		return;
	}
	
#ifdef USER_DIR
	
	fs::path dir = expandEvironmentVariables(USER_DIR);
	
	fs::path to_create;
#ifdef USER_DIR_PREFIXES
	if(dir.is_relative()) {
		config.paths.user = findSubdirectory(USER_DIR_PREFIXES, dir, &to_create);
		if(!config.paths.user.empty()) {
			LogDebug("Got user directory from USER_DIR_PREFIXES: " << config.paths.user);
			return;
		}
	}
#endif // USER_DIR_PREFIXES
	
	if(fs::is_directory(dir)) {
		config.paths.user = dir;
		LogDebug("Got user directory from USER_DIR: " << config.paths.user);
		return;
	}
	
	// Create a new user directory.
	if(!config.paths.data.empty()) {
		if(!to_create.empty()) {
			config.paths.user = to_create;
			LogDebug("Selected new user directory from USER_DIR_PREFIXES: " << config.paths.user);
		} else {
			config.paths.user = dir;
			LogDebug("Selected new user directory from USER_DIR: " << config.paths.user);
		}
		return;
	}
	
#endif // USER_DIR
	
	// Use the current directory for both data and config files.
	config.paths.user = ".";
	LogDebug("Using working directory as user directory: " << config.paths.user);
}

static void findConfigDirectory() {
	
	config.paths.config.clear();
	
#ifdef CONFIG_DIR
	
	fs::path dir = expandEvironmentVariables(CONFIG_DIR);
	
	fs::path to_create;
#ifdef CONFIG_DIR_PREFIXES
	if(dir.is_relative()) {
		config.paths.config = findSubdirectory(CONFIG_DIR_PREFIXES, dir, &to_create);
		if(!config.paths.config.empty()) {
			LogDebug("Got config directory from CONFIG_DIR_PREFIXES: " << config.paths.config);
			return;
		}
	}
#endif // USER_DIR_PREFIXES
	
	if(fs::is_directory(dir)) {
		config.paths.config = dir;
		LogDebug("Got config directory from CONFIG_DIR: " << config.paths.config);
		return;
	}
	
	// Create a new config directory.
	if(!config.paths.data.empty()) {
		if(!to_create.empty()) {
			config.paths.config = to_create;
			LogDebug("Selected new config directory from CONFIG_DIR_PREFIXES: "
			         << config.paths.config);
		} else {
			config.paths.config = dir;
			LogDebug("Selected new config directory from CONFIG_DIR: " << config.paths.config);
		}
		return;
	}
	
#endif // CONFIG_DIR
	
	// Use the user directory as the config directory.
	config.paths.config = config.paths.user;
	LogDebug("Using user directory as config directory: " << config.paths.config);
}

static bool createUserAndConfigDirectory() {
	
	if(config.paths.user.empty() || config.paths.config.empty()) {
		LogCritical << "No user / config directory available.";
		return false;
	}
	
	if(!fs::is_directory(config.paths.user)) {
		if(!fs::create_directories(config.paths.user)) {
			LogCritical << "Error creating user directory at " << config.paths.user;
			return false;
		}
		LogInfo << "Created new user directory at " << config.paths.user;
	}
	
	if(!fs::is_directory(config.paths.config)) {
		if(!fs::create_directories(config.paths.config)) {
			LogCritical << "Error creating config directory at " << config.paths.config;
			return false;
		}
		LogInfo << "Created new config directory at " << config.paths.config;
	}
	
	if(config.paths.data == config.paths.user) {
		config.paths.data.clear();
	}

	return true;
}

static void listDirectories(const string & regKey, const string & suffix = string(),
                            const string & where = string()) {
	
	if(!regKey.empty()) {
		std::cout << " - Registry key {HKCU,HKLM}\\Software\\ArxLibertatis\\" << regKey << '\n';
		string temp;
		if(getSystemConfiguration(regKey, temp)) {
			std::cout << "   = " << fs::path(temp) << '\n';
		}
	}
	
	if(suffix.empty()) {
		return;
	}
	fs::path dir = expandEvironmentVariables(suffix);
	
	if(!where.empty() && dir.is_relative()) {
		
		string prefixes = expandEvironmentVariables(where);
		
		std::cout << " - \"" << suffix << '"';
		if(dir.string() != suffix) {
			std::cout << " = " << fs::path(dir);
		}
		std::cout << " in one of \"" << where << '"';
		if(prefixes != where) {
			std::cout << "\n    = \"" << prefixes << '"';
		}
		std::cout << ":\n";
		
		std::set<fs::path> prefixset;
		
		size_t start = 0;
		while(true) {
			size_t end = prefixes.find(env_list_seperator, start);
			fs::path prefix = prefixes.substr(start, (end == string::npos) ? end : (end - start));
			if(prefixset.find(prefix) == prefixset.end()) {
				prefixset.insert(prefix);
				std::cout << "  * " << (prefix / dir) << '\n';
			}
			if(end == string::npos) {
				break;
			} else {
				start = end + 1;
			}
		}
	}
	
	std::cout << " - \"" << suffix << '"';
	if(dir.string() != suffix) {
		std::cout << " = " << fs::path(dir);
	}
	std::cout << '\n';
	
}

static void listDirectories() {
	
	std::cout << "\nData directories (data files):\n";
	std::cout << " - --data-dir (-d) command-line parameter\n";
#ifdef DATA_DIR
# ifdef DATA_DIR_PREFIXES 
	listDirectories("DataDir", DATA_DIR, DATA_DIR_PREFIXES);
# else
	listDirectories("DataDir", DATA_DIR);
# endif // DATA_DIR_PREFIXES
#else // DATA_DIR
	listDirectories("DataDir");
#endif // DATA_DIR
	std::cout << "selected: ";
	if(config.paths.data.empty()) {
		std::cout << "(none)\n";
	} else {
		std::cout << config.paths.data << '\n';
	}
	
	std::cout << "\nUser directories (save files, data files):\n";
	std::cout << " - --user-dir (-u) command-line parameter\n";
#ifdef USER_DIR
# ifdef USER_DIR_PREFIXES 
	listDirectories("UserDir", USER_DIR, USER_DIR_PREFIXES);
# else
	listDirectories("UserDir", USER_DIR);
# endif // USER_DIR_PREFIXES
#else // USER_DIR
	listDirectories("UserDir");
#endif // USER_DIR
	std::cout << " - Current working directory\n";
	std::cout << "selected: ";
	if(config.paths.user.empty()) {
		std::cout << "(none)\n";
	} else {
		std::cout << config.paths.user << '\n';
	}
	
	std::cout << "\nConfig directories:\n";
	std::cout << " - --config-dir (-c) command-line parameter\n";
#ifdef CONFIG_DIR
# ifdef CONFIG_DIR_PREFIXES 
	listDirectories(std::string(), CONFIG_DIR, CONFIG_DIR_PREFIXES);
# else
	listDirectories(std::string(), CONFIG_DIR);
# endif // USER_DIR_PREFIXES
#endif // CONFIG_DIR
	std::cout << " - The selected user directory\n";
	std::cout << "selected: ";
	if(config.paths.config.empty()) {
		std::cout << "(none)\n";
	} else {
		std::cout << config.paths.config << '\n';
	}
	std::cout << '\n';
}

#if ARX_PLATFORM != ARX_PLATFORM_WIN32
bool parseCommandLine(int argc, char ** argv) {
	
	std::string command_line;
	for(int i = 1; i < argc; i++) {
		command_line += argv[i];
		command_line += ' ';
	}
	
#else
bool parseCommandLine(const char * command_line) {
#endif
	
	CrashHandler::setVariable("Command line", command_line);

	po::options_description options_desc("Arx Libertatis Options");
	options_desc.add_options()
		("help,h", "Show supported options.")
		("no-data-dir,n", "Don't automatically detect a data directory.")
		("data-dir,d", po::value<string>(), "Where to find the data files.")
		("user-dir,u", po::value<string>(), "Where to store save files.")
		("config-dir,c", po::value<string>(), "Where to store config files.")
		("debug,g", po::value<string>(), "Log level settings.")
		("list-dirs,l", "List the searched user and data directories.")
	;
	
	po::variables_map options;
	
	try {
		
#if ARX_PLATFORM != ARX_PLATFORM_WIN32
		po::store(po::parse_command_line(argc, argv, options_desc), options);
#else
		std::vector<string> args = po::split_winmain(command_line);
		po::store(po::command_line_parser(args).options(options_desc).run(), options);
#endif
		
		po::notify(options);
		
		if(options.count("help")) {
			std::cout << options_desc << std::endl;
			return false;
		}
		
		po::variables_map::const_iterator debug = options.find("debug");
		if(debug != options.end()) {
			Logger::configure(debug->second.as<string>());
		}
		
		defineSystemDirectories();
		
		po::variables_map::const_iterator data_dir = options.find("data-dir");
		if(data_dir != options.end()) {
			config.paths.data = data_dir->second.as<string>();
			LogDebug("Got data directory from command-line: " << config.paths.data);
		} else if(options.count("no-data-dir")) {
			LogDebug("Disabled data directory.");
		} else {
			findDataDirectory();
		}
		if(!config.paths.data.empty() && !fs::is_directory(config.paths.data)) {
			LogWarning << "Data directory " << config.paths.data << " does not exist.";
			config.paths.data.clear();
		}
		
		po::variables_map::const_iterator user_dir = options.find("user-dir");
		if(user_dir != options.end()) {
			config.paths.user = user_dir->second.as<string>();
			LogDebug("Got user directory from command-line: " << config.paths.user);
		} else {
			findUserDirectory();
		}
		
		po::variables_map::const_iterator config_dir = options.find("config-dir");
		if(config_dir != options.end()) {
			config.paths.config = config_dir->second.as<string>();
			LogDebug("Got config directory from command-line: " << config.paths.config);
		} else {
			findConfigDirectory();
		}
		
		if(options.count("list-dirs")) {
			listDirectories();
			return false;
		}
		createUserAndConfigDirectory();
		
	} catch(po::error & e) {
		std::cerr << "Error parsing command-line: " << e.what() << "\n\n";
		std::cout << options_desc << std::endl;
		return false;
	}

	return true;
}
