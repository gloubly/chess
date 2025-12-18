#include "game.hpp"

Game::Game(): selected_tile(), current_player(PlayerColor::WHITE) {
    board = std::make_unique<Board>();
    
    renderer = std::make_unique<Renderer>();

    setScreenDims(1, 1); // initialize Rects
}

Game::Game(const std::string& board_str, PlayerColor first_player) : selected_tile(), current_player(first_player)
{
    board = std::make_unique<Board>(board_str);
    renderer = std::make_unique<Renderer>();    

    setScreenDims(1,1); // initialize Rects
}

void Game::init() {
    // load textures
    ResourceManager::loadMultipleTextures("resources/textures/pieces.png", 6, 1, "pieces");
    ResourceManager::loadSingleTexture("resources/textures/circle.png", "circle");

    // load shaders
    ResourceManager::loadShader("resources/shaders/tile.vert", "resources/shaders/tile.frag", "", "tile");
    ResourceManager::loadShader("resources/shaders/sprite.vert", "resources/shaders/sprite.frag", "", "sprite");
    ResourceManager::loadShader("resources/shaders/text.vert", "resources/shaders/text.frag", "", "text");

    renderer->init();
    text_renderer = std::make_unique<TextRenderer>("resources/fonts/Super Croissant.ttf");
}

void Game::renderBoard() {
    Shader tile_shader = ResourceManager::getShader("tile");
    Shader sprite_shader = ResourceManager::getShader("sprite");

    std::vector<Texture> piece_textures = ResourceManager::getTextureArray("pieces");
    Texture circle_texture = ResourceManager::getTexture("circle");
    circle_texture.blurrySettings();
    // tiles
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Color tile_state = (row + col) % 2==0 ? Color::LIGHT : Color::DARK;
            glm::vec4 tile_color = COLOR_MAP.at(tile_state);
            
            Piece* p = board->at(col, row);
            if (p && p->pieceType == PieceType::KING && static_cast<King*>(p)->checked) {
                tile_color = blendColors(COLOR_MAP.at(Color::CHECK), tile_color);
            }
            else if (selected_tile.x == col && selected_tile.y == row) {
                tile_color = blendColors(COLOR_MAP.at(Color::SELECTED), tile_color);
            }

            float x = board_rect.x_left + (col + 0.5f) * tile_size;
            float y = board_rect.y_bottom + (7 - row + 0.5f) * tile_size; // board origin is top left

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, -0.5f));
            model = glm::scale(model, glm::vec3(tile_size, tile_size, 0.0f));

            glm::mat4 transform = projection * model;
            
            tile_shader.use();
            tile_shader.setMat4("transform", transform);
            tile_shader.setVec4("color", tile_color);
            renderer->renderTile();
        }
    }

    // pieces
    const float piece_size = tile_size * 0.8f;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = board->at(col, row);
            if (p) {
                float x = board_rect.x_left + (col + 0.5f) * tile_size;
                float y = board_rect.y_bottom + (7 - row + 0.5f) * tile_size; // board origin is top left
                
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
                model = glm::scale(model, glm::vec3(piece_size));

                size_t tex_index = p->pieceType;
                Color piece_color = getPieceColor(p->color);
                piece_textures[tex_index].bind();
                sprite_shader.use().setMat4("transform", projection * model);
                sprite_shader.setVec4("color", COLOR_MAP.at(piece_color));
                renderer->renderSprite();
            }
        }
    }

    // possible moves
    const float marker_width = tile_size * 0.45f;
    for (auto& move : possible_moves) {
        float x = board_rect.x_left + (move.position.x + 0.5f) * tile_size;
        float y = board_rect.y_bottom + (7 - move.position.y + 0.5f) * tile_size; // board origin is top left
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
        model = glm::scale(model, glm::vec3(marker_width));

        circle_texture.bind();
        sprite_shader.use();
        sprite_shader.setMat4("transform", projection * model);
        sprite_shader.setVec4("color", COLOR_MAP.at(Color::HIGHLIGHTED));
        renderer->renderSprite();
    }
}

void Game::renderSidebar() {
    const float text_width = sidebar.width * 0.9f;
    const float square_size = sidebar.width * 0.3f;

    const float half_margin = tile_size * 0.05f;

    //container
    Shader tile_shader = ResourceManager::getShader("tile").use();
    tile_shader.setVec4("color", COLOR_MAP.at(Color::SIDEBAR));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(sidebar.x_center, sidebar.y_center, 0.0f));
    model = glm::scale(model, glm::vec3(sidebar.width, sidebar.height, 0.0f));
    tile_shader.setMat4("transform", projection * model);
    renderer->renderTile();

    //text
    Shader text_shader = ResourceManager::getShader("text").use();
    text_shader.setMat4("projection", projection);
    text_shader.setVec4("color", COLOR_MAP.at(Color::WHITE));

    Anchor text_anchor = { sidebar.x_center, sidebar.y_center + half_margin };
    TextAlign text_align = { TextHorizontalAlign::CENTER, TextVerticalAlign::BOTTOM };
    text_renderer->render("current player", text_anchor, text_width, std::nullopt, text_align);
    
    // current player square
    Color piece_color = getPieceColor(current_player);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(sidebar.x_center, sidebar.y_center - half_margin - square_size/2, 0.0f));
    model = glm::scale(model, glm::vec3(square_size));
    tile_shader.use();
    tile_shader.setVec4("color", COLOR_MAP.at(piece_color));
    tile_shader.setMat4("transform", projection * model);
    renderer->renderTile();

    //captured pieces
    const float bar_padding_y = tile_size;
    const float padding_x = tile_size * 0.1f;
    const float small_piece_size = tile_size * 0.3f;
    const float diff_piece_margin_x = small_piece_size * 0.2f;
    const float piece_margin_y = tile_size * 0.1f;
    const float y_step = small_piece_size + piece_margin_y;
    std::vector<Texture> piece_textures = ResourceManager::getTextureArray("pieces");
    Shader sprite_shader = ResourceManager::getShader("sprite");

    auto [black_score, white_score] = board->getPlayerScores();
    PlayerColor best_player = (black_score > white_score) ? PlayerColor::BLACK : PlayerColor::WHITE;
    int score_diff = std::abs(black_score - white_score);

    float x_start, y_start, x_offset, y_offset;

    int y_direction;

    const auto advance = [&](float advance_x) {
        if (x_offset + advance_x + small_piece_size > sidebar.width - 2 * padding_x) {
            x_offset = 0;
            y_offset += y_step;
        }
        else x_offset += advance_x;
    };

    for (auto& opp_player : { PlayerColor::WHITE, PlayerColor::BLACK }) {
        Color opp_piece_color = getPieceColor(opp_player);

        if (opp_player == PlayerColor::BLACK) {
            x_offset = 0;
            y_offset = 0;
            for (size_t i = 0; i < NB_DIFF_PIECES; ++i) {
                int count = captured_pieces[i + toIndex(opp_player) * NB_DIFF_PIECES];
                if (count == 0) continue;
                for (int k = 0; k < count - 1; ++k) {
                    advance(small_piece_size);
                }
                advance(diff_piece_margin_x + small_piece_size);
            }
            y_start = board_rect.y_bottom + bar_padding_y + y_offset + small_piece_size/2;
        }
        else y_start = board_rect.y_top - bar_padding_y - small_piece_size / 2;

        x_offset = 0;
        y_offset = 0;
        x_start = sidebar.x_left + padding_x + small_piece_size / 2;
        for (size_t i = 0; i < NB_DIFF_PIECES; ++i) {
            int count = captured_pieces[i + toIndex(opp_player) * NB_DIFF_PIECES];
            if (count == 0) continue;
            for (int k = 0; k < count; ++k) {

                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x_start + x_offset, y_start - y_offset, 0.0f));
                model = glm::scale(model, glm::vec3(small_piece_size));

                piece_textures[i].bind();
                sprite_shader.use().setMat4("transform", projection * model);
                sprite_shader.setVec4("color", COLOR_MAP.at(opp_piece_color));
                renderer->renderSprite();

                if (k < count - 1) advance(small_piece_size);
            }
            advance(diff_piece_margin_x + small_piece_size);
        }
        if (opp_player != best_player && score_diff > 0) {
            x_offset -= small_piece_size / 2; // remove textures center offset
            text_shader.use();
            text_shader.setMat4("projection", projection);
            text_shader.setVec4("color", COLOR_MAP.at(Color::WHITE));
            text_renderer->render("+" + std::to_string(score_diff), 
                { x_start + x_offset, y_start - y_offset },
                std::nullopt, small_piece_size,
                {TextHorizontalAlign::LEFT, TextVerticalAlign::CENTER}
            );
        }
    }
}

void Game::render() {
    renderBoard();
    renderSidebar();

    if (game_state == GameState::PROMOTION) {
        std::vector<Texture> piece_textures = ResourceManager::getTextureArray("pieces");
        Shader sprite_shader = ResourceManager::getShader("sprite");
        Shader tile_shader = ResourceManager::getShader("tile");

        const float banner_width = tile_size * 4;
        const float banner_height = tile_size;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(promotion_rect.x_center, promotion_rect.y_center, 0.0f));
        model = glm::scale(model, glm::vec3(banner_width, banner_height, 0.0f));

        tile_shader.use();
        tile_shader.setMat4("transform", projection * model);
        tile_shader.setVec4("color", COLOR_MAP.at(Color::TRANSLUSCENT_GRAY));
        renderer->renderTile();

        float offset = promotion_rect.height * 0.5f;
        for (size_t i = 0; i < 4; ++i) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(promotion_rect.x_left + i * promotion_rect.height + offset, promotion_rect.y_center, 0.0f));
            model = glm::scale(model, glm::vec3(promotion_rect.height));

            size_t tex_index = promotion_order[i];
            Color piece_color = getPieceColor(current_player);

            piece_textures[tex_index].bind();
            sprite_shader.use();
            sprite_shader.setMat4("transform", projection * model);
            sprite_shader.setVec4("color", COLOR_MAP.at(piece_color));
            renderer->renderSprite();
        }
    }
    else if (game_state == GameState::FINISHED) {
        std::string title;
        std::string subtitle;
        if (game_result == GameResult::CHECKMATE || game_result == GameResult::RESIGNATION) {
            std::string winner_str = playerToString(winner);
            winner_str[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(winner_str[0])));
            title = winner_str + " wins";
        }
        else {
            title = "Draw";
        }

        switch (game_result) {
            case GameResult::DRAW_STALEMATE:
                subtitle = "Stalemate";
                break;

            case GameResult::DRAW_REPETITION:
                subtitle = "Threefold repetition";
                break;

            case GameResult::DRAW_DEAD_POSITION:
                subtitle = "Dead position";
                break;

            case GameResult::DRAW_FIFTY_MOVES:
                subtitle = "Fifty-move rule";
                break;

            case GameResult::DRAW_AGREEMENT:
                subtitle = "Agreement";
                break;

            case GameResult::CHECKMATE:
                subtitle = "Checkmate";
                break;

            case GameResult::RESIGNATION:
                subtitle = "Resignation";
                break;
            default:
                throw std::runtime_error("wrong game result");
        }

        //rendering
        Shader tile_shader = ResourceManager::getShader("tile");
        Shader text_shader = ResourceManager::getShader("text");

        //overlay
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(board_rect.x_center, board_rect.y_center, 0.0f));
        model = glm::scale(model, glm::vec3(board_rect.width, board_rect.height, 0.0f));
        tile_shader.use();
        tile_shader.setMat4("transform", projection * model);
        tile_shader.setVec4("color", COLOR_MAP.at(Color::TRANSLUSCENT_GRAY));
        renderer->renderTile();

        //texts
        Anchor title_anchor = { board_rect.x_center, board_rect.y_center };
        Anchor subtitle_anchor = { board_rect.x_center, board_rect.y_center };
        TextAlign title_align = { TextHorizontalAlign::CENTER, TextVerticalAlign::BOTTOM };
        TextAlign subtitle_align = { TextHorizontalAlign::CENTER, TextVerticalAlign::TOP };
        const float title_height = tile_size * 0.03f;
        const float subtitle_height = tile_size * 0.01f;

        text_shader.use();
        text_shader.setMat4("projection", projection);
        text_shader.setVec4("color", COLOR_MAP.at(Color::WHITE));
        text_renderer->render(title, title_anchor, title_height, title_align);
        text_renderer->render(subtitle, subtitle_anchor, subtitle_height, subtitle_align);
        
    }
}

void Game::setScreenDims(int screen_width, int screen_height) {
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    projection = glm::ortho(0.0f, static_cast<float>(screen_width), 0.0f, static_cast<float>(screen_height), -1.0f, 1.0f);

    const float screen_padding_factor = 0.9f;

    float margin_x = 0.1f * tile_size;
    
    float board_length = 8 * tile_size;

    const float sidebar_width = tile_size * 2;

    const float ui_width = board_length + margin_x + sidebar_width;
    const float ui_height = board_length;

    const float scale = std::min(
        screen_padding_factor * screen_width / ui_width,
        screen_padding_factor * screen_height / ui_height
    );

    board_length *= scale;
    tile_size *= scale;
    margin_x *= scale;


    board_rect = Rect::fromBottomLeft(
        (this->screen_width - ui_width * scale) * 0.5f,
        (this->screen_height - ui_height * scale) * 0.5f,
        board_length,
        board_length
    );

    sidebar = Rect::fromBottomLeft(
        board_rect.x_left + board_length + margin_x,
        board_rect.y_bottom,
        sidebar_width * scale,
        board_length
    );

    promotion_rect = Rect::fromCenter(
        board_rect.x_center,
        board_rect.y_center,
        tile_size * 4,
        tile_size
    );

    if (std::abs(promotion_rect.height * 4 - promotion_rect.width) > 1e-5f) {
        throw std::runtime_error("invalid promotion_rect dimensions : " + promotion_rect.toString());
    }
}

void Game::handleClick(float xpos, float ypos, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    ypos = screen_height - ypos;

    if (game_state == GameState::PROMOTION) {
        if (board->at(selected_tile)->pieceType != PieceType::PAWN) {
            throw std::runtime_error("selected piece at " + selected_tile.toString() + " should be a pawn ");
        }

        if (!promotion_rect.isIn(xpos, ypos)) return;

        auto x_tile = static_cast<int>((xpos - (promotion_rect.x_center - promotion_rect.width / 2)) / promotion_rect.height);
        if (x_tile < 0 || x_tile > 4) {
            throw std::runtime_error("x_tile invalid:" + x_tile);
        }
        if (possible_moves.size() != 1) {
            throw std::runtime_error("invalid possible moves size: " + possible_moves.size());
        }
        registerCaptured(selected_tile, possible_moves[0]);
        board->promote(selected_tile, promotion_order[x_tile]);
        board->movePiece(selected_tile, possible_moves[0]);
        current_player = otherPlayer(current_player);
        action_performed = true;
        game_state = GameState::RUNNING;
    }
    else if (game_state == GameState::RUNNING) {
        if (!board_rect.isIn(xpos, ypos)) return;
        auto tile_clicked = Position(
            static_cast<int>((xpos - board_rect.x_left) / tile_size),
            7 - static_cast<int>((ypos - board_rect.y_bottom) / tile_size) // board origin is top left
        );
        if (isOnBoard(tile_clicked)) {
            auto it = std::find_if(possible_moves.begin(), possible_moves.end(), [&tile_clicked](auto& pm) {return pm.position==tile_clicked; });
            
            if (it != possible_moves.end()) {
                const PossibleMove& move = *it;

                if (move.moveType == MoveType::PROMOTE) {
                    game_state = GameState::PROMOTION;
                    possible_moves = { move };
                    return;
                }
                
                if (game_state != GameState::PROMOTION) {
                    registerCaptured(selected_tile, move);
                    board->movePiece(selected_tile, move);
                    current_player = otherPlayer(current_player);
                    action_performed = true;
                }
            }

            // select/deselect a tile
            if (selected_tile == tile_clicked) {
                selected_tile.clear();
                possible_moves.clear();
            }
            else {
                Piece* p = board->at(tile_clicked);
                if (p && p->color == current_player) {
                    possible_moves = board->filterLegalMoves(p->position);
                    selected_tile = tile_clicked;
                }
            }
        }
    }
}

bool Game::isThreefoldRepetition() {
    return std::any_of(board_state_memory.begin(), board_state_memory.end(), [](auto const& e) { return e.second >= 3; });
}

void Game::registerBoardState() {
    std::string board_state = board->getState(current_player);

    if (board_state_memory.find(board_state) == board_state_memory.end()) {
        board_state_memory[board_state] = 1;
    }
    else {
        board_state_memory[board_state]++;
    }
}

void Game::preActionCheck() {
    if (!action_performed) return;

    selected_tile.clear();
    possible_moves.clear();

    board->updateChecks();
    registerBoardState();
    board->disableEnPassant(current_player);

    if (board->is50Moves()) {
        game_result = GameResult::DRAW_FIFTY_MOVES;
        game_state = GameState::FINISHED;
    }
    else if (board->isDeadPosition()) {
        game_result = GameResult::DRAW_DEAD_POSITION;
        game_state = GameState::FINISHED;
    }
    else if (board->isStalemate(current_player)) {
        game_result = GameResult::DRAW_STALEMATE;
        game_state = GameState::FINISHED;
    }
    else if (isThreefoldRepetition()) {
        game_result = GameResult::DRAW_REPETITION;
        game_state = GameState::FINISHED;
        
    }
    else if (board->isCheckmate(current_player)) {
        game_result = GameResult::CHECKMATE;
        game_state = GameState::FINISHED;
        winner = otherPlayer(current_player);
    }
    action_performed = false;
}

void Game::registerCaptured(Position& current_pos, const PossibleMove& move) {
    Position next_pos;
    if (move.moveType == MoveType::EN_PASSANT) {
        next_pos = Position(current_pos.x, move.position.y);
    }
    else next_pos = move.position;

    Piece* p = board->at(next_pos);
    if (p) {
        size_t index = toIndex(p->color) * NB_DIFF_PIECES + p->pieceType;
        captured_pieces[index]++;
    }
}

void Game::resignation(PlayerColor player) {
    winner = otherPlayer(player);
    game_result = GameResult::RESIGNATION;
    game_state = GameState::FINISHED;
}

Color Game::getPieceColor(PlayerColor pc) {
    return (pc == PlayerColor::WHITE) ? Color::WHITE_PIECE : Color::BLACK_PIECE;
}
