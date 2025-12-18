#pragma once

#include "board.hpp"
#include "resource_manager.hpp"
#include "renderer.hpp"
#include "text_renderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
#include <memory>
#include <locale>
#include "stb_image.h"
#include <string>
#include <array>
#include <unordered_map>

const int NB_DIFF_PIECES = 6;

enum class GameResult {
    DRAW_STALEMATE,
    DRAW_REPETITION,
    DRAW_DEAD_POSITION,
    DRAW_FIFTY_MOVES,
    DRAW_AGREEMENT,
    CHECKMATE,
    RESIGNATION
};

enum class GameState {
    //MENU, maybe ?
    RUNNING,
    PROMOTION,
    FINISHED
};



enum class Color : size_t {
    // tiles
    LIGHT = 0,
    DARK,
    HIGHLIGHTED,
    SELECTED,
    CHECK,
    CHECKMATE,

    TRANSLUSCENT_GRAY,
    SIDEBAR,
    WHITE_PIECE,
    BLACK_PIECE,
    WHITE
};

const std::unordered_map<Color, glm::vec4> COLOR_MAP = {
    {Color::LIGHT,              glm::vec4(0.82f, 0.94f, 0.75f, 1.0f)},
    {Color::DARK,               glm::vec4(0.13f, 0.55f, 0.13f, 1.0f)},
    {Color::HIGHLIGHTED,        glm::vec4(0.2f, 0.2f, 0.2f, 0.5f)},
    {Color::SELECTED,           glm::vec4(0.6f, 0.8f, 0.2f, 0.8f)},
    {Color::CHECK,              glm::vec4(1.0f, 0.0f, 0.0f, 0.6f)},
    {Color::TRANSLUSCENT_GRAY,  glm::vec4(0.2f, 0.2f, 0.2f, 0.8f)},
    {Color::SIDEBAR,            glm::vec4(0.44f, 0.47f, 0.49f, 1.0f)},
    {Color::WHITE_PIECE,        glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)},
    {Color::BLACK_PIECE,        glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)},
    {Color::WHITE,              glm::vec4(0.9f, 0.9f, 0.9f, 1.0f)}
};

struct Rect {
    float x_left, x_right, y_bottom, y_top, x_center, y_center, width, height;

    Rect() = default;

    static Rect fromBottomLeft(float x_left, float y_bottom, float width, float height) {
        Rect r;
        r.x_left = x_left;
        r.x_right = x_left + width;
        r.y_bottom = y_bottom;
        r.y_top = y_bottom + height;
        r.width = width;
        r.height = height;
        r.x_center = x_left + width * 0.5f;
        r.y_center = y_bottom + height * 0.5f;
        return r;
    }

    static Rect fromCenter(float x_center, float y_center, float width, float height) {
        Rect r;
        r.width = width;
        r.height = height;
        r.x_center = x_center;
        r.y_center = y_center;
        r.x_left = x_center - width * 0.5f;
        r.x_right = x_center + width * 0.5f;
        r.y_bottom = y_center - height * 0.5f;
        r.y_top = y_center + height * 0.5f;
        return r;
    }

    bool isIn(float x, float y) {
        return (std::abs(x - x_center) <= width * 0.5f && std::abs(y - y_center) <= height * 0.5f);
    }

    std::string toString() {
        return
            std::to_string(x_left) + " " +
            std::to_string(y_bottom) + " " +
            std::to_string(width) + " " +
            std::to_string(height) + " " +
            std::to_string(x_center) + " " +
            std::to_string(y_center);
    }
};


class Game {
public:
    Game();

    Game(const std::string& board_str, PlayerColor first_player);
    
    void init(); // only once glfw is init

    void render();

    void setScreenDims(int screen_width, int screen_height);

    void handleClick(float xpos, float ypos, int button, int action, int mods);

    void preActionCheck();

    std::unique_ptr<Board> board;
private:
    void renderBoard();

    void renderSidebar();

    bool isThreefoldRepetition(); // same position appeared 3 times (threefold repetition)

    void registerBoardState();

    void registerCaptured(Position& current_pos, const PossibleMove& move);

    void resignation(PlayerColor player);

    Color getPieceColor(PlayerColor pc);

    glm::mat4 projection;

    float tile_size = 80.0f;

    int screen_width;
    int screen_height;

    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TextRenderer> text_renderer;

    Position selected_tile;

    std::vector<PossibleMove> possible_moves;

    std::vector<Position> check_pieces;

    PlayerColor current_player;
    PlayerColor winner = PlayerColor::NONE;

    GameState game_state = GameState::RUNNING;
    GameResult game_result;

    // UI
    Rect board_rect;
    Rect sidebar;
    Rect promotion_rect;

    std::unordered_map<std::string, int> board_state_memory;

    bool action_performed = true; // prevent unnecessary pre-action checks - true to check for the first time

    std::array<int, NB_DIFF_PIECES * 2> captured_pieces;
};

constexpr size_t getColor(Color c) {
    return static_cast<size_t>(c);
}

glm::vec4 inline blendColors(const glm::vec4& top, const glm::vec4& bottom) {
    glm::vec3 c_top(top);
    glm::vec3 c_bottom(bottom);
    return glm::vec4(c_top * top.a + c_bottom * (1 - top.a), top.a + bottom.a * (1 - top.a));
}
