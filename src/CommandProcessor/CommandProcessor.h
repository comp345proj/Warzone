#pragma once
#include "GameTypes/GameTypes.h"
#include "Observer/Observer.h"
#include "Subject/Subject.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Forward declaration
class CommandProcessor;

class Command : public Subject, public ILoggable {
  private:
	std::string* commandText;
	std::string* effectText;

  public:
	Command();
	Command(const std::string &cmd, const std::string &effect = "");
	Command(const Command &other);
	Command &operator=(const Command &other);

	std::string &getCommandText() const;
	std::string &getEffectText() const;
	std::string stringToLog() override;
	void saveEffect(const std::string &effect);
	friend std::ostream &operator<<(std::ostream &out, const Command &command);
	~Command();
};

// Interface for command processing
class ICommandProcessor {
  public:
	virtual ~ICommandProcessor() = default;
	virtual Command* getCommand() = 0;
	virtual bool validate(std::string cmdText, StateType state) = 0;
	virtual void saveCommand(Command* command) = 0;
};

class CommandProcessor : public Subject, public ILoggable, public ICommandProcessor {
  protected:
	std::vector<Command*> _commandsList;

  private:
	virtual Command* readCommand();

  public:
	CommandProcessor();
	CommandProcessor(const CommandProcessor &commandProcessor);
	CommandProcessor &operator=(const CommandProcessor &rhs);
	virtual ~CommandProcessor();

	Command* getCommand() override;
	std::vector<Command*>* getCommandsList();
	bool validate(std::string cmdText, StateType state) override;
	void saveCommand(Command* command) override;
	std::string stringToLog() override;
	friend std::ostream &operator<<(std::ostream &out,
									const CommandProcessor &cp);
};

class FileCommandProcessorAdapter : public CommandProcessor {
private:
    std::string filename;
    std::ifstream fileStream;
    Command* readCommand() override;

public:
    explicit FileCommandProcessorAdapter(const std::string& filename);
    ~FileCommandProcessorAdapter() override;
};
