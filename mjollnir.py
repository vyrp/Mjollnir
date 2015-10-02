#!/usr/bin/python

"""
The Mjollnir master script.
Helps on the local development an agent of an AI challenge.
"""

## Imports and Constants ##

import json
import math
import os
import re
import shutil
import sys

from glob import glob
from inspect import getdoc
from os import path
from subprocess import call, CalledProcessError, check_call, Popen, STDOUT
from time import sleep, strftime

SOLUTIONSDIR = path.expanduser("~/mjollnir-solutions")
VIGRIDR = "/Mjollnir/vigridr"
VIGRIDRSRC = path.join(VIGRIDR, "src")
GAMESDIR = path.join(VIGRIDRSRC, "games")
PUBLISHED = "published"
NUM_PLAYERS = "num_players"
RESULT_TXT = "result.txt"
POSIX_CHARS = "[A-Za-z0-9._-]"

sys.path.append(VIGRIDRSRC)
from cache_state import cache_state
from change_game_code import change_game_code
sys.path.pop(-1)

## Global Variables ##

languages = ("cpp", "cs", "java", "py")
solution_name_regex = re.compile(r"^%s+$" % POSIX_CHARS)
folder_regex = re.compile(path.join(SOLUTIONSDIR, r"(%s+)" % POSIX_CHARS, r"(%s+)" % POSIX_CHARS) + "$")
logger = None
games_config = {}

# Load games configuration
for game in os.listdir(GAMESDIR):
    if not path.isdir(path.join(GAMESDIR, game)):
        continue
    with open(path.join(GAMESDIR, game, "config.json")) as config:
        games_config[game] = json.load(config)

# Only published games are available for development
games = tuple(sorted([game for game, config in games_config.iteritems() if config[PUBLISHED]]))

## Classes ##

class _DefaultLogger():
    """
    The default logger for the Mjollnir script.
    info -> stdout
    warn -> stdout (with the word 'WARNING' on front)
    err -> stderr (with the word 'ERROR' on front)
    """

    ERROR = "[ERROR]"
    WARNING = "[WARNING]"

    def info(self, msg, new_line=True):
        if new_line:
            print msg
        else:
            print msg,
    def warn(self, msg):
        print _DefaultLogger.WARNING, msg
    def err(self, msg):
        print >> sys.stderr, _DefaultLogger.ERROR, msg

class _SilentLogger(_DefaultLogger):
    """
    A logger that silences info and warn messages.
    """

    def info(self, msg):
        pass
    def warn(self, msg):
        pass

## Local functions ##

def _build_game(game):
    """
    Builds a game, in the vigridr directory.
    It doesn't restore the original files.

    Parameter:
        game - the game name as string
    """
    logger.info("The %s game has never been built. Building it..." % game)
    os.chdir(VIGRIDR)
    check_call(['make', 'directories'])
    check_call(['make', 'remove'])
    os.chdir(VIGRIDRSRC)
    change_game_code(game, copy_sample_clients=False, copy_tests=False, copy_obj=False, used_logger=_SilentLogger())
    check_call(['make', 'server'])
    cache_state(game)

def _check_correct_folder():
    """
    Checks if we are in a Mjollnir solution folder.
    The curent directory must match the `folder_regex` and contain a file
    whose name is the name of the solution and some extension, of an available language.

    Returns:
        correct_folder  - a boolean, whether we are in a solution folder
        solution_folder - a string, the full path of where we are
        game            - a string, the name of the game
        solution_name   - a string, the name of the solution
        language        - a string, the language of the solution
    """
    correct_folder, game, solution_name, language = False, None, None, None

    solution_folder = os.getcwd()
    m = folder_regex.match(solution_folder)
    if m:
        game, solution_name = m.groups()
        files = glob(solution_name + ".*")
        if game in games and len(files) == 1:
            language = files[0].split(".")[-1]
            if language in languages:
                correct_folder = True

    return correct_folder, solution_folder, game, solution_name, language

def _indent(doc_string, N):
    """
    Takes a piece of text and indents it so it displays correctly in the help message.

    Parameters:
        doc_string - a string, the text to indent
        N          - an integer, the amount of spaces to indent

    Returns:
        a string, the indented text
    """
    lines = doc_string.split("\n")
    lines = [line.strip() for line in lines]
    return ("\n" + " "*(4+N+3)).join(lines) # 4 spaces for tab, and 3 characters for " - "

def _move_log(game, solution_name, oponents, idx=""):
    """
    Moves the log file of a match to the logs folder of that game.

    Parameters:
        game          - a string, the name of the game
        solution_name - a string, the name of the solution
        oponents      - a list of strings, the names of the oponents
        idx           - a string or integer, used for indexing the logs of a batch of matches
    """
    logs_folder = path.join(SOLUTIONSDIR, game, "logs")
    if not path.isdir(logs_folder):
        os.mkdir(logs_folder)

    if idx:
        idx = "." + str(idx)
    shutil.move("logs", path.join(logs_folder, "-".join([strftime("%Y.%m.%d-%Hh%Mm%Ss"), solution_name] + oponents) + idx + ".log"))

def _cut_to_nth_appearance(string, separator, n):
    """
    Cuts a string up to the nth appearance of a separator

    Parameters:
        string    - the string to be cut
        separator - a string, the symbols to find and count
        n         - an integer

    Returns:
        the shortened string
    """
    return separator.join(string.split(separator)[:n])

## Exported Functions ##

def build(params):
    """
    With no parameters, builds the solution in the current folder.
    Parameters: [clean]

    clean - Delete build and bin folders of current solution.
    """

    # Requirements

    if len(params) > 1:
        logger.err("Wrong number of parameters\n")
        help(["build"])
        return 1

    correct_folder, solution_folder, game, solution_name, language = _check_correct_folder()
    if not correct_folder:
        logger.err("You are not in a solution folder")
        return 1

    clean = False
    if len(params) == 1:
        if params[0] == "clean":
            clean = True
        else:
            logger.err("Unrecognized option: " + params[0])
            return 1

    # Command execution

    build_folder = path.join("/sandboxes", "-".join([game, solution_name + "." + language]))
    if path.isdir(build_folder):
        logger.info("Deleting old build folder...")
        shutil.rmtree(build_folder)

    if clean:
        if path.isdir("./bin"):
            logger.info("Deleting bin folder...")
            shutil.rmtree("./bin")
        return 0

    full_lang = language
    if language == "cs":
        full_lang = "csharp"

    # Check if game code has been built
    if not path.isdir(path.join(GAMESDIR, game, "gen-" + full_lang)):
        try:
            _build_game(game)
        except CalledProcessError as e:
            logger.err(str(e))
            return 1

    try:
        logger.info("Changing game code...")
        os.chdir(VIGRIDRSRC)
        change_game_code(game, copy_sample_clients=True, copy_tests=False, copy_obj=False, used_logger=_SilentLogger())

        logger.info("Copying to build folder...")

        def ignore(src, names):
            """
            Returns the names the files inside `src` to NOT copy.
            """
            do_copy = names

            if src == VIGRIDR:
                do_copy = ["Makefile", "third-parties", "src"]

            elif src == VIGRIDRSRC:
                do_copy = ["Makefile", "Makefile.inc", "client", "thrifts"]

            elif src == path.join(VIGRIDRSRC, "client"):
                do_copy = ["Makefile", "GameClient." + language]
                if language == "cpp":
                    do_copy.append("ClientLogic.h")
                elif language == "java":
                    do_copy.append("java_run_script.sh")
                elif language == "py":
                    do_copy.append("python_run_script.sh")

            elif src == path.join(VIGRIDRSRC, "thrifts"):
                do_copy = ["gen-" + full_lang]

            elif src == path.join(VIGRIDR, "third-parties"):
                do_copy = ["python" if full_lang == "py" else full_lang]

            return list(set(names) - set(do_copy))

        shutil.copytree(VIGRIDR, build_folder, symlinks=True, ignore=ignore)

        logger.info("Copying your specific solution...")
        shutil.copy(path.join(solution_folder, solution_name + "." + language), path.join(build_folder, "src", "client", "ClientLogic." + language))

        os.chdir(path.join(build_folder, "src"))

        logger.info("Building solution...")
        os.makedirs(path.join(build_folder, "obj", "client"))
        os.makedirs(path.join(build_folder, "bin", full_lang))
        return_value = call(["make", "client" + full_lang])
        if return_value != 0:
            return return_value

        # Specific code for python, just to make a syntactic analysis
        if language == "py":
            filename = path.join(solution_folder, solution_name + "." + language)
            contents = ""
            with open(filename, "r") as f:
                contents = f.read()
            try:
                compile(contents, filename, "exec")
            except SyntaxError as e:
                logger.info("")
                logger.err(str(e))
                logger.info("")
                return 1

        logger.info("Copying back...")
        if path.isdir(path.join(solution_folder, "bin")):
            shutil.rmtree(path.join(solution_folder, "bin"))
        shutil.copytree(path.join(build_folder, "bin", full_lang), path.join(solution_folder, "bin"))

    finally:
        logger.info("Changing game code back to normal...")
        os.chdir(VIGRIDRSRC)
        change_game_code("template", copy_sample_clients=True, copy_tests=False, copy_obj=False, used_logger=_SilentLogger())

    return 0

def create(params):
    """
    Creates a new folder for a new game.
    Parameters: <game> <language> <solution_name> [--go]

    <game>          - The game name. One of: %s.
    <language>      - The solution language. One of: %s.
    <solution_name> - The name of the solution. Must only contain characaters of the POSIX Portable Character Set (%s).
    --go            - Creates a file to easily go to created folder. Run it with '. go'.
    """

    # Requirements

    if len(params) != 3 and len(params) != 4:
        logger.err("Wrong number of parameters\n")
        help(["create"])
        return 1

    game, language, solution_name = params[:3]

    if game not in games:
        logger.err("%s is not an available game" % game)
        logger.info("Possible games: " + " ".join(games))
        return 1

    if language not in languages:
        logger.err("%s is not an available language" % language)
        logger.info("Possible languages: " + " ".join(languages))
        return 1

    if not solution_name_regex.match(solution_name):
        logger.err("<solution_name> must contain only characaters of the POSIX Portable Character Set (%s)" % POSIX_CHARS)
        return 1

    reserved_names = set(("logs", "--go"))
    if solution_name in reserved_names:
        logger.err("%s is a reserved name" % solution_name)
        return 1

    go = False
    if len(params) == 4:
        if params[3] != "--go":
            logger.err("Unrecognized option: %s\n" % params[3])
            help(["create"])
            return 1
        go = True

    folder = path.join(SOLUTIONSDIR, game, solution_name)
    if path.exists(folder):
        logger.err("A solution with that name already exists for that game and language")
        return 1

    # Command execution

    os.makedirs(folder)
    shutil.copy(path.join(GAMESDIR, game, "sampleclient", "ClientLogic." + language), path.join(folder, solution_name + "." + language))

    logger.info("Sample solution file created in " + folder)

    if go:
        with open("go", "w") as f:
            f.write("cd " + folder + "\n")
            f.write("rm " + path.join(os.getcwd(), "go"))
        logger.info("'go' help file created in the current directory. Please run '. go'")

    return 0

create.__doc__ = create.__doc__ % (" ".join(games), " ".join(languages), POSIX_CHARS)

def help(params=[]):
    """
    Displays this help message or more help about a command.
    Parameters: [<command>]

    <command> - More help about <command>.
    """

    # Requirements

    if len(params) > 1:
        logger.err("Wrong number of parameters\n")
        help(["help"])
        return 1

    if len(params) == 1:
        if params[0] not in commands:
            logger.err("Unrecognized command: %s\n" % params[0])
            help()
            return 1
        logger.info("%-6s - %s\n" % (params[0], _indent(getdoc(commands[params[0]]), 6-4)))
        return 0

    # Command execution

    logger.info("This is the master command to develop solutions to the Mjollnir platform\n")

    logger.info("Usage:")
    logger.info("    mjollnir <command> [<parameters>]\n")

    logger.info("Commands:")
    for command in sorted(commands.keys()):
        logger.info("    %-6s - %s\n" % (command, _indent(_cut_to_nth_appearance(getdoc(commands[command]), "\n", 2), 6)))

    return 0

def replay(params):
    """
    Replays the specified match log. If no log is given, the latest is replayed.
    Parameters: [<match_log>]

    <match_log> - If given, the log to replay.
    """
    logger.err("Not yet implemented")
    # TODO: Implement
    return 1

def run(params):
    """
    Runs a match against the specified oponents, if any. Must be inside a solution folder.
    Parameters: [<solution_name>]* [--seed <NUM>] [--num <NUM>] [--show-oponents]

    <solution_name> - Parameter passed zero or more times. Indicates the oponents. Must come first.
    --seed <NUM>    - The seed to be used for randomness.
    --num <NUM>     - The amount of times to play.
    --show-oponents - Whether to show a console for each oponent. A console is always shown for the current solution.
    """

    __SEED, __NUM, __SHOW_OPONENTS = "--seed", "--num", "--show-oponents"

    # Requirements

    correct_folder, current_solution_folder, game, solution_name, language = _check_correct_folder()
    if not correct_folder:
        logger.err("You are not in a solution folder")
        return 1

    num_players = games_config[game][NUM_PLAYERS]

    if len(params) < num_players - 1:
        logger.err("Wrong number of parameters for this particular game. At least %d oponent%s needed\n" % (num_players - 1, " is" if num_players == 2 else "s are"))
        help(["run"])
        return 1

    if len(params) > num_players + 3:
        logger.err("Too many parameters for this particular game\n")
        help(["run"])
        return 1

    if not path.isfile(path.join(current_solution_folder, "bin", "client")):
        logger.err("This solution has not yet been built")
        return 1

    oponents = params[:num_players-1]
    options = params[num_players-1:]

    if not set([__SEED, __NUM, __SHOW_OPONENTS]).isdisjoint(oponents):
        logger.err("Too few oponents. At least %d oponent%s needed\n" % (num_players - 1, " is" if num_players - 1 == 1 else "s are"))
        help(["run"])
        return 1

    oponent_folders = []
    for oponent in oponents:
        oponent_folder = path.join(SOLUTIONSDIR, game, oponent)
        if not path.isdir(oponent_folder):
            logger.err("Cannot find the %s oponent" % oponent)
            return 1

        if not path.isfile(path.join(oponent_folder, "bin", "client")):
            logger.err("The %s oponent has not yet been built" % oponent)
            return 1
        oponent_folders.append(oponent_folder)

    # Check if game server has been built
    if not path.isfile(path.join(GAMESDIR, game, "bin", "server")):
        try:
            _build_game(game)
        except CalledProcessError as e:
            logger.err(str(e))
            return 1

    # Parsing options

    seed = None
    num = None
    show_oponents = False
    while len(options) > 0:
        param = options.pop(0)
        if param == __SEED:
            if not options:
                logger.err("Missing seed argument")
                return 1
            arg = options.pop(0)
            try:
                seed = int(arg)
            except ValueError:
                logger.err("Seed argument must be a number")
                return 1
            logger.warn("Seed option not yet implemented")

        elif param == __NUM:
            if not options:
                logger.err("Missing num argument")
                return 1
            arg = options.pop(0)
            try:
                num = int(arg)
            except ValueError:
                logger.err("Num argument must be a number")
                return 1
            if num <= 0:
                logger.err("Num argument must be positive")
                return 1

        elif param == __SHOW_OPONENTS:
            show_oponents = True

        else:
            logger.err("Unrecognized option: " + str(param))
            return 1

    if show_oponents and (num is not None):
        logger.warn("Both %s and %s passed. Ignoring %s." % (__SHOW_OPONENTS, __NUM, __SHOW_OPONENTS))

    ######## HACK ########
    # TODO: remove when a 1-player-game actually runs with only 1 client
    #
    # This hack makes a 1-player-game run with 2 clients.
    # Both are the user's solution, but the second client is ignored by the game server.
    if num_players == 1:
        num_players = 2
        oponents = [solution_name]
        oponent_folders = [current_solution_folder]

    # Command execution

    # TODO: implement seed option

    try:
        dev_null_file = open(os.devnull, "w")

        # Play only once
        if num is None or num == 1:
            client_command = "gnome-terminal -t Client%d --working-directory='%s' -x bash -c 'echo \"Waiting for connection...\" && sleep 2 && ./client --port %d; read -p \"Press enter to exit\"'"

            # This solution
            port = 9090
            logger.info("Opening client1: %s (this solution) @ %d..." % (solution_name, port))
            Popen(client_command % (1, path.join(current_solution_folder, "bin"), port), shell=True)

            # Oponents
            processes = []
            for i, oponent in enumerate(oponents):
                sleep(0.1) # Necessary so all oponents do not clog server by all connecting at the same time
                port = 9091 + i
                logger.info("Opening client%d: %s (oponent) @ %d..." % (i+2, oponent, port))
                if show_oponents:
                    Popen(client_command % (i+2, path.join(oponent_folders[i], "bin"), port), shell=True)
                else:
                    processes.append(Popen([path.join(oponent_folders[i], "bin", "client"), "--port", str(port)], stdout=dev_null_file, stderr=STDOUT))

            # Game server
            logger.info("Opening server...")
            with open(RESULT_TXT, "w") as result_file:
                try:
                    check_call([path.join(GAMESDIR, game, "bin", "server")], stdout=result_file) # This is a blocking command
                except CalledProcessError as e:
                    logger.err(str(e))
                    return 1

            sleep(0.2) # Let clients end
            for process, oponent in zip(processes, oponents):
                if process.poll() != 0:
                    logger.warn("Oponent %s had a runtime error" % oponent)

            # Get and show results
            with open(RESULT_TXT, "r") as result_file:
                result = result_file.read()
                try:
                    if result.startswith("s:"):
                        logger.info("\nSCORE: %d\n" % int(result[2:]))
                    elif result == "9090":
                        logger.info("\nRESULT: You won!\n")
                    elif result == "-1":
                        logger.info("\nRESULT: Draw.\n")
                    else:
                        winner = int(result)
                        if 9091 <= winner <= 9090 + len(oponents):
                            logger.info("\nRESULT: You lost...\n")
                        else:
                            logger.err("\nUnknown result: %d\n" % winner)
                except ValueError:
                    logger.err("\nUnknown result: %s\n" % result)

            logger.info("Moving log...")
            _move_log(game, solution_name, oponents)

        # Play NUM times
        else:
            # Calculated statistics, not all used; depends on game type
            wins = 0
            losses = 0
            draws = 0
            errors = 0
            total_points = 0

            digits = int(math.floor(math.log10(num)) + 1) # For pretty display
            for idx in xrange(num):
                logger.info("Running game %0*d..." % (digits, idx+1), False) # No new line
                sys.stdout.flush()

                with open(RESULT_TXT, "w") as result_file:
                    # Game server
                    server_process = Popen([path.join(GAMESDIR, game, "bin", "server")], stdout=result_file, stderr=dev_null_file)
                    sleep(0.1) # Upstart server time

                    # This solution
                    port = 9090
                    this_solution_process = Popen([path.join(current_solution_folder, "bin", "client"), "--port", str(port)], stdout=dev_null_file, stderr=STDOUT)

                    # Oponents
                    processes = []
                    for i, oponent in enumerate(oponents):
                        sleep(0.1) # Necessary so all oponents do not clog server by all connecting at the same time
                        port = 9091 + i
                        processes.append(Popen([path.join(oponent_folders[i], "bin", "client"), "--port", str(port)], stdout=dev_null_file, stderr=STDOUT))

                    # Wait for processes
                    server_process.wait()
                    this_solution_process.wait()
                    for p in processes:
                        p.wait()

                # See if any clients had a runtime error
                was_error = False
                if server_process.returncode != 0:
                    was_error = True
                    logger.info("Error (in server)")
                elif this_solution_process.returncode != 0:
                    was_error = True
                    logger.info("Error (in this solution)")
                else:
                    for process in processes:
                        if process.returncode != 0:
                            logger.info("Error (in an oponent)")
                            was_error = True
                            break

                # Check winner
                # Print Win, Loss, Draw
                # And accumulate statistics
                if was_error:
                    errors += 1
                else:
                    with open(RESULT_TXT, "r") as result_file:
                        result = result_file.read()
                        try:
                            if result.startswith("s:"):
                                points = int(result[2:])
                                total_points += points
                                logger.info("%dpts" % points)
                            elif result == "9090":
                                logger.info("You won")
                                wins += 1
                            elif result == "-1":
                                logger.info("Draw")
                                draws += 1
                            else:
                                winner = int(result)
                                if 9091 <= winner <= 9090 + len(oponents):
                                    logger.info("You lost")
                                    losses += 1
                                else:
                                    logger.info("Unknown result: %d" % winner)
                                    errors += 1
                        except ValueError:
                            logger.info("Unknown result: %s" % result)
                            errors += 1

                _move_log(game, solution_name, oponents, idx + 1)

            # Print summary
            logger.info("\nSummary:")
            if num_players == 1:
                logger.info("  Average points: %d" % float(points)/num)
                logger.info("  Errors: %d" % errors)
            else:
                logger.info("  Wins: %d" % wins)
                logger.info("  Draws: %d" % draws)
                logger.info("  Losses: %d" % losses)
                logger.info("  Errors: %d" % errors)

    finally: # Clean up
        if path.isfile(RESULT_TXT):
            os.remove(RESULT_TXT)

        dev_null_file.close()

    return 0

# Available commands. Must come after functions definition.
commands = {
    "build": build,
    "create": create,
    "help": help,
    "replay": replay,
    "run": run,
}

def mjollnir(command, params=[], logger_to_use=_DefaultLogger()):
    """
    The entry point for the Mjollnir master script

    Parameters:
        command       - the command name as string
        params        - a list of strings passed as parameters to the command
        logger_to_use - an instance of a class that has methods info, warn and err.
                        It is used for logging throughout the script.
                        It defaults to _DefaultLogger.
    """
    global logger
    logger = logger_to_use
    if command in commands:
        return commands[command](params)
    else:
        logger.err("'%s' is not a command\n" % command)
        help()
        return 1

if __name__ == "__main__":
    if len(sys.argv) == 1:
        ret = mjollnir("help") # Call `help` if no argument
    else:
        ret = mjollnir(sys.argv[1], sys.argv[2:])

    if ret == 0:
        logger.info("[Success]")

    exit(ret)

