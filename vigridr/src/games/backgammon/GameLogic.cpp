#include "GameLogic.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>

using std::vector;
using std::to_string;
using std::find;
using std::max_element;
using std::transform;
using std::remove_copy_if;

namespace mjollnir { namespace vigridr {

/** Helper functions **/

Move make_move(int32_t src, int32_t dst) {
  Move mv;
  mv.src = src;
  mv.dst = dst;
  return mv;
}

std::ostream& operator<<(std::ostream& os, Move m) {
    return os << m.src << "->" << m.dst;
}

std::ostream& operator<<(std::ostream& os, Command c) {
    os << "(";
    for (auto move : c.moves) {
      os << move << ", ";
    }
    return os << ")";
}

template<typename T>
std::ostream& operator<<(std::ostream& os, vector<T> v) {
    os << "[";
    for (auto item : v) {
      os << item << ", ";
    }
    return os << "]";
}

/** Methods from GameLogic **/

GameLogic::GameLogic(int32_t playerId1, int32_t playerId2) {
  srand(time(NULL));
  player1_ = playerId1;
  player2_ = playerId2;
  winner_ = "-1";
  hasFinished_ = false;
  playerIdToInvalid_[playerId1] = false;
  playerIdToInvalid_[playerId2] = false;

  worldModel_.board = {
    {2, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 5},

    {0, 0},
    {0, 3},
    {0, 0},
    {0, 0},
    {0, 0},
    {5, 0},

    {0, 5},
    {0, 0},
    {0, 0},
    {0, 0},
    {3, 0},
    {0, 0},

    {5, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 2}
  };
  worldModel_.bar = {0, 0};
  worldModel_.borne_off = {0, 0};
  rollDice_();
}

bool GameLogic::all_checkers_in_home_board_(WorldModel wm, PlayerColor color) {
  if (wm.bar[color] > 0) {
    return false;
  }

  size_t start = (color == RED ? 0 : 6);
  size_t end = (color == RED ? NP - 6 : NP);
  for (size_t src = start; src < end; ++src) {
    if (wm.board[src][color] > 0) {
      return false;
    }
  }
  return true;
}

void GameLogic::move_(int32_t src, int32_t dst, PlayerColor color) {
  size_t u_src = static_cast<size_t>(src);
  size_t u_dst = static_cast<size_t>(dst);

  if (src == FROM_BAR) {
    --worldModel_.bar[color];
  } else {
    --worldModel_.board[u_src][color];
  }

  if (dst == BEAR_OFF) {
    ++worldModel_.borne_off[color];
  } else {
    ++worldModel_.board[u_dst][color];

    int32_t& hit = worldModel_.board[u_dst][color^1];
    if (hit == 1) {
      --hit;
      ++worldModel_.bar[color^1];
    }
  }
}

bool GameLogic::try_move_(const WorldModel& wm, int32_t src, int32_t dst, PlayerColor color, WorldModel& new_wm) {
  size_t u_src = static_cast<size_t>(src);
  size_t u_dst = static_cast<size_t>(dst);

  if (!(src == FROM_BAR || (0 <= u_src && u_src < NP))) {
    return false;
  }

  if (!(dst == BEAR_OFF || (0 <= u_dst && u_dst < NP))) {
    return false;
  }

  // Copy board
  new_wm.board = wm.board;
  new_wm.bar = wm.bar;
  new_wm.borne_off = wm.borne_off;

  int32_t& from = (src == FROM_BAR ? new_wm.bar : new_wm.board[u_src])[color];
  if (from > 0) {
    --from;
  } else {
    return false;
  }

  if (dst == BEAR_OFF) {
    ++new_wm.borne_off[color];
    return true;
  }

  int32_t& to = new_wm.board[u_dst][color];
  int32_t& hit = new_wm.board[u_dst][color^1];
  if (to < NC && (hit == 0 || hit == 1)) {
    ++to;
    if (hit == 1) {
      --hit;
      ++new_wm.bar[color^1];
    }
  } else {
    return false;
  }

  return true;
}

vector<Command> GameLogic::calculate_possibilities_(WorldModel wm, Command command, PlayerColor color) {
  vector<Command> possibilities = { command };
  if (wm.dice.size() == 0) {
    return possibilities;
  }

  // From bar
  if (wm.bar[color] > 0) {
    int32_t die = wm.dice[0];

    WorldModel new_wm;
    new_wm.dice = vector<int32_t>(wm.dice.begin() + 1, wm.dice.end());

    int32_t dst = ((color == RED) ? (-1 + die) : (NP - die));
    if (try_move_(wm, FROM_BAR, dst, color, new_wm)) {
      Command new_command = command;
      new_command.moves.push_back(make_move(FROM_BAR, dst));
      vector<Command> new_poss = calculate_possibilities_(new_wm, new_command, color);
      possibilities.insert(possibilities.end(), new_poss.begin(), new_poss.end());
    }
    return possibilities;
  }

  // Normal play
  for (size_t src = 0; src < wm.board.size(); ++src) {
    if (wm.board[src][color] > 0) {
      int32_t die = wm.dice[0];

      WorldModel new_wm;
      new_wm.dice = vector<int32_t>(wm.dice.begin() + 1, wm.dice.end());

      int32_t dst = ((color == RED) ? (src + die) : (src - die));
      if (dst >= 0 && try_move_(wm, src, dst, color, new_wm)) {
        Command new_command = command;
        new_command.moves.push_back(make_move(src, dst));
        vector<Command> new_poss = calculate_possibilities_(new_wm, new_command, color);
        possibilities.insert(possibilities.end(), new_poss.begin(), new_poss.end());
      }
    }
  }

  // Bearing off
  if (all_checkers_in_home_board_(wm, color)) {
    int32_t die = wm.dice[0];

    WorldModel new_wm;
    new_wm.dice = vector<int32_t>(wm.dice.begin() + 1, wm.dice.end());

    int32_t src = (color == RED ? NP - die : -1 + die);
    int32_t direction = (color == RED ? -1 : +1);
    int32_t limit = (color == RED ? NP - 6 : 5);

    // Exact move
    if (wm.board[src][color] > 0 && try_move_(wm, src, BEAR_OFF, color, new_wm)) {
      Command new_command = command;
      new_command.moves.push_back(make_move(src, BEAR_OFF));
      vector<Command> new_poss = calculate_possibilities_(new_wm, new_command, color);
      possibilities.insert(possibilities.end(), new_poss.begin(), new_poss.end());
      return possibilities;
    }

    // Must move a checker in a higher point, if there exists
    bool has_higer = false;
    for (int32_t try_src = src + direction; try_src != limit + direction; try_src += direction) {
      if (wm.board[try_src][color] > 0) {
        has_higer = true;
      }
    }
    if (has_higer) {
      return possibilities;
    }

    // Can move a checker in a lower point, then
    direction = (color == RED ? +1 : -1);
    limit = (color == RED ? NP - 1 : 0);

    for (int32_t try_src = src + direction; try_src != limit + direction; try_src += direction) {
      if (wm.board[try_src][color] > 0) {
        if (try_move_(wm, try_src, BEAR_OFF, color, new_wm)) {
          Command new_command = command;
          new_command.moves.push_back(make_move(try_src, BEAR_OFF));
          vector<Command> new_poss = calculate_possibilities_(new_wm, new_command, color);
          possibilities.insert(possibilities.end(), new_poss.begin(), new_poss.end());
          return possibilities;
        }
      }
    }
  }

  return possibilities;
}

vector<Command> GameLogic::filter_commands_(const vector<Command>& possible_commands, PlayerColor color) {
  // Filtering, so we keep only those commands with the highest number of moves
  size_t max_size = max_element(possible_commands.begin(), possible_commands.end(), [](const Command& cmd1, const Command& cmd2){ return cmd1.moves.size() < cmd2.moves.size(); })->moves.size();

  vector<Command> filtered_commands(possible_commands.size());
  auto filtered_end = remove_copy_if(possible_commands.begin(), possible_commands.end(), filtered_commands.begin(), [max_size](const Command& cmd){ return cmd.moves.size() < max_size; });
  filtered_commands.resize(filtered_end - filtered_commands.begin());

  // Filtering, so we keep only those commands with the highest dice roll
  if (max_size == 1) {
    int32_t max_die = 0;

    vector<Command> filtered_commands2;
    for (const Command& cmd : filtered_commands) {
      const Move& move = cmd.moves[0];
      if (move.dst == BEAR_OFF) {
        continue;
      }

      int32_t src;
      if (move.src == FROM_BAR) {
        src = (color == RED ? -1 : NP);
      } else {
        src = move.src;
      }

      int32_t diff = abs(move.dst - src);
      if (diff == max_die) {
        filtered_commands2.push_back(cmd);
      } else if (diff > max_die) {
        filtered_commands2.clear();
        filtered_commands2.push_back(cmd);
        max_die = diff;
      }
    }

    filtered_commands = std::move(filtered_commands2);
  }

  return filtered_commands;
}

bool GameLogic::update(Command command, int32_t playerId) {
  PlayerColor color = color_(playerId);
  vector<Command> possible_commands;

  // Calculate all possible commands
  if (worldModel_.dice[0] == worldModel_.dice[1]) {
    worldModel_.dice.push_back(worldModel_.dice[0]);
    worldModel_.dice.push_back(worldModel_.dice[0]);
    possible_commands = calculate_possibilities_(worldModel_, Command(), color);
  } else {
    possible_commands = calculate_possibilities_(worldModel_, Command(), color);
    worldModel_.dice = { worldModel_.dice[1], worldModel_.dice[0] };
    auto possible_commands2 = calculate_possibilities_(worldModel_, Command(), color);
    possible_commands.insert(possible_commands.end(), possible_commands2.begin(), possible_commands2.end());
  }

  vector<Command> filtered_commands = filter_commands_(possible_commands, color);

  // Analysis
  bool success = (find(filtered_commands.begin(), filtered_commands.end(), command) != filtered_commands.end());

  if (success) {
    // Apply
    for (const auto& move : command.moves) {
      move_(move.src, move.dst, color);
    }

    // Check winner
    if (worldModel_.borne_off[RED] == NC) {
      hasFinished_ = true;
      winner_ = to_string(player1_);
    } else if (worldModel_.borne_off[WHITE] == NC) {
      hasFinished_ = true;
      winner_ = to_string(player2_);
    }
  } else {
    playerIdToInvalid_[playerId] = true;
    hasFinished_ = true;
  }

  rollDice_();
  return success;
}

void GameLogic::rollDice_() {
  worldModel_.dice = {rand() % 6 + 1, rand() % 6 + 1};
}

GameDescription GameLogic::getGameDescription(int32_t playerId) const {
  GameDescription gameDescription;
  gameDescription.myColor = color_(playerId);
  return gameDescription;
}

bool GameLogic::isFinished() const {
  return hasFinished_;
}

std::string GameLogic::getWinner() const {
  return winner_;
}

bool GameLogic::shouldPrintWorldModel(int32_t playerId){
  return true;
}

bool GameLogic::shouldIncrementCycle(int32_t playerId){
  return true;
}

size_t GameLogic::getNumberOfPlayers() const {
  return numberOfPlayers_;
}

WorldModel GameLogic::getWorldModel() const {
  return worldModel_;
}

TotalWorldModel GameLogic::getTotalWorldModel() const {
  return twm_;
}

GameResult GameLogic::createGameResult(std::string result, int32_t id) {
  GameResult gr;
  gr.won = (result == std::to_string(id));
  gr.invalid = playerIdToInvalid_[id];
  return gr;
}

inline PlayerColor GameLogic::color_(int32_t playerId) const {
  return static_cast<PlayerColor>(playerId - player1_);
}

void GameLogic::setBoard_forTest(const vector<Point>& board) {
  worldModel_.board = board;
  int32_t reds = 0;
  int32_t whites = 0;
  for (const auto& point : board) {
    reds += point[RED];
    whites += point[WHITE];
  }
  worldModel_.bar = {0, 0};
  worldModel_.borne_off = {NC - reds, NC - whites};
}

void GameLogic::setDice_forTest(const vector<int32_t>& dice) {
  worldModel_.dice = dice;
}

}}  // namespaces
