#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include "CommandProcessor.h"
#include "RESPReplies.h"


TEST(CommandProcessor, Ping) {
    Command command({"ping"});
    CommandProcessor processor;

    EXPECT_EQ(processor.execute(command), "+PONG\r\n");
}

TEST(CommandProcessor, SetGet) {
    Command command({"set", "key", "value"});

    CommandProcessor processor;
    processor.execute(command);

    command = Command({"get", "key"});
    processor.execute(command);

    EXPECT_EQ(processor.execute(command), "$5\r\nvalue\r\n");
}

TEST(CommandProcessor, SetGetDel) {
    CommandProcessor processor;
    const std::string key = "test:command-processor:set-get-del";

    Command set({"SET", key, "value"});
    EXPECT_EQ(processor.execute(set), "+OK\r\n");

    Command get({"GET", key});
    EXPECT_EQ(processor.execute(get), "$5\r\nvalue\r\n");

    Command del({"DEL", key});
    EXPECT_EQ(processor.execute(del), ":1\r\n");
}

TEST(RESPParser, UnknownCommand) {
    CommandProcessor processor;
    
    Command command({"trinity", "key", "value"});

    EXPECT_EQ(processor.execute(command),
        std::string(resp::ERROR_UNKNOWN_COMMAND)
        + command.name
        + std::string(resp::ERROR_UNKNOWN_COMMAND_MULTIPLE_ARGUMENTS)
        + command.args[0]
        + "'"
        + std::string(resp::CRLF));
}

TEST(RESPParser, WrongArgumentsNumber) {
    CommandProcessor processor;
    
    Command command({"SET", "key"});
    
    EXPECT_EQ(processor.execute(command),
    std::string(resp::ERROR_ARGUMENTS_NUMBER)
    + command.name
    + "' command"
    + std::string(resp::CRLF));
}