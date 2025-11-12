#pragma once
#include "GameTypes/GameTypes.h"
#include "LoggingObserver/LoggingObserver.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// Forward declaration
class CommandProcessor;

// Tournament structure
struct Tournament {
    std::vector<std::string> maps;
    std::vector<std::string> strategies; // Strategy names
    int numGames;
    int maxTurns;
};

// Represent a command issued by the user (file or console)
class Command : public Subject, public ILoggable {
  private:
    std::string* commandText; // Full command text (including args)
    std::string* effectText;
    CommandType commandType;

  public:
    Command();
    Command(const std::string &cmd);
    Command(const Command &other);
    Command &operator=(const Command &other);

    std::string &getCommandText() const;
    std::string &getEffectText() const;
    CommandType getCommandType() const;
    std::string stringToLog() override;
    void saveEffect(const std::string &effect);
    friend std::ostream &operator<<(std::ostream &os, const Command &command);
    virtual ~Command();
};

// Interface for command processing
class ICommandProcessor {
  public:
    virtual ~ICommandProcessor() = default;
    virtual Command* getCommand() = 0;
    virtual bool
    validate(Command* command, StateType stateType, bool print = true) = 0;
    virtual void saveCommand(Command* command) = 0;
};

// Processes commands (console)
class CommandProcessor : public Subject,
                         public ILoggable,
                         public ICommandProcessor {
  protected:
    std::vector<Command*> commandsList;

  private:
    virtual Command* readCommand();
    bool validateTournament(Command* command, bool print = true);

  public:
    CommandProcessor();
    CommandProcessor(const CommandProcessor &commandProcessor);
    CommandProcessor &operator=(const CommandProcessor &rhs);
    virtual ~CommandProcessor();

    Command* getCommand() override;
    std::vector<Command*>* getCommandsList();
    bool
    validate(Command* command, StateType stateType, bool print = true) override;
    void saveCommand(Command* command) override;
    std::string stringToLog() override;
    Tournament prepareTournament(const std::string &cmdText);
    friend std::ostream &operator<<(std::ostream &os,
                                    const CommandProcessor &cp);
};

// Adapts to process commands from a file
class FileCommandProcessorAdapter : public CommandProcessor {
  private:
    std::string filename;
    std::string filepath;
    std::ifstream fileStream;
    Command* readCommand() override;

  public:
    explicit FileCommandProcessorAdapter(const std::string &filename);
    ~FileCommandProcessorAdapter() override;

    std::string getFileName() const { return filename; }
    friend std::ostream &operator<<(std::ostream &os,
                                    const FileCommandProcessorAdapter &fcpa);
};
