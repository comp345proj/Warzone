#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Subject/Subject.h"
#include "Observer/Observer.h"

enum class StateType;

class Command : public Subject, public ILoggable{
	private:
		std::string* commandText;
		std::string* effectText;

	public:
		Command();
		Command(const std::string &cmd);
		Command(const std::string &cmd, const std::string &effect);
		Command(const Command &other);
		Command& operator=(const Command &other);

		std::string& getCommand() const;
		std::string& getEffect() const;
		std::string stringToLog() override;
		void saveEffect(const std::string &effect);
		friend std::ostream& operator<<(std::ostream& out, const Command &command);
		~Command();
};

class CommandProcessor : public Subject, public ILoggable{
	private:
		std::vector<Command*> _commandHistory;
		void saveCommand(const std::string& command);
		void readCommand();

	public:
		CommandProcessor();
    	CommandProcessor(const CommandProcessor &commandProcessor);
    	CommandProcessor& operator=(const CommandProcessor &rhs);
    	~CommandProcessor();
		
		Command& getCommand() const;
		std::string stringToLog() override;
		bool validate(Command& command, StateType gameState);
		friend std::ostream& operator<<(std::ostream& out, const CommandProcessor &cp);
};

class FileLineReader {
	private:
		std::ifstream *_fileStream;

	public:
		FileLineReader();
		FileLineReader(const std::string& filePath);
		~FileLineReader();

		friend std::ostream& operator<<(std::ostream& out, const FileLineReader &flr);
		std::string& readLineFromFile();
};

class FileCommandProcessorAdapter : public CommandProcessor {
	private:
		FileLineReader *_fileLineReader;

	public:    
		FileCommandProcessorAdapter();
		FileCommandProcessorAdapter(const std::string& filePath);
		FileCommandProcessorAdapter(const FileCommandProcessorAdapter& fcp);
		~FileCommandProcessorAdapter();
		
		FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter &rhs);
		friend std::ostream& operator<<(std::ostream& out, const FileCommandProcessorAdapter &fcp);
		std::string& readCommand();
};