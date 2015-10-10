#!/usr/bin/python

"""
The Mjollnir master script.
Helps on the local development of an agent of an AI challenge.


Help
~~~~

To get general help:

$ mjollnir help

To get help about a specific command, pass it as an argument to the help command.
For example, about the 'create' command:

$ mjollnir help create


Tutorial
~~~~~~~~

1) Create a sample solution for a game, in the language of your preference.
For example, to create a solution named my-solution in python for Tic Tac Toe:

$ mjollnir create tictactoe py my-solution

Now you would edit your solution.

2) Build your solution.
In the specific case of python, it does a syntactic verification.
Note: you must be inside the solution folder.

$ mjollnir build

3) Run your solution.
If the game has more than one player, then you must pass the opponents as arguments.
Note: you must be inside the solution folder.

For example, to run the solution in the current folder against the 'random' solution:

$ mjollnir run random

4) Replay the last match in a beautiful interface in the browser:

$ mjollnir replay

Summary:
In a normal development, you would mostly alternate between editing your solution,
building it and running it (steps 2 and 3).

IMPORTANT: for now, the solution must consist of only one file (for any language).

Also, if you quickly want to navigate to the ~/mjollnir-solutions/ folder or to one of its
subfolders, you can use the 'mjollnir open' command.
For example, to go to the tictactoe folder:

$ mjollnir open tictactoe


Advanced Options
~~~~~~~~~~~~~~~~

There are other options to the 'run' command that may be used.

To run a game multiple times, pass it as the --num argument.
Note: no information is passed between one run and the next (for now).

For example, to run against the 'random' solution 10 times:

$ mjollnir run random --num 10

To pass a seed to the random generator of the server (game ambient) and the clients (agents),
use the --seed option.
Note: not yet implemented.

For example:

$ mjollnir run random --seed 1234

Finally, to show the output of the opponents, use the --show-opponents option.
For example, to run against the 'random' solution and show its output (besides your own):

$ mjollnir run random --show-opponents

(The options above can be combined.)


Hints
~~~~~

Autocomplete:
    Hitting TAB twice during the typing of a mjollnir command shows all possible options.
    For example:

    $ ls ~/mjollnir-solutions/
    tictactoe  wumpus
    $ mjollnir open [TAB][TAB]
    tictactoe  wumpus
    $ mjollnir open

    Hitting TAB once after some partially written command (or game, or solution name, etc.)
    completes it. For example:

    $ mjollnir rep[TAB]
    $ mjollnir replay

Run against yourself:
    You can run against yourself.
    For example, if you are in the ~/mjollnir-solutions/tictactoe/my-solution folder,
    you can run a my-solution instance against another my-solution instance:

    $ mjollnir run my-solution

"""

## Imports and Constants ##

import jinja2
import json
import math
import os
import re
import shutil
import sys

from datetime import datetime
from glob import glob
from inspect import getdoc
from os import path
from subprocess import call, CalledProcessError, check_call, check_output, Popen, STDOUT
from threading import Timer
from time import sleep, strftime

VERSION = "0.1"

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

sys.path.append("/Mjollnir/bifrost")
from extensions.string_building import time_since_from_seconds
sys.path.pop(-1)

## Global Variables ##

languages = ("cpp", "cs", "java", "py")
solution_name_regex = re.compile(r"^%s+$" % POSIX_CHARS)
folder_regex = re.compile(path.join(SOLUTIONSDIR, r"(%s+)" % POSIX_CHARS, r"(%s+)" % POSIX_CHARS) + "$")
log_filename_regex = re.compile(r"^(\d{4})\.(\d{2})\.(\d{2})-(\d{2})h(\d{2})m(\d{2})s:((?::%s+)+)(?:::\d+)?\.log$" % POSIX_CHARS)
logger = None
games_config = {}
jinja = jinja2.Environment(loader=jinja2.FileSystemLoader("/Mjollnir/mjollnir"), extensions=['jinja2.ext.autoescape'])

# Load games configuration
for game in os.listdir(GAMESDIR):
    if not path.isdir(path.join(GAMESDIR, game)):
        continue
    with open(path.join(GAMESDIR, game, "config.json")) as config:
        games_config[game] = json.load(config)

# Only published games are available for development
games = tuple(sorted([game for game, config in games_config.iteritems() if config[PUBLISHED]]))
del game, config

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

def _build_game(game, stdout=sys.stdout):
    """
    Builds a game, in the vigridr directory.
    It doesn't restore the original files.

    Parameter:
        game - the game name as string
    """
    logger.info("The %s game has never been built. Building it..." % game)
    os.chdir(VIGRIDR)
    check_call(['make', 'directories'], stdout=stdout)
    check_call(['make', 'remove'], stdout=stdout)
    os.chdir(VIGRIDRSRC)
    change_game_code(game, copy_sample_clients=False, copy_tests=False, copy_obj=False, used_logger=_SilentLogger())
    check_call(['make', 'server'], stdout=stdout)
    cache_state(game)

def _check_correct_folder():
    """
    Checks if we are in a Mjollnir solution folder.
    The current directory must match the `folder_regex` and contain a file
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
    return ("\n" + (" " * N)).join(doc_string.split("\n"))

def _move_log(game, solution_name, opponents, timestamp=strftime("%Y.%m.%d-%Hh%Mm%Ss"), idx=""):
    """
    Moves the log file of a match to the logs folder of that game.

    Parameters:
        game          - a string, the name of the game
        solution_name - a string, the name of the solution
        opponents     - a list of strings, the names of the opponents
        timestamp     - a string with the current datetime
        idx           - a string used for indexing the logs of a batch of matches
    """
    logs_folder = path.join(SOLUTIONSDIR, game, "logs")
    if not path.isdir(logs_folder):
        os.mkdir(logs_folder)

    if idx:
        idx = "::" + str(idx)
    shutil.move("logs", path.join(logs_folder, timestamp + ":".join([":", solution_name] + opponents) + idx + ".log"))

## Exported Functions ##

def build(params, stdout=sys.stdout):
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

    try:
        # Check if game code has been built
        if not path.isdir(path.join(GAMESDIR, game, "gen-" + full_lang)):
            try:
                _build_game(game)
            except CalledProcessError as e:
                logger.err(str(e))
                return 1

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
                if language == "cpp":
                    do_copy.append("utils")

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
        if language == "cpp":
            os.makedirs(path.join(build_folder, "obj", "thrifts", "gen-cpp"))
            os.makedirs(path.join(build_folder, "obj", "utils"))
        os.makedirs(path.join(build_folder, "bin", full_lang))
        return_value = call(["make", "client" + full_lang], stdout=stdout)
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

    except KeyboardInterrupt as e:
        logger.err(repr(e))
        return 1

    finally:
        logger.info("Changing game code back to normal...")
        os.chdir(VIGRIDRSRC)
        change_game_code("template", copy_sample_clients=True, copy_tests=False, copy_obj=False, used_logger=_SilentLogger())

    return 0

def create(params):
    """
    Creates a new folder for a new game.
    Parameters: <game> <language> <solution_name>

    <game>          - The game name. One of: %s.
    <language>      - The solution language. One of: %s.
    <solution_name> - The name of the solution. Must only contain characters of the POSIX
                      Portable Character Set (%s).
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
        logger.err("A solution with that name already exists for that game")
        return 1

    # Command execution

    os.makedirs(folder)
    shutil.copy(path.join(GAMESDIR, game, "sampleclient", "ClientLogic." + language), path.join(folder, solution_name + "." + language))

    logger.info("Sample solution file created in " + folder)

    # Creating file with the location so we can go there. See /Mjollnir/mjollnir/include-mjollnir
    with open(path.expanduser("~/location"), "w") as f:
        f.write(folder)

    return 0

create.__doc__ = create.__doc__ % (" ".join(games), " ".join(languages), POSIX_CHARS)

def help(params=[]):
    """
    Displays this help message or more help about a command.
    Parameters: [<command> | --tutorial]

    <command>  - More help about <command>.
    --tutorial - Show Mjollnir tutorial.
    """

    # Requirements

    if len(params) > 1:
        logger.err("Wrong number of parameters\n")
        help(["help"])
        return 1

    if len(params) == 1:
        if params[0].startswith("-"):
            if params[0] != "--tutorial":
                logger.err("Unrecognized option: %s\n" % params[0])
                help(["help"])
                return 1
            print getdoc(sys.modules[__name__]) + "\n"
            return 0
        if params[0] not in commands:
            logger.err("Unrecognized command: %s\n" % params[0])
            help()
            return 1
        logger.info("    %s\n        %s\n" % (params[0], _indent(getdoc(commands[params[0]]), 8)))
        return 0

    # Command execution

    logger.info("This is the master command to develop solutions to the Mjollnir platform")
    logger.info("Version: %s\n" % VERSION)

    logger.info("Usage:")
    logger.info("    mjollnir <command> [<parameters>]\n")

    logger.info("Commands:")
    for command in sorted(commands.keys()):
        logger.info("    %s\n        %s\n" % (command, _indent(getdoc(commands[command]).split("\n\n")[0], 8)))

    return 0

def list_options(params):
    """
    List possibilities for given parameter type, in one line, separated by spaces.
    Mainly made for other scripts.
    Parameters: --commands | --games | --languages | --matches | --solutions

    --commands  - List possible commands
    --games     - List available games
    --languages - List available languages
    --matches   - List existing match logs (for all games)
    --solutions - List existing solutions for the game in which you are currently located
    """

    # Requirements

    if len(params) != 1:
        logger.err("Wrong number of parameters\n")
        help(["list"])
        return 1

    # Parsing options

    if params[0] == "--commands":
        logger.info(" ".join(sorted(commands.keys())), False)

    elif params[0] == "--games":
        logger.info(" ".join(games), False)

    elif params[0] == "--languages":
        logger.info(" ".join(languages), False)

    elif params[0] == "--matches":
        logger.info(" ".join([path.basename(log) for log in glob(path.join(SOLUTIONSDIR, "*", "logs", "*.log"))]), False)

    elif params[0] == "--solutions":
        correct_folder, _, game, _, _ = _check_correct_folder()
        if not correct_folder:
            logger.err("You are not in a solution folder")
            return 1
        logger.info(" ".join(filter(lambda s: s != "logs", os.listdir(path.join(SOLUTIONSDIR, game)))), False)

    else:
        logger.err("Unkown option: %s\n" % params[0])
        help(["list"])
        return 1

    return 0

def open_folder(params):
    """
    Go to the mjollnir-solutions directory if no argument is specified, else go to the given
    parameters folder.
    Parameters: [<game> [<solution_name>]]

    <game>          - A game for which you have solutions
    <solution_name> - A solution that you created for that game
    """

    # Requirements and parsing options

    if len(params) > 2:
        logger.err("Wrong number of parameters\n")
        help(["open"])
        return 1

    game, solution_name = None, None
    if len(params) >= 1:
        game = params[0]
        if game not in games:
            logger.err("%s is not an available game" % game)
            logger.info("Possible games: " + " ".join(games))
            return 1
        if not path.isdir(path.join(SOLUTIONSDIR, game)):
            logger.err("You never created a solution in %s" % game)
            return 1

    if len(params) == 2:
        solution_name = params[1]
        if not path.isdir(path.join(SOLUTIONSDIR, game, solution_name)):
            logger.err("Solution %s doesn't exist" % solution_name)
            return 1

    # Command execution

    target = SOLUTIONSDIR
    if game:
        target = path.join(target, game)
    if solution_name:
        target = path.join(target, solution_name)

    # Creating file with the location so we can go there. See /Mjollnir/mjollnir/include-mjollnir
    with open(path.expanduser("~/location"), "w") as f:
        f.write(target)

    return 0

def replay(params):
    """
    Replays the specified match log. If no log is given, the latest is replayed.
    Parameters: [<match_log>]

    <match_log> - If given, the log to replay. Must have a standard log name.
    """

    # Requirements

    if len(params) > 1:
        logger.err("Wrong number of parameters\n")
        help(["replay"])
        return 1

    # Command execution

    # Choosing match log depending on parameters
    if len(params) == 1:
        match_log = params[0]
        if not path.isfile(match_log):
            logger.err("Could not find match log '%s'" % match_log)
            return 1
        if not log_filename_regex.match(match_log.split(os.sep)[-1]):
            logger.err("The given match_log doesn't match the standard name")
            return 1
    else:
        possibilities = glob(path.join(SOLUTIONSDIR, "*", "logs", "*.log"))
        if not possibilities:
            logger.warn("No logs found")
            return 1

        match_log = max(possibilities, key=path.getctime)
        logger.info("Latest match log found: %s" % match_log)

    # Generating HTML
    if path.isfile(match_log + ".html"):
        logger.info("Html already generated")
    else:
        m = log_filename_regex.match(match_log.split(os.sep)[-1])
        if not m:
            logger.err("Found a log with a strange name: %s" % match_log.split(os.sep)[-1])
            return 1

        logger.info("Generating html...")

        # Getting parameters
        timestamp = datetime(*map(int, m.groups()[:6]))
        solution_names = m.group(7).split(":")[1:]
        challenge_name = path.dirname(path.abspath(match_log)).split("/")[-2]

        values = {
            "match": {
                "challenge_name": challenge_name[0].upper() + challenge_name[1:],
                "solution_names": solution_names,
                "time_since": time_since_from_seconds((datetime.now() - timestamp).total_seconds()),
                "solutions": [{"id": "p%d" % (idx+1), "name": solution} for idx, solution in enumerate(solution_names)],
                "visualizer": challenge_name + ".js",
                "log_json": open(match_log, "r").read().strip(),
            },
            "mjollnir_assets": "/MjollnirAssets",
        }

        # Creating html from jinja template
        with open(match_log + ".html", "w") as result:
            result.write(jinja.get_template("replay_template.html").render(values))

    logger.info("Opening firefox...")
    Popen(["firefox", match_log + ".html"])

    return 0

def run(params):
    """
    Runs a match against the specified opponents, if any. Must be inside a solution folder.
    Parameters: [<solution_name>]* [--seed <NUM>] [--num <NUM>] [--show-opponents]

    <solution_name>  - Parameter passed zero or more times. Indicates the opponents.
                       Must come first.
    --seed <NUM>     - The seed to be used for randomness.
    --num <NUM>      - The amount of times to play.
    --show-opponents - Whether to show a console for each opponent.
                       A console is always shown for the current solution.
    """

    __SEED, __NUM, __SHOW_OPPONENTS = "--seed", "--num", "--show-opponents"

    # Requirements

    correct_folder, current_solution_folder, game, solution_name, language = _check_correct_folder()
    if not correct_folder:
        logger.err("You are not in a solution folder")
        return 1

    num_players = games_config[game][NUM_PLAYERS]

    if len(params) < num_players - 1:
        logger.err("Wrong number of parameters for this particular game. At least %d opponent%s needed\n" % (num_players - 1, " is" if num_players == 2 else "s are"))
        help(["run"])
        return 1

    if len(params) > num_players + 3:
        logger.err("Too many parameters for this particular game\n")
        help(["run"])
        return 1

    if not path.isfile(path.join(current_solution_folder, "bin", "client")):
        logger.err("This solution has not yet been built")
        return 1

    opponents = params[:num_players-1]
    options = params[num_players-1:]

    if not set([__SEED, __NUM, __SHOW_OPPONENTS]).isdisjoint(opponents):
        logger.err("Too few opponents. At least %d opponent%s needed\n" % (num_players - 1, " is" if num_players - 1 == 1 else "s are"))
        help(["run"])
        return 1

    opponent_folders = []
    for opponent in opponents:
        opponent_folder = path.join(SOLUTIONSDIR, game, opponent)
        if not path.isdir(opponent_folder):
            logger.err("Cannot find the %s opponent" % opponent)
            return 1

        if not path.isfile(path.join(opponent_folder, "bin", "client")):
            logger.err("The %s opponent has not yet been built" % opponent)
            return 1
        opponent_folders.append(opponent_folder)

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
    show_opponents = False
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

        elif param == __SHOW_OPPONENTS:
            if num_players == 1:
                logger.warn("Ignoring %s argument, since this game has only one player" % __SHOW_OPPONENTS)
            else:
                show_opponents = True

        else:
            logger.err("Unrecognized option: " + str(param))
            return 1

    if show_opponents and (num is not None):
        logger.warn("Both %s and %s passed. Ignoring %s." % (__SHOW_OPPONENTS, __NUM, __SHOW_OPPONENTS))

    ######## HACK ########
    # TODO: remove when a 1-player-game actually runs with only 1 client
    #
    # This hack makes a 1-player-game run with 2 clients.
    # Both are the user's solution, but the second client is ignored by the game server.
    if num_players == 1:
        opponents = [solution_name]
        opponent_folders = [current_solution_folder]

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

            # Used to capture the variables in the for below
            def create_opponent_command(i, port):
                def run_process():
                    try:
                        processes.append(Popen([path.join(opponent_folders[i], "bin", "client"), "--port", str(port)], stdout=dev_null_file, stderr=STDOUT))
                    except ValueError as e:
                        if e.message != "I/O operation on closed file":
                            logger.err("Opponent %d: %s" % (i + 1, str(e)))
                return run_process

            processes = []
            for i, opponent in enumerate(opponents):
                sleep(0.1) # Necessary so all opponents do not clog server by all connecting at the same time
                port = 9091 + i
                logger.info("Opening client%d: %s (opponent) @ %d..." % (i+2, opponent, port))
                if show_opponents:
                    Popen(client_command % (i+2, path.join(opponent_folders[i], "bin"), port), shell=True)
                else:
                    # Run this opponent only in 2s
                    Timer(2, create_opponent_command(i, port)).start()

            # Game server
            logger.info("Opening server...")
            with open(RESULT_TXT, "w") as result_file:
                try:
                    check_call([path.join(GAMESDIR, game, "bin", "server")], stdout=result_file) # This is a blocking command, therefore it must come after clients
                except CalledProcessError as e:
                    logger.err(str(e))
                    return 1

            sleep(0.2) # Let clients end
            for process, opponent in zip(processes, opponents):
                if process.poll() != 0:
                    logger.warn("Oponent %s had a runtime error" % opponent)

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
                        if 9091 <= winner <= 9090 + len(opponents):
                            logger.info("\nRESULT: You lost...\n")
                        else:
                            logger.err("\nUnknown result: %d\n" % winner)
                except ValueError:
                    logger.err("\nUnknown result: %s\n" % result)

            logger.info("Moving log...")
            _move_log(game, solution_name, opponents)

        # Play NUM times
        else:
            # Calculated statistics, not all used; depends on game type
            wins = 0
            losses = 0
            draws = 0
            errors = 0
            total_points = 0

            timestamp = strftime("%Y.%m.%d-%Hh%Mm%Ss")
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
                    for i, opponent in enumerate(opponents):
                        sleep(0.1) # Necessary so all opponents do not clog server by all connecting at the same time
                        port = 9091 + i
                        processes.append(Popen([path.join(opponent_folders[i], "bin", "client"), "--port", str(port)], stdout=dev_null_file, stderr=STDOUT))

                    # Wait for processes
                    server_process.wait()
                    # TODO: should we really wait for the clients?
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
                            logger.info("Error (in an opponent)")
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
                                if 9091 <= winner <= 9090 + len(opponents):
                                    logger.info("You lost")
                                    losses += 1
                                else:
                                    logger.info("Unknown result: %d" % winner)
                                    errors += 1
                        except ValueError:
                            logger.info("Unknown result: %s" % result)
                            errors += 1

                _move_log(game, solution_name, opponents, timestamp, "%0*d" % (digits, idx+1))

            # Print summary
            logger.info("\nSummary:")
            if num_players == 1:
                logger.info("  Average points: %.2f" % (float(total_points)/num))
                logger.info("  Errors: %d" % errors)
            else:
                logger.info("  Wins: %d" % wins)
                logger.info("  Draws: %d" % draws)
                logger.info("  Losses: %d" % losses)
                logger.info("  Errors: %d" % errors)

    except KeyboardInterrupt as e:
        logger.err(repr(e))
        return 1

    finally: # Clean up
        if path.isfile(RESULT_TXT):
            os.remove(RESULT_TXT)

        dev_null_file.close()

    return 0

def update(params):
    """
    Updates the repository and run necessary environment modifications.
    Please run this command instead of only 'git pull'.
    No parameters.
    """

    # Requirements

    if len(params) != 0:
        logger.err("This command receives no parameters\n")
        help(["update"])
        return 1

    # Command execution

    try:
        # Make sure we are on master
        os.chdir("/Mjollnir")
        m = re.search(r"\* (.*)", check_output(["git", "branch"]))
        if not m:
            logger.err("Could not figure out current git branch")
            return 1
        if m.group(1) != "master":
            logger.err("You are not on branch master")
            return 1

        logger.info("Running git pull...")
        output = check_output(["git", "pull"])
        if output == "Already up-to-date.\n":
            print "Already up-to-date. Nothing to do."
            return 0

        # We must import only after having pulled, so we have the latest version
        import updates
        return updates.update(sys.modules[__name__])

    except CalledProcessError as e:
        logger.err(str(e))
        return 1

    except KeyboardInterrupt as e:
        logger.err(repr(e))
        return 1

# Available commands. Must come after functions definition.
commands = {
    "build": build,
    "create": create,
    "help": help,
    "list": list_options,
    "open": open_folder,
    "replay": replay,
    "run": run,
    "update": update,
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

    if ret == 0 and not (len(sys.argv) > 1 and sys.argv[1] == "list"):
        logger.info("[Success]")

    exit(ret)

__all__ = [func.__name__ for func in commands.values()] + ["mjollnir", "VERSION"]

