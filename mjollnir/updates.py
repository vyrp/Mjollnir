"""
This script contains actions that should be executed after a 'git pull',
so that the virtual machine's environment is properly updated.
"""

## Imports and constants ##

import json
import os
import sys

from glob import glob
from os import path
from subprocess import CalledProcessError, check_output

MJOLLNIR = "/Mjollnir/mjollnir/"
BIFROST = "/Mjollnir/bifrost/"
VIGRIDRSRC = "/Mjollnir/vigridr/src/"

sys.path.append(VIGRIDRSRC)
from change_game_code import change_game_code
sys.path.pop(-1)

## Local functions ##

def _changed(filename_or_foldername):
    """
    Checks if the argument was changed between the current master and the last position of master.
    Parameters:
        filename_or_foldername - a string, the name of the item to check
    Returns:
        a bool - whether it was changed
    """
    return check_output(["git", "diff", "--name-only", "master@{1}", "master", filename_or_foldername]) != ""

## Exported functions ##

def update(mjollnir):
    build_game = mjollnir._build_game
    build_solution = mjollnir.build
    logger = mjollnir.logger

    # Silence invocations of build_game and build_solution
    mjollnir.logger = mjollnir._SilentLogger()

    try:
        logger.info("Checking for configuration changes...")
        something_changed = False
        dev_null = open(os.devnull, "w")

        if _changed(VIGRIDRSRC):
            logger.info(" * Source code for games changed")
            something_changed = True
            logger.info("   -> Rebuilding all game binaries")
            for game in glob(path.join(VIGRIDRSRC, "games", "*")):
                if not path.isdir(game):
                    continue
                if not json.load(open(path.join(game, "config.json"), "r"))["published"]:
                    continue
                try:
                    game_name = path.basename(game)
                    logger.info("      Game '%s'..." % game_name)
                    build_game(game_name, stdout=dev_null)
                except CalledProcessError as e:
                    logger.warn("Failure to build game '%s'" % game_name)
                    print str(e)
                    # If game failed, doesn't matter. Go to next.

            bin_folders = glob(path.expanduser(path.join("~", "mjollnir-solutions", "*", "*", "bin")))
            if bin_folders:
                logger.info("   -> Rebuilding solution binaries")
                for bin_folder in bin_folders:
                    solution_folder = path.dirname(bin_folder)
                    solution_name = path.basename(solution_folder)
                    game = path.basename(path.dirname(solution_folder))
                    logger.info("      Solution '%s/%s'..." % (game, solution_name))
                    os.chdir(solution_folder)
                    if build_solution([], stdout=dev_null) != 0:
                        logger.warn("Failure to build solution '%s'" % solution_name)
                        # If solution failed, doesn't matter. Go to next.

        if _changed(path.join(MJOLLNIR, "autocomplete-mjollnir")) or _changed(path.join(MJOLLNIR, "include-mjollnir")):
            logger.info(" * A shell script was changed.")
            logger.info("   -> In order to get full capabilities, please either")
            logger.info("      close and reopen your terminals or run '. ~/.bashrc' in each of them.")
            something_changed = True

        if not something_changed:
            logger.info("Nothing to do.")

    except KeyboardInterrupt as e:
        logger.err(repr(e))
        return 1

    finally:
        os.chdir(VIGRIDRSRC)
        change_game_code("template", copy_sample_clients=True, copy_tests=False, copy_obj=False, used_logger=mjollnir._SilentLogger())
        mjollnir.logger = logger
        dev_null.close()

    return 0

__all__ = ["update"]

